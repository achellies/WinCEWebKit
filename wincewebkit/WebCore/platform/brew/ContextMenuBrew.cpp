/*
 * Copyright (C) 2006, 2007 Apple Inc.  All rights reserved.
 * Copyright (C) 2010 Company 100, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "ContextMenu.h"

#include "NotImplemented.h"
#include <wtf/text/CString.h>

namespace WebCore {

ContextMenu::ContextMenu(const HitTestResult& result)
    : m_hitTestResult(result)
{
    ASSERT_NOT_REACHED();
    notImplemented();
}

ContextMenu::ContextMenu(const HitTestResult& result, const PlatformMenuDescription menu)
    : m_hitTestResult(result)
{
    ASSERT_NOT_REACHED();
    notImplemented();
}

ContextMenu::~ContextMenu()
{
    ASSERT_NOT_REACHED();
    notImplemented();
}

unsigned ContextMenu::itemCount() const
{
    ASSERT_NOT_REACHED();
    return 0;
}

void ContextMenu::insertItem(unsigned int position, ContextMenuItem& item)
{
    ASSERT_NOT_REACHED();
    notImplemented();
}

void ContextMenu::appendItem(ContextMenuItem& item)
{
    ASSERT_NOT_REACHED();
    notImplemented();
}

ContextMenuItem* ContextMenu::itemWithAction(unsigned action)
{
    ASSERT_NOT_REACHED();
    notImplemented();
    return 0;
}

ContextMenuItem* ContextMenu::itemAtIndex(unsigned index, const PlatformMenuDescription platformDescription)
{
    ASSERT_NOT_REACHED();
    notImplemented();
    return 0;
}

void ContextMenu::setPlatformDescription(PlatformMenuDescription menu)
{
    ASSERT_NOT_REACHED();
    notImplemented();
}

PlatformMenuDescription ContextMenu::platformDescription() const
{
    ASSERT_NOT_REACHED();
    notImplemented();
    return 0;
}

PlatformMenuDescription ContextMenu::releasePlatformDescription()
{
    ASSERT_NOT_REACHED();
    notImplemented();
    return 0;
}

}
