/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WebInspectorProxy_h
#define WebInspectorProxy_h

#if ENABLE(INSPECTOR)

#include "APIObject.h"
#include "Connection.h"
#include <wtf/Forward.h>
#include <wtf/PassRefPtr.h>

#if PLATFORM(MAC)
#include <wtf/RetainPtr.h>
#ifdef __OBJC__
@class WKView;
#else
class WKView;
#endif
#endif

namespace WebKit {

class WebPageProxy;
struct WebPageCreationParameters;

class WebInspectorProxy : public APIObject {
public:
    static const Type APIType = TypeInspector;

    static PassRefPtr<WebInspectorProxy> create(WebPageProxy* page)
    {
        return adoptRef(new WebInspectorProxy(page));
    }

    ~WebInspectorProxy();

    void invalidate();

    // Public APIs
    WebPageProxy* page() { return m_page; }

    bool isVisible() const { return m_isVisible; }
    void show();
    void close();

    void showConsole();

    bool isAttached() const { return m_isAttached; }
    void attach();
    void detach();

    bool isDebuggingJavaScript() const { return m_isDebuggingJavaScript; }
    void toggleJavaScriptDebugging();

    bool isProfilingJavaScript() const { return m_isProfilingJavaScript; }
    void toggleJavaScriptProfiling();

    bool isProfilingPage() const { return m_isProfilingPage; }
    void togglePageProfiling();

#if ENABLE(INSPECTOR)
    // Implemented in generated WebInspectorProxyMessageReceiver.cpp
    void didReceiveWebInspectorProxyMessage(CoreIPC::Connection*, CoreIPC::MessageID, CoreIPC::ArgumentDecoder*);
    CoreIPC::SyncReplyMode didReceiveSyncWebInspectorProxyMessage(CoreIPC::Connection*, CoreIPC::MessageID, CoreIPC::ArgumentDecoder*, CoreIPC::ArgumentEncoder*);
#endif

private:
    WebInspectorProxy(WebPageProxy* page);

    virtual Type type() const { return APIType; }

    WebPageProxy* platformCreateInspectorPage();

    // Implemented the platform WebInspectorProxy file
    String inspectorPageURL() const;

    // Called by WebInspectorProxy messages
    void createInspectorPage(uint64_t& inspectorPageID, WebPageCreationParameters&);
    void didLoadInspectorPage();

    WebPageProxy* m_page;

    bool m_isVisible;
    bool m_isAttached;
    bool m_isDebuggingJavaScript;
    bool m_isProfilingJavaScript;
    bool m_isProfilingPage;

#if PLATFORM(MAC)
    RetainPtr<WKView> m_inspectorView;
#endif
};

} // namespace WebKit

#endif // ENABLE(INSPECTOR)

#endif // WebInspectorProxy_h
