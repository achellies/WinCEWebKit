/*
 * Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer. 
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution. 
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#import <WebKit/WebNSFileManagerExtras.h>

#import "WebKitNSStringExtras.h"
#import "WebNSURLExtras.h"
#import <JavaScriptCore/Assertions.h>
#import <WebKitSystemInterface.h>
#import <sys/stat.h>

@implementation NSFileManager (WebNSFileManagerExtras)

- (NSString *)_webkit_carbonPathForPath:(NSString *)posixPath
{
    OSStatus error;
    FSRef ref, rootRef, parentRef;
    FSCatalogInfo info;
    NSMutableArray *carbonPathPieces;
    HFSUniStr255 nameString;

    // Make an FSRef.
    error = FSPathMakeRef((const UInt8 *)[posixPath fileSystemRepresentation], &ref, NULL);
    if (error != noErr) {
        return nil;
    }

    // Get volume refNum.
    error = FSGetCatalogInfo(&ref, kFSCatInfoVolume, &info, NULL, NULL, NULL);
    if (error != noErr) {
        return nil;
    }

    // Get root directory FSRef.
    error = FSGetVolumeInfo(info.volume, 0, NULL, kFSVolInfoNone, NULL, NULL, &rootRef);
    if (error != noErr) {
        return nil;
    }

    // Get the pieces of the path.
    carbonPathPieces = [NSMutableArray array];
    for (;;) {
        error = FSGetCatalogInfo(&ref, kFSCatInfoNone, NULL, &nameString, NULL, &parentRef);
        if (error != noErr) {
            return nil;
        }
        [carbonPathPieces insertObject:[NSString stringWithCharacters:nameString.unicode length:nameString.length] atIndex:0];
        if (FSCompareFSRefs(&ref, &rootRef) == noErr) {
            break;
        }
        ref = parentRef;
    }

    // Volume names need trailing : character.
    if ([carbonPathPieces count] == 1) {
        [carbonPathPieces addObject:@""];
    }

    return [carbonPathPieces componentsJoinedByString:@":"];
}

typedef struct MetaDataInfo
{
    CFStringRef URLString;
    CFStringRef referrer;
    CFStringRef path;
} MetaDataInfo;

static void *setMetaData(void* context)
{
    MetaDataInfo *info = (MetaDataInfo *)context;
    WKSetMetadataURL((NSString *)info->URLString, (NSString *)info->referrer, (NSString *)info->path);

    if (info->URLString)
        CFRelease(info->URLString);
    if (info->referrer)
        CFRelease(info->referrer);
    if (info->path)
        CFRelease(info->path);
    
    free(info);
    return 0;
}

- (void)_webkit_setMetadataURL:(NSString *)URLString referrer:(NSString *)referrer atPath:(NSString *)path
{
    ASSERT(URLString);
    ASSERT(path);

    NSURL *URL = [NSURL _web_URLWithUserTypedString:URLString];
    if (URL)
        URLString = [[URL _web_URLByRemovingUserInfo] _web_userVisibleString];
 
    // Spawn a background thread for WKSetMetadataURL because this function will not return until mds has
    // journaled the data we're're trying to set. Depending on what other I/O is going on, it can take some
    // time. 
    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    MetaDataInfo *info = malloc(sizeof(MetaDataInfo));
    
    info->URLString = URLString ? CFStringCreateCopy(0, (CFStringRef)URLString) : 0;
    info->referrer = referrer ? CFStringCreateCopy(0, (CFStringRef)referrer) : 0;
    info->path = path ? CFStringCreateCopy(0, (CFStringRef)path) : 0;

    pthread_create(&tid, &attr, setMetaData, info);
    pthread_attr_destroy(&attr);
}

- (NSString *)_webkit_startupVolumeName
{
    NSString *path = [self _webkit_carbonPathForPath:@"/"];
    return [path substringToIndex:[path length]-1];
}

// -[NSFileManager fileExistsAtPath:] returns NO if there is a broken symlink at the path.
// So we use this function instead, which returns YES if there is anything there, including
// a broken symlink.
static BOOL fileExists(NSString *path)
{
    struct stat statBuffer;
    return !lstat([path fileSystemRepresentation], &statBuffer);
}

- (NSString *)_webkit_pathWithUniqueFilenameForPath:(NSString *)path
{
    // "Fix" the filename of the path.
    NSString *filename = [[path lastPathComponent] _webkit_filenameByFixingIllegalCharacters];
    path = [[path stringByDeletingLastPathComponent] stringByAppendingPathComponent:filename];

    if (fileExists(path)) {
        // Don't overwrite existing file by appending "-n", "-n.ext" or "-n.ext.ext" to the filename.
        NSString *extensions = nil;
        NSString *pathWithoutExtensions;
        NSString *lastPathComponent = [path lastPathComponent];
        NSRange periodRange = [lastPathComponent rangeOfString:@"."];
        
        if (periodRange.location == NSNotFound) {
            pathWithoutExtensions = path;
        } else {
            extensions = [lastPathComponent substringFromIndex:periodRange.location + 1];
            lastPathComponent = [lastPathComponent substringToIndex:periodRange.location];
            pathWithoutExtensions = [[path stringByDeletingLastPathComponent] stringByAppendingPathComponent:lastPathComponent];
        }

        for (unsigned i = 1; ; i++) {
            NSString *pathWithAppendedNumber = [NSString stringWithFormat:@"%@-%d", pathWithoutExtensions, i];
            path = [extensions length] ? [pathWithAppendedNumber stringByAppendingPathExtension:extensions] : pathWithAppendedNumber;
            if (!fileExists(path))
                break;
        }
    }

    return path;
}

@end

#ifdef BUILDING_ON_TIGER

@implementation NSFileManager (WebNSFileManagerTigerForwardCompatibility)

- (NSArray *)contentsOfDirectoryAtPath:(NSString *)path error:(NSError **)error
{
    // We don't report errors via the NSError* output parameter, so ensure that the caller does not expect us to do so.
    ASSERT_ARG(error, !error);

    return [self directoryContentsAtPath:path];
}

- (NSString *)destinationOfSymbolicLinkAtPath:(NSString *)path error:(NSError **)error
{
    // We don't report errors via the NSError* output parameter, so ensure that the caller does not expect us to do so.
    ASSERT_ARG(error, !error);

    return [self pathContentOfSymbolicLinkAtPath:path];
}

- (NSDictionary *)attributesOfFileSystemForPath:(NSString *)path error:(NSError **)error
{
    // We don't report errors via the NSError* output parameter, so ensure that the caller does not expect us to do so.
    ASSERT_ARG(error, !error);

    return [self fileSystemAttributesAtPath:path];
}

- (NSDictionary *)attributesOfItemAtPath:(NSString *)path error:(NSError **)error
{
    // We don't report errors via the NSError* output parameter, so ensure that the caller does not expect us to do so.
    ASSERT_ARG(error, !error);

    return [self fileAttributesAtPath:path traverseLink:NO];
}

- (BOOL)moveItemAtPath:(NSString *)srcPath toPath:(NSString *)dstPath error:(NSError **)error
{
    // The implementation of moveItemAtPath:toPath:error: interacts with the NSFileManager's delegate.
    // We are not matching that behaviour at the moment, but it should not be a problem as any client
    // expecting that would need to call setDelegate: first which will generate a compile-time warning,
    // as that method is not available on Tiger.
    return [self movePath:srcPath toPath:dstPath handler:nil];
}

- (BOOL)removeItemAtPath:(NSString *)path error:(NSError **)error
{
    // The implementation of removeItemAtPath:error: interacts with the NSFileManager's delegate.
    // We are not matching that behaviour at the moment, but it should not be a problem as any client
    // expecting that would need to call setDelegate: first which will generate a compile-time warning,
    // as that method is not available on Tiger.
    return [self removeFileAtPath:path handler:nil];
}

@end

#endif
