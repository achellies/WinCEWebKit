#ifndef WebView_h
#define WebView_h

#include "IntRect.h"
#include "OwnPtr.h"
#include "PassRefPtr.h"

namespace WTF {
class String;
}

namespace WebCore {
class Frame;
class Page;
class FrameView;
class HTMLFrameOwnerElement;
class KURL;
class ResourceRequest;
}

class WebView
{
public:
    WebView(HWND hwnd, bool enableGradients = true, bool enableDoubleBuffer = true);
    ~WebView();

    static void initialize(HINSTANCE instanceHandle);
    static void cleanup();

    HWND windowHandle() const { return m_windowHandle; }
    WebCore::Frame* frame() const { return m_frame; }
    WebCore::Page* page() const { return m_page; }
    WebCore::FrameView *view() const;

    void load(LPCWSTR url);
    void load(const WTF::String &url);
    void load(const WebCore::ResourceRequest &request);
    void reload();
    void stop();

    void frameRect(RECT* rect) const;

    PassRefPtr<WebCore::Frame> createFrame(const WebCore::KURL&, const WTF::String&, WebCore::HTMLFrameOwnerElement*, const WTF::String&, bool, int, int);

    // JavaScript Dialog
    void runJavaScriptAlert(const WTF::String& message);
    bool runJavaScriptConfirm(const WTF::String& message);
    bool runJavaScriptPrompt(const WTF::String& message, const WTF::String& defaultValue, WTF::String& result);

    void invalidateContents(const WebCore::IntRect&);

private:
    static LRESULT CALLBACK WebViewWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT WndProc(HWND, UINT, WPARAM, LPARAM);

    bool handlePaint(HWND hWnd);
    bool handleMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    bool handleMouseWheel(HWND hWnd, WPARAM wParam, LPARAM lParam, bool isHorizontal);
    bool handleKeyDown(WPARAM virtualKeyCode, LPARAM keyData, bool systemKeyDown);
    bool handleKeyPress(WPARAM charCode, LPARAM keyData, bool systemKeyDown);
    bool handleKeyUp(WPARAM virtualKeyCode, LPARAM keyData, bool systemKeyDown);

    void paint(HDC hDC, const WebCore::IntRect& clipRect);

    WebCore::Frame* m_frame;
    WebCore::Page* m_page;
    HWND m_parentWindowHandle;
    HWND m_windowHandle;
    bool m_enableDoubleBuffer;
    OwnPtr<HDC> m_doubleBufferDC;
    OwnPtr<HBITMAP> m_doubleBufferBitmap;
    WebCore::IntRect m_invalidatedRect;
};

#endif // WebView_h
