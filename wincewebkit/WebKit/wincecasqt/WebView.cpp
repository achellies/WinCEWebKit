#include "config.h"
#include "WebView.h"

#include "ChromeClientWinCE.h"
#include "ContextMenuClientWinCE.h"
#include "DragClientWinCE.h"
#include "EditorClientWinCE.h"
#include "FocusController.h"
#include "Frame.h"
#include "FrameLoader.h"
#include "FrameLoaderClientWinCE.h"
#include "FrameView.h"
#include "GraphicsContext.h"
#include "InitializeThreading.h"
#include "InspectorClientWinCE.h"
#include "IntSize.h"
#include "MainThread.h"
#include "NotImplemented.h"
#include "Page.h"
#include "PlatformKeyboardEvent.h"
#include "PlatformMouseEvent.h"
#include "PlatformWheelEvent.h"
#include "PlatformStrategiesWinCE.h"
#include "ResourceRequest.h"
#include "Settings.h"
#include "SharedBuffer.h"
#include "WebCoreInstanceHandle.h"

#include "resource.h"

// added by achellies, add the header file for RenderImage, RenderText, InlineTextBox, RenderView
#include "MemoryCache.h"
#include "renderimage.h"
#include "renderview.h"
#include "TextEvent.h"
#include "TextIterator.h"
#include "inlinetextbox.h"
#include "MouseEventWithHitTestResults.h"
#include "viewportarguments.h"

#if ENABLE(DIVIDE_PAGES)
// added by achellies, include the header file for stricmp
#include <wtf/StringExtras.h> 
#include <windows.h>
#include <windowsx.h>
#endif

#if ENABLE(WINCEONWINDOWS)
#include <tchar.h>
#endif

const LPCWSTR kWebViewWindowClassName = L"WebViewWindowClass";

using namespace WebCore;


#if ENABLE(DIVIDE_PAGES)


CEvent::CEvent(LPCTSTR lpszName, BOOL bManualReset, BOOL bInitialState)
{
	m_hEvent = CreateEvent(NULL,bManualReset,bInitialState,lpszName);
}

CEvent::~CEvent()
{
	if (m_hEvent != NULL)
	{
		CloseHandle(m_hEvent);
	}
}

bool CEvent::isSigned()
{
	bool IsSigneded = false;
	if (m_hEvent != NULL)
	{
		DWORD dwWaitResult = WaitForSingleObject(m_hEvent,1);
        switch (dwWaitResult) 
        {
		// The thread got ownership of the mutex
		case WAIT_OBJECT_0:
		// The thread got ownership of an abandoned mutex
		case WAIT_ABANDONED:
			break;
		case WAIT_TIMEOUT:
			IsSigneded = true;
			break;
        }
	}
	return IsSigneded;
}

bool CEvent::wait(DWORD dwTimeOut)
{
	bool IsSigneded = false;
	if (m_hEvent != NULL)
	{
		DWORD dwWaitResult = WaitForSingleObject(m_hEvent, dwTimeOut);
        switch (dwWaitResult) 
        {
		// The thread got ownership of the mutex
		case WAIT_OBJECT_0:
		// The thread got ownership of an abandoned mutex
		case WAIT_ABANDONED:
			IsSigneded = true;
			break;
		case WAIT_TIMEOUT:
			IsSigneded = false;
			break;
		case WAIT_FAILED:
			IsSigneded = false;
			break;
		default:
			break;
        }
		ResetEvent(m_hEvent);
	}
	else
	{
		IsSigneded = true;
	}

	return IsSigneded;
}

void CEvent::reset()
{
	if (m_hEvent != NULL)
	{
		ResetEvent(m_hEvent);
	}
}

void CEvent::set()
{
	if (m_hEvent != NULL)
	{
		SetEvent(m_hEvent);
	}
}

// added by achellies, this class is used to cache the divided pages rects
class WebViewPrivate {
public:
	WebViewPrivate()
		: m_nCurPage(0)
	{}

	bool pageAvailable() const {
		return (!m_pagesRect.isEmpty() && (m_nCurPage >= 0) && (m_nCurPage <= (static_cast<int>(m_pagesRect.size()) - 1)));
	}

	int curPage() const {
		return m_nCurPage;
	}

	void setCurPage(int nPage) {
		m_nCurPage = nPage;
	}

	void reset() {
		m_pagesRect.clear();
		m_pagesOffset.clear();
		m_nCurPage = 0;
		m_nMaximumText = 0;
	}

	void setMaximumText(int maximumText) {
		m_nMaximumText = maximumText;
	}

	int maximumText() {
		return m_nMaximumText;
	}

	void append(IntRect rect) {
		m_pagesRect.append(rect);
	}

	void addTextOffset(int offset) {
		m_pagesOffset.append(offset);
	}

	int getTextOffset(int nIndex) {
		if (pageAvailable() && (!m_pagesOffset.isEmpty() && (nIndex >= 0) && (nIndex <= (static_cast<int>(m_pagesOffset.size()) - 1))))
			return m_pagesOffset.at(nIndex);

		return 0;
	}

	int curPageOffset() {
		return getTextOffset(m_nCurPage);
	}

	int getPageByOffset(int offset) {
		int nPage = 0;
		if (pageAvailable())
			for (Vector<int>::const_iterator citer = m_pagesOffset.begin();
				citer != m_pagesOffset.end(); ++citer, ++nPage)
				if (offset < *citer) {
					nPage = (nPage > 0)?(nPage - 1):0;
					break;
				}
				else if (offset == *citer)
					break;

		return nPage;
	}

	IntRect lastPageRect() const {
		IntRect pageRect;
		if (pageAvailable())
			pageRect = m_pagesRect.last();
		return pageRect;
	}

	IntRect firstPageRect() const {
		IntRect pageRect;
		if (pageAvailable())
			pageRect = m_pagesRect.first();
		return pageRect;
	}

	IntRect curPageRect() const {
		IntRect pageRect;
		if (pageAvailable())
			pageRect = m_pagesRect.at(m_nCurPage);

		return pageRect;
	}

	IntRect pageRect(int nIndex) const {
		IntRect pageRect;
		if (pageAvailable())
			pageRect = m_pagesRect.at(nIndex);

		return pageRect;
	}

	int pagesCount() const {
		return static_cast<int>(m_pagesRect.size());
	}
private:
	// the current page index, start by 0
	int m_nCurPage;

	// the maximum text number for each page
	int m_nMaximumText;

	// the vector contains the pages rect information
	Vector<IntRect> m_pagesRect;

	// this vector contains the offset of each page
	Vector<int> m_pagesOffset;
};

namespace WebCore {

IntRect FrameView::visibleContentRect(bool includeScrollbars) const
{
	if (m_data && m_data->pageAvailable())
	{
		return IntRect(IntPoint(m_data->curPageRect().x(), m_data->curPageRect().y()),
					   IntSize(max(0, m_data->curPageRect().width() - (verticalScrollbar() && !includeScrollbars ? verticalScrollbar()->width() : 0)), 
							   max(0, m_data->curPageRect().height() - (horizontalScrollbar() && !includeScrollbars ? horizontalScrollbar()->height() : 0))));
	}
	
	return __super::visibleContentRect(includeScrollbars);
}

} // namespace WebCore

#endif

struct MyDlgParam
{
    String message;
    String value;
};

static INT_PTR CALLBACK PromptWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        {
            MyDlgParam *dlgParam = reinterpret_cast<MyDlgParam*>(lParam);
            SetDlgItemTextW(hDlg, IDC_PROMPT_TITLE, dlgParam->message.charactersWithNullTermination());
            SetDlgItemTextW(hDlg, IDC_PROMPT_TEXT, dlgParam->value.charactersWithNullTermination());
            SetDlgItemTextW(hDlg, IDOK, L"OK");
            SetDlgItemTextW(hDlg, IDCANCEL, L"Cancel");
            SetWindowLong(hDlg, DWL_USER, lParam);
            dlgParam->value = String();
        }
        return 0;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            if (LOWORD(wParam) == IDOK)
            {
                MyDlgParam *dlgParam = reinterpret_cast<MyDlgParam*>(GetWindowLong(hDlg, DWL_USER));

                HWND hInput = GetDlgItem(hDlg, IDC_PROMPT_TEXT);
                if (dlgParam && hInput)
                {
                    UChar* characters;
                    int length = GetWindowTextLengthW(hInput);
                    dlgParam->value = String::createUninitialized(length, characters);
                    GetWindowTextW(hInput, characters, length);
                }
            }

            EndDialog(hDlg, LOWORD(wParam));
            return 0;
        }
        break;
    case WM_CLOSE:
        EndDialog(hDlg, message);
        return 0;
    }

    return DefDlgProc(hDlg, message, wParam, lParam);
}

LRESULT CALLBACK WebView::WebViewWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (WebView* webView = reinterpret_cast<WebView*>(GetWindowLong(hWnd, 0)))
        return webView->WndProc(hWnd, message, wParam, lParam);

    return DefWindowProc(hWnd, message, wParam, lParam);
}

PassRefPtr<SharedBuffer> loadResourceIntoBuffer(const char* name)
{
    notImplemented();
    return 0;
}


WebView::WebView(HWND hwnd, bool enableGradients, bool enableDoubleBuffer, bool enableReaderMode)
    : m_frame(0)
    , m_page(0)
    , m_parentWindowHandle(hwnd)
    , m_enableDoubleBuffer(enableDoubleBuffer)
#if ENABLE(DIVIDE_PAGES)
	, m_data(0)
	, m_enableReaderMode(enableReaderMode)
	, m_zoomMultiplier(1.0f)
    , m_zoomMultiplierIsTextOnly(true)
	, m_isLButtonDown(false)
	, m_layoutEvent(IREADER_LAYOUT_EVENT, TRUE, TRUE)
	, m_syncData(IREADER_SYNC_DATA, TRUE, TRUE)
#endif
{
    RECT rcClient;
    GetClientRect(hwnd, &rcClient);

#if ENABLE(DIVIDE_PAGES)
	// added by achellies, we just create the screen size view window
	m_windowHandle = CreateWindow(kWebViewWindowClassName, 0, WS_CHILD /*| WS_HSCROLL | WS_VSCROLL*/,
		0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), hwnd, 0, WebCore::instanceHandle(), 0);
#else
	m_windowHandle = CreateWindow(kWebViewWindowClassName, 0, WS_CHILD /*| WS_HSCROLL | WS_VSCROLL*/,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, hwnd, 0, WebCore::instanceHandle(), 0);
#endif

    SetWindowLong(m_windowHandle, 0, reinterpret_cast<LONG>(this));

    MoveWindow(m_windowHandle, 0, 0, rcClient.right, rcClient.bottom, TRUE);
    ShowWindow(m_windowHandle, SW_SHOW);

    GraphicsContext::setGradientsEnabled(enableGradients);

    Page::PageClients pageClients;
    pageClients.chromeClient = new WebKit::ChromeClientWinCE(this);
    pageClients.contextMenuClient = new WebKit::ContextMenuClientWinCE(this);
    pageClients.editorClient = new WebKit::EditorClientWinCE(this);
    pageClients.dragClient = new WebKit::DragClientWinCE();
    pageClients.inspectorClient = new WebKit::InspectorClientWinCE(this);
    m_page = new Page(pageClients);

    Settings* settings = m_page->settings();

	// modified by achellies
	// we change the default fixed font size fro 14 to 32,
	// default font size from 14 to 32, minimun font size from 8 to 24
	// minimum logical font size from 8 to 24, and disable the javascript
    settings->setDefaultFixedFontSize(32);
    settings->setDefaultFontSize(32);
    settings->setMinimumFontSize(24);
    settings->setMinimumLogicalFontSize(24);
    settings->setJavaScriptEnabled(false);
    settings->setLoadsImagesAutomatically(true);

#if ENABLE(DIVIDE_PAGES)
	// added by achellies
	m_data = new WebViewPrivate();
	if (view())
		view()->setWebViewPrivate(m_data);

	m_page->setCookieEnabled(false);

	// Refer To : MemoryCache.cpp
	const int cDefaultCacheCapacity = 5 * 1024 * 1024;

	settings->setMaximumDecodedImageSize(static_cast<size_t>(cDefaultCacheCapacity / 5));
	settings->setMediaEnabled(false);
	settings->setJavaEnabled(false);
	settings->setJavaScriptEnabled(false);
	settings->setPluginsEnabled(false);
	settings->setLocalStorageEnabled(false);
	settings->setUsesPageCache(false);

	if (cache())
		cache()->setCapacities(0, cDefaultCacheCapacity, cDefaultCacheCapacity);
#endif

    WebKit::FrameLoaderClientWinCE *loaderClient = new WebKit::FrameLoaderClientWinCE(this);
    RefPtr<Frame> frame = Frame::create(m_page, 0, loaderClient);
    m_frame = frame.get();
    loaderClient->setFrame(m_frame);

    m_page->mainFrame()->init();

	if (view()) {
		RECT windowRect;
		frameRect(&windowRect);
		view()->resize(IntRect(windowRect).size());
	}
}

WebView::~WebView()
{
#if ENABLE(DIVIDE_PAGES)
	view()->setWebViewPrivate(0);
	delete m_data, m_data = 0;
#endif
    delete m_page;
    DestroyWindow(m_windowHandle);
}

void WebView::initialize(HINSTANCE instanceHandle)
{
    JSC::initializeThreading();
    WTF::initializeMainThread();
    PlatformStrategiesWinCE::initialize();

    WebCore::setInstanceHandle(instanceHandle);

    WNDCLASS wc;
    wc.style          = CS_DBLCLKS;
    wc.lpfnWndProc    = WebView::WebViewWndProc;
    wc.cbClsExtra     = 0;
    wc.cbWndExtra     = sizeof(void *);
    wc.hInstance      = instanceHandle;
    wc.hIcon          = 0;
    wc.hCursor        = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground  = 0;
    wc.lpszMenuName   = 0;
    wc.lpszClassName  = kWebViewWindowClassName;

    RegisterClass(&wc);
}

void WebView::cleanup()
{
    UnregisterClass(kWebViewWindowClassName, WebCore::instanceHandle());
}

PassRefPtr<Frame> WebView::createFrame(const KURL& url, const String& name, HTMLFrameOwnerElement* ownerElement, const String& referrer,
                                       bool /*allowsScrolling*/, int /*marginWidth*/, int /*marginHeight*/)
{
    Frame* coreFrame = m_frame;

    WebKit::FrameLoaderClientWinCE *loaderClient = new WebKit::FrameLoaderClientWinCE(this);
    RefPtr<Frame> childFrame = Frame::create(m_page, ownerElement, loaderClient);
    loaderClient->setFrame(childFrame.get());

    coreFrame->tree()->appendChild(childFrame);
    childFrame->tree()->setName(name);
    childFrame->init();

    // The creation of the frame may have run arbitrary JavaScript that removed it from the page already.
    if (!childFrame->page())
        return 0;

    childFrame->loader()->loadURLIntoChildFrame(url, referrer, childFrame.get());

    // The frame's onload handler may have removed it from the document.
    if (!childFrame->tree()->parent())
        return 0;

    return childFrame.release();
}

void WebView::runJavaScriptAlert(const String& message)
{
    String newStr(message);
    MessageBoxW(m_windowHandle, newStr.charactersWithNullTermination(), 0, MB_OK | MB_ICONEXCLAMATION);
}

bool WebView::runJavaScriptConfirm(const String& message)
{
    String newStr(message);
    return MessageBoxW(m_windowHandle, newStr.charactersWithNullTermination(), 0, MB_OKCANCEL | MB_ICONQUESTION) == IDOK;
}

bool WebView::runJavaScriptPrompt(const String& message, const String& defaultValue, String& result)
{
    MyDlgParam dlgParam;
    dlgParam.message = message;
    dlgParam.value = defaultValue;
    DialogBoxParamW(WebCore::instanceHandle(), MAKEINTRESOURCE(IDD_PROMPT), m_windowHandle, PromptWndProc, (LPARAM)&dlgParam);

    if (dlgParam.value.isNull())
        return false;

    result = dlgParam.value;
    return true;
}

void WebView::invalidateContents(const IntRect& updateRect)
{
    m_invalidatedRect.unite(updateRect);
}

void WebView::frameRect(RECT* rect) const
{
    GetWindowRect(m_windowHandle, rect);
}

FrameView *WebView::view() const
{
    return m_frame ? m_frame->view() : 0;
}

void WebView::load(LPCWSTR url)
{
#if ENABLE(DIVIDE_PAGES)
	// added by achellies
	Settings* settings = m_page->settings();

	if (frame() && frame()->loader() && (frame()->loader()->isLoading() || frame()->loader()->isLoadingMainResource()))
		stop();

	if (cache()) {
		cache()->setDisabled(true);
		cache()->setDisabled(false);
	}

	if (m_enableReaderMode && view()) {
		//view()->setScrollbarModes(ScrollbarAlwaysOff, ScrollbarAlwaysOff);
		//view()->setWasScrolledByUser(true);

		if (settings && g_iReaderData && (_tcslen(g_iReaderData->m_szFontFamily) > 0) && (g_iReaderData->m_nFontSize > 0)) {
			String fontFamily = settings->standardFontFamily();
			if (fontFamily.isEmpty() || (_tcsicmp(fontFamily.charactersWithNullTermination(), g_iReaderData->m_szFontFamily) != 0))
				settings->setStandardFontFamily(g_iReaderData->m_szFontFamily);

			settings->setDefaultFixedFontSize(g_iReaderData->m_nFontSize);
			settings->setDefaultFontSize(g_iReaderData->m_nFontSize);
			settings->setMinimumFontSize(g_iReaderData->m_nFontSize);
			settings->setMinimumLogicalFontSize(g_iReaderData->m_nFontSize);
		}

		if (g_iReaderData) {
			g_iReaderData->m_hasSelectionText = false;
			g_iReaderData->m_nCurPage = 1;
			g_iReaderData->m_nPagesCount = 1;
			g_iReaderData->m_bUnderSearch = false;
			g_iReaderData->m_nNextMatches = 0;
			g_iReaderData->m_nPrevMatches = 0;
		}

		if (g_stringManager)
			g_stringManager.Uninitialize();

		clearTextSelection();

		if (m_data)
		{
			m_data->reset();
		}

		m_postilMaker.SetInfo(windowHandle(), g_iReaderData->m_szPostilPath);
	}
#endif
	load(String(url));
}

void WebView::load(const String &url)
{
    load(WebCore::ResourceRequest(url));
}

void WebView::load(const WebCore::ResourceRequest &request)
{
    frame()->loader()->load(request, false);
}

void WebView::reload()
{
    frame()->loader()->reload();
}

void WebView::stop()
{
    frame()->loader()->stopAllLoaders();
}

void WebView::paint(HDC hDC, const IntRect& clipRect)
{
    OwnPtr<HRGN> clipRgn(CreateRectRgn(clipRect.x(), clipRect.y(), clipRect.right(), clipRect.bottom()));
    SelectClipRgn(hDC, clipRgn.get());

    GraphicsContext gc(hDC);
    view()->paint(&gc, clipRect);
}

bool WebView::handlePaint(HWND hWnd)
{
#if ENABLE(DIVIDE_PAGES)
	// added by achellies
	bool hasUpdateRect = true;
	HDC hDC = NULL;
	RECT updateRect;
	PAINTSTRUCT ps;
	if (::GetUpdateRect(hWnd, &updateRect, false)) {        
		hDC = BeginPaint(m_windowHandle, &ps);		
	}
	else {
		hDC = GetDC(hWnd);
		hasUpdateRect = false;
		GetClientRect(m_windowHandle, &updateRect);
	}	
	RECT rcClient;
	GetClientRect(m_windowHandle, &rcClient);
	IntRect clipRect(rcClient);

	if (m_data && m_data->pageAvailable() && m_enableDoubleBuffer) {
		if (!m_doubleBufferDC) {

			m_doubleBufferDC = adoptPtr(CreateCompatibleDC(hDC));
			m_doubleBufferBitmap = adoptPtr(CreateCompatibleBitmap(hDC, rcClient.right, rcClient.bottom));
			SelectObject(m_doubleBufferDC.get(), m_doubleBufferBitmap.get());			
		}
		FillRect(m_doubleBufferDC.get(), &rcClient, static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));

		OwnPtr<HRGN> clipRgn(CreateRectRgn(clipRect.x(), clipRect.y(), clipRect.right(), clipRect.bottom()));
		SelectClipRgn(m_doubleBufferDC.get(), clipRgn.get());

		GraphicsContext gc(m_doubleBufferDC.get());
		view()->paint(&gc, clipRect);

		int nScrollbarWidth = view()->verticalScrollbar()?view()->verticalScrollbar()->width():0;		

		IntSize offseSize= view()->scrollOffset();

		int nTopAdjust = m_data->curPageRect().y() - offseSize.height();

		bool hasUnPaintedArea = false;
		if (m_data->curPageRect().height() < clipRect.height())
			hasUnPaintedArea = true;

		clipRect.setY(rcClient.top + nTopAdjust);
		clipRect.setHeight(m_data->curPageRect().height());

		if (hasUnPaintedArea) {
			RECT unPaintRect = rcClient;
			unPaintRect.top = m_data->curPageRect().height();
			FillRect(m_doubleBufferDC.get(), &unPaintRect, static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));
		}

		m_postilMaker.Draw(m_doubleBufferDC.get());

		if (g_iReaderData->m_bHasBookmark) {
			RECT rcLabel;
			rcLabel.left = rcClient.right - 40;
			rcLabel.right = rcClient.right - 20;
			rcLabel.top = rcClient.top;
			rcLabel.bottom = rcLabel.top + 20;

			HBRUSH hGrayBrush = (HBRUSH) GetStockObject(GRAY_BRUSH); 

			POINT pt[4];
			pt[0].x = rcLabel.left;
			pt[0].y = rcLabel.top;
			pt[1].x = rcLabel.right;
			pt[1].y = rcLabel.top;
			pt[2].x = rcLabel.right;
			pt[2].y = rcLabel.bottom;
			pt[3] = pt[0];
			SelectObject(m_doubleBufferDC.get(),hGrayBrush);
			Polygon(m_doubleBufferDC.get(),pt,4);

			HPEN hGrayPen = (HPEN)CreatePen(PS_SOLID,1,RGB(128,128,128));
			HPEN HOldPen = (HPEN)SelectObject(m_doubleBufferDC.get(),hGrayPen);
			MoveToEx(m_doubleBufferDC.get(),rcLabel.left,rcLabel.top,NULL);
			LineTo(m_doubleBufferDC.get(),rcLabel.left,rcLabel.bottom);

			LineTo(m_doubleBufferDC.get(),rcLabel.right,rcLabel.bottom);

			SelectObject(m_doubleBufferDC.get(),HOldPen);
			DeleteObject(hGrayPen);
		}

		BitBlt(hDC, rcClient.left, rcClient.top, clipRect.width() - nScrollbarWidth, rcClient.bottom - rcClient.top, 
			m_doubleBufferDC.get(), clipRect.x(), clipRect.y(), SRCCOPY);
	}
#if ENABLE(WINCEONWINDOWS)
	else {
		if (m_enableDoubleBuffer) {
			if (!m_doubleBufferDC) {
				RECT rcClient;
				GetClientRect(m_windowHandle, &rcClient);

				m_doubleBufferDC = adoptPtr(CreateCompatibleDC(hDC));
				m_doubleBufferBitmap = adoptPtr(CreateCompatibleBitmap(hDC, rcClient.right, rcClient.bottom));
				SelectObject(m_doubleBufferDC.get(), m_doubleBufferBitmap.get());
				FillRect(m_doubleBufferDC.get(), &rcClient, static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));
			}

			if (m_invalidatedRect.intersects(clipRect)) {
				paint(m_doubleBufferDC.get(), m_invalidatedRect);
				m_invalidatedRect = IntRect();
			}

			BitBlt(hDC, clipRect.x(), clipRect.y(), clipRect.width(), clipRect.height(), m_doubleBufferDC.get(), clipRect.x(), clipRect.y(), SRCCOPY);
		} else
			paint(hDC, clipRect);
	}
#endif

	if (hasUpdateRect)
	{
		EndPaint(m_windowHandle, &ps);
	}
	else
	{
		ReleaseDC(m_windowHandle, hDC);
	}
#else
    RECT updateRect;
    if (::GetUpdateRect(hWnd, &updateRect, false)) {
        PAINTSTRUCT ps;
        HDC hDC = BeginPaint(m_windowHandle, &ps);

        IntRect clipRect(updateRect);

        if (m_enableDoubleBuffer) {
            if (!m_doubleBufferDC) {
                RECT rcClient;
                GetClientRect(m_windowHandle, &rcClient);

                m_doubleBufferDC = adoptPtr(CreateCompatibleDC(hDC));
                m_doubleBufferBitmap = adoptPtr(CreateCompatibleBitmap(hDC, rcClient.right, rcClient.bottom));
                SelectObject(m_doubleBufferDC.get(), m_doubleBufferBitmap.get());
            }

            if (m_invalidatedRect.intersects(clipRect)) {
                paint(m_doubleBufferDC.get(), m_invalidatedRect);
				m_invalidatedRect = IntRect();
			}

			BitBlt(hDC, clipRect.x(), clipRect.y(), clipRect.width(), clipRect.height(), m_doubleBufferDC.get(), clipRect.x(), clipRect.y(), SRCCOPY);
		} else
			paint(hDC, clipRect);

		EndPaint(m_windowHandle, &ps);
		return true;
	}
#endif
	return false;
}

#if ENABLE(DIVIDE_PAGES)

void WebView::clearCache()
{
	stop();

	m_postilMaker.Save();
	cache()->setDisabled(true);
}

WebCore::Node* WebView::selectLeafNode(const WebCore::IntPoint& absolutePoint, WebCore::Node* node)
{
	if (node && node->renderer()) {
		IntRect pressNodeRect = node->getRect();
		if (node->renderer() && node->renderer()->enclosingBox())
			pressNodeRect = node->renderer()->enclosingBox()->absoluteContentBox();

		if (node->renderer()->isText()
			|| node->renderer()->isTextArea()
			|| node->renderer()->isTextField()
			|| node->renderer()->isRubyText()
#if defined(ENABLE_SVG) && ENABLE_SVG
			|| node->renderer()->isSVGText()
			|| node->renderer()->isSVGInlineText()
#endif
			) {
				Vector<IntRect> leafNodesRect;
				node->renderer()->absoluteRects(leafNodesRect, pressNodeRect.x(), pressNodeRect.y());
				for (Vector<IntRect>::const_iterator citer = leafNodesRect.begin(); citer != leafNodesRect.end(); ++citer)
				{
					if (citer->contains(absolutePoint))
						return node;
				}		
		}
		else if (node->renderer()->isImage()
			|| node->renderer()->isVideo()
			|| node->renderer()->isTable()
#if defined(ENABLE_SVG) && ENABLE_SVG
			|| node->renderer()->isSVGPath()
			|| node->renderer()->isSVGImage()
#endif
			){
				if (pressNodeRect.contains(absolutePoint))
					return node;
		}
	}

	return NULL;
}

WebCore::Node* WebView::walkThroughRecrusionNode(const WebCore::IntPoint& absolutePoint, WebCore::Node* node)
{
	if (node) {
		for (; node; node = node->nextSibling()) {
			if (node->hasChildNodes()) {
				WebCore::Node* leafNode = walkThroughRecrusionNode(absolutePoint, node->firstChild());
				if (leafNode != NULL)
					return leafNode;
			} else {
				WebCore::Node* leafNode = selectLeafNode(absolutePoint, node);
				if (leafNode != NULL)
					return leafNode;
			}
		}
	}

	return NULL;
}

WTF::String WebView::selectNode(const WebCore::IntPoint& clickedPoint, bool useRelativePostion)
{
	WTF::String pressedText;
	if (!view() || !frame() || !frame()->document())
		return pressedText;

	WebCore::IntPoint absolutePoint = clickedPoint;
	IntRect pageRect;
	if (m_data && m_data->pageAvailable()) {
		pageRect = m_data->curPageRect();
		if (useRelativePostion) {
			absolutePoint.setX(pageRect.x() + clickedPoint.x());
			absolutePoint.setY(pageRect.y() + clickedPoint.y());
		}
	} else {
		RECT rcClient = {0};
		IntSize scrollSize = view()->scrollOffset();
		if (useRelativePostion) {
			absolutePoint.setX(clickedPoint.x() + scrollSize.width());
			absolutePoint.setY(clickedPoint.y() + scrollSize.height());
		}

		GetClientRect(m_windowHandle, &rcClient);
		pageRect.setX(scrollSize.width());
		pageRect.setY(scrollSize.height());
		pageRect.setWidth(rcClient.right - rcClient.left);
		pageRect.setHeight(rcClient.bottom - rcClient.top);
	}

	Document* doc = frame()->document();
	if (doc && doc->firstChild()) {
		WebCore::Node* mousePressedNode = walkThroughRecrusionNode(absolutePoint, doc->firstChild());

		if (mousePressedNode && mousePressedNode->renderer()) {
			if (mousePressedNode->renderer()->isText()) {
				WebCore::RenderText* pTextObject = toRenderText(mousePressedNode->renderer());
				IntRect pressNodeRect = mousePressedNode->getRect();
				WTF::String nodeText = pTextObject->text();

				if (mousePressedNode->renderer() && mousePressedNode->renderer()->enclosingBox())
					pressNodeRect = mousePressedNode->renderer()->enclosingBox()->absoluteContentBox();

				for (InlineTextBox* pTextBox = pTextObject->firstTextBox(); pTextBox; pTextBox = pTextBox->nextTextBox()) {
					IntRect textRect(pTextBox->x(), pressNodeRect.y() + pTextBox->y(), pTextBox->width(), pTextBox->height());
					if (textRect.contains(absolutePoint.x(), absolutePoint.y())) {
						unsigned start = pTextBox->start();
						unsigned end = pTextBox->end();
						unsigned len = pTextBox->len();
						if (absolutePoint.x() >= textRect.x()) {
							unsigned textWidth = static_cast<unsigned>(pTextBox->width() / len);
							unsigned textoffset = static_cast<unsigned>((absolutePoint.x() - textRect.x()) / textWidth);
							pressedText = nodeText.substring(start + textoffset, 1 + (((absolutePoint.x() - textRect.x()) % textWidth))?1:0);
						}
					}
				}
			}

			IntRect pressNodeRect = mousePressedNode->getRect();
			RenderObject* render = mousePressedNode->renderer();

			if (mousePressedNode->renderer() && mousePressedNode->renderer()->enclosingBox())
				pressNodeRect = mousePressedNode->renderer()->enclosingBox()->absoluteContentBox();

			HDC hDC = GetDC(m_windowHandle);
			if (!pressNodeRect.isEmpty()) {
				//Rectangle(hDC, pressNodeRect.x(), pressNodeRect.y(), pressNodeRect.right(), rect.bottom());
			}
			if (render) {
				if (render->isImage()) {
					RenderImage* pImage = toRenderImage(render);
					Rectangle(hDC, pressNodeRect.x(), pressNodeRect.y() - pageRect.y(), pressNodeRect.right(), pressNodeRect.bottom() - pageRect.y());
				}
				else if (render->isText()) {
					RenderText* pText = toRenderText(render);
					InlineTextBox* pTextBox = pText->firstTextBox();
					while (pTextBox != NULL) {
						int width = pTextBox->width();

						int xPos = pTextBox->x();

						int yPos = pTextBox->y();

						int height = pTextBox->height();

						MoveToEx(hDC, pressNodeRect.x() + xPos, pressNodeRect.y() + yPos + height - pageRect.y(), NULL);
						LineTo(hDC, pressNodeRect.x() + xPos + width, pressNodeRect.y() + yPos + height - pageRect.y());

						pTextBox = pTextBox->nextTextBox();
					}
				}
				ReleaseDC(m_windowHandle, hDC);
			}

		}
	}

	return pressedText;
}

void WebView::getLeafNodeRect(Vector<WebCore::IntRect>& leafNodesRect, WebCore::Node* node)
{
	if (node && node->renderer()) {
		WebCore::IntRect baseRect =  node->getRect();
		if (node->renderer() && node->renderer()->enclosingBox())
			baseRect = node->renderer()->enclosingBox()->absoluteContentBox();

		if (node->renderer()->isText()
			|| node->renderer()->isTextArea()
			|| node->renderer()->isTextField()
			|| node->renderer()->isRubyText()
#if defined(ENABLE_SVG) && ENABLE_SVG
			|| node->renderer()->isSVGText()
			|| node->renderer()->isSVGInlineText()
#endif
			) {
			node->renderer()->absoluteRects(leafNodesRect, baseRect.x(), baseRect.y());
		}
		else if (node->renderer()->isImage()
			|| node->renderer()->isVideo()
			|| node->renderer()->isTable()
#if defined(ENABLE_SVG) && ENABLE_SVG
			|| node->renderer()->isSVGPath()
			|| node->renderer()->isSVGImage()
#endif
			){
			leafNodesRect.append(baseRect);
		}
		else
			leafNodesRect.append(baseRect);
	}
	return;
}

void WebView::walkThroughRecrusionNode(Vector<WebCore::IntRect>& leafNodesRect, WebCore::Node* node)
{
	if (node) {
		for (; node; node = node->nextSibling()) {
			if (node->hasChildNodes()) {
				if (node->renderer() && node->renderer()->isImage()) {
					WebCore::IntRect parentRect = node->getRect();
					leafNodesRect.append(parentRect);
				} else {
					walkThroughRecrusionNode(leafNodesRect, node->firstChild());
				}
			} else {
				getLeafNodeRect(leafNodesRect, node);
			}
		}
	}

	return;
}

int WebView::dividePages(HWND hWnd)
{
	if (view())
		view()->setWebViewPrivate(m_data);

	int nTotalPage = 0;
	RECT rcClient = {0};
	frameRect(&rcClient);
	WebCore::IntSize pageSize(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
	if (!view() || !frame() || !m_data || pageSize.isEmpty())
		return nTotalPage;

	m_data->reset();

	int contentsWidth = view()->contentsWidth();
	int contentsHeight = view()->contentsHeight();

	int visibleWidth = view()->visibleWidth();
	int visibleHeight = view()->visibleHeight();

	//_tprintf(_T("WebView::dividePages, contentsWidth = %d, contentsHeight = %d, visibleWidth = %d, visibleHeight = %d\n"),
	//	contentsWidth, contentsHeight, visibleWidth, visibleHeight);

	//_tprintf(_T("WebView::dividePages, rcClient.left = %d, rcClient.top = %d, rcClient.right = %d, rcClient.bottom=%d\n"),
	//	rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);

	int nPageCount = static_cast<int>(contentsHeight / pageSize.height()) + ((contentsHeight % pageSize.height())?1:0);

	Document* doc = frame()->document();
	if (doc && doc->firstChild()) {
		Vector<WebCore::IntRect> leafNodesRect;
		walkThroughRecrusionNode(leafNodesRect, doc->firstChild());
		if (!leafNodesRect.isEmpty()) {
			WebCore::IntRect pageRect(0, 0, pageSize.width(), pageSize.height());
			for (int i = 0; i < (nPageCount + 3); ++i) {
				int nHeight = 0;
				if (m_data->pageAvailable()) {
					pageRect.setY(m_data->lastPageRect().y() + m_data->lastPageRect().height());
					pageRect.setHeight(pageSize.height());
					nHeight = pageRect.height();
				} else {
					nHeight = pageSize.height();
				}

				bool bHasLeafNode = false;				
				Vector<WebCore::IntRect> intersectsRect;
				for (Vector<WebCore::IntRect>::const_iterator citer = leafNodesRect.begin(); citer != leafNodesRect.end(); ++citer) {
					if (pageRect.contains(*citer)) {
						bHasLeafNode = true;
						if (nHeight < citer->bottom())
							nHeight = citer->bottom();
					} else {
						if (pageRect.intersects(*citer))
							intersectsRect.append(*citer);
					}
				}

				if (!bHasLeafNode)
				{
					nHeight = pageRect.y() + pageSize.height();
					for (Vector<WebCore::IntRect>::const_iterator citer = leafNodesRect.begin(); citer != leafNodesRect.end(); ++citer) {
						if (citer->contains(pageRect) || citer->intersects(pageRect)) {
							bHasLeafNode = true;
							break;
						}
					}
				}

				if (!bHasLeafNode)
					break;

				int yPos = pageRect.y() + pageRect.height();
				if (intersectsRect.isEmpty()) {
					pageRect.setHeight(nHeight - pageRect.y());
				} else {
					for (Vector<WebCore::IntRect>::const_iterator citer = intersectsRect.begin(); citer != intersectsRect.end(); ++citer) {
						if ((yPos > citer->y()) && (yPos < (citer->y() + citer->height())))
							yPos = citer->y();
					}

					if (yPos <= pageRect.y()) {
						pageRect.setY(m_data->lastPageRect().y() + m_data->lastPageRect().height());
					} else {
						pageRect.setHeight(yPos - pageRect.y());
					}
				}

				m_data->append(pageRect);
			}
		}
	}

	//_tprintf(_T("WebView::dividePages, pageCount = %d.\n"), m_data->pagesCount());
	if (m_data->pagesCount() > 0) {

		WTF::String text;
		int nOffset = 0;
		int nMaximumText = 0;
		m_data->addTextOffset(nOffset);
		for (int i = 0; i < m_data->pagesCount() - 1; ++i) {
			text = getPageText(i);
			nOffset += text.length();
			if (nMaximumText < text.length())
			{
				nMaximumText = text.length();
			}
			m_data->addTextOffset(nOffset);
		}
		if (m_data->pagesCount() == 1) {
			nMaximumText = getPageText(0).length();
		}
		m_data->setMaximumText(nMaximumText);

		if (g_iReaderData) {
			g_iReaderData->m_nPagesCount = m_data->pagesCount();
			g_iReaderData->m_nMaximumText = m_data->maximumText();
		}
	}

	return (nTotalPage = m_data->pagesCount());
}

WTF::String WebView::getRecursionRenderObjectText(int nIndex, WebCore::RenderText* pText)
{
	WTF::String text;
	
	if (pText && pText->node()) {

		IntRect rect = pText->node()->getRect();
		if (pText->enclosingBox())
			rect = pText->enclosingBox()->absoluteContentBox();

		String strText = pText->text();
		for (InlineTextBox* pTextBox = pText->firstTextBox(); pTextBox; pTextBox = pTextBox->nextTextBox()) {
			IntRect textRect(pTextBox->x(), rect.y() + pTextBox->y(), pTextBox->width(), pTextBox->height());
			if (m_data->pageRect(nIndex).contains(textRect)) {
				unsigned start = pTextBox->start();
				unsigned end = pTextBox->end();
				unsigned len = pTextBox->len();
				text += strText.substring(start, len);
			}
		}

		for (RenderObject* child = pText->firstChild(); child && child->isText(); child = child->nextSibling()) {
			text += getRecursionRenderObjectText(nIndex, toRenderText(child));
		}
	}

	return text;
}

WTF::String WebView::getRecrusionNodeText(int nIndex, WebCore::Node* node)
{
	WTF::String text;
	if (node) {
		for (; node; node = node->nextSibling()) {
			if (node->hasChildNodes()) {
				text += getRecrusionNodeText(nIndex, node->firstChild());
			}
			else if (node->renderer() && node->renderer()->isText()) {
				text += getRecursionRenderObjectText(nIndex, toRenderText(node->renderer()));
			}
		}
	}

	return text;
}

WTF::String WebView::getPageText(int nIndex)
{
	WTF::String text;

	if (!view() || !frame() || !m_data || !m_data->pageAvailable()) {
		return text;
	}

	Document* doc = frame()->document();
	if (doc && doc->firstChild()) {
		text = getRecrusionNodeText(nIndex, doc->firstChild());
	}

	return text;
}

WTF::String WebView::selectedText()
{
	WTF::String selectedText;
	if (frame() && view())
		selectedText = frame()->selectedText();

	return selectedText;
}

int WebView::pageCount()
{
	return (m_data && m_data->pageAvailable())?m_data->pagesCount():0;
}

int WebView::curPage()
{
	return (m_data && m_data->pageAvailable())?m_data->curPage():0;
}

bool WebView::firstPage()
{
	if (m_data && m_data->pageAvailable())
		return navigatePage(0);

	return false;
}

bool WebView::lastPage()
{
	if (m_data && m_data->pageAvailable())
		return navigatePage(m_data->pagesCount() - 1);

	return false;
}

bool WebView::nextPage()
{
	if (m_data && m_data->pageAvailable()) {

		int nCurPage = curPage();
		++nCurPage;
		if (nCurPage > m_data->pagesCount() - 1)
		{
			nCurPage = m_data->pagesCount() - 1;
		}

		return navigatePage(nCurPage);
	}

	return false;
}

bool WebView::prevPage()
{
	if (m_data && m_data->pageAvailable()) {

		int nCurPage = curPage();
		--nCurPage;
		if (nCurPage < 0)
		{
			nCurPage = 0;
		}

		return navigatePage(nCurPage);
	}

	return false;
}

bool WebView::navigatePage(int nIndex)
{
	if (m_data && m_data->pageAvailable()) {

		if (view() && g_iReaderData)
			m_postilMaker.SetZoom(g_iReaderData->m_nFontSize, g_iReaderData->m_nPagesCount);
		
		g_iReaderData->m_bCurrentPageHasPostil = m_postilMaker.GetPageData(nIndex);

		m_data->setCurPage(nIndex);

		clearTextSelection();

		view()->setScrollPosition(IntPoint(m_data->curPageRect().x(), m_data->curPageRect().y()));
		invalidateContents(m_data->curPageRect());

		InvalidateRect(m_windowHandle, NULL, TRUE);

		if (g_iReaderData)
			g_iReaderData->m_nCurPage = nIndex;

		return true;
	}

	return false;
}

int WebView::getPageNumberByOffset(int nOffset)
{
	if (m_data && m_data->pageAvailable())
		return m_data->getPageByOffset(nOffset);

	return 0;
}

int WebView::getPageOffset(int nPage)
{
	if (m_data && m_data->pageAvailable())
		return m_data->getTextOffset(nPage);

	return 0;
}

int WebView::getCurrOffset()
{
	if (m_data && m_data->pageAvailable())
		return m_data->curPageOffset();

	return 0;
}

bool WebView::getPageBeginText(int nPage)
{
	if (m_data && m_data->pageAvailable()) {
		WTF::String pageText = getPageText(nPage);
		if (!pageText.isEmpty() && g_iReaderData)
			memcpy(g_iReaderData->m_szSearchText, pageText.charactersWithNullTermination(), MAX_PATH);

		return true;
	}

	return false;
}

bool WebView::getPageTTSText(int nPage)
{
	if (m_data && m_data->pageAvailable()) {
		if (!g_stringManager)
			g_stringManager.Initialize(m_data->maximumText());

		WTF::String pageText = getPageText(nPage);
		if (g_stringManager && !pageText.isEmpty())
			g_stringManager.FillBuffer(pageText.charactersWithNullTermination(), pageText.length());

		return true;
	}

	return false;
}

void WebView::clearTextSelection()
{
	if (page()) {
		page()->removeAllVisitedLinks();
		if (!g_iReaderData || !g_iReaderData->m_bUnderSearch)
			page()->unmarkAllTextMatches();
	}

	if (frame() && frame()->selection())
		frame()->selection()->clear();

	if (g_iReaderData)
		g_iReaderData->m_hasSelectionText = false;
}

bool WebView::setTextSizeMultiplier(float multiplier)
{
    if (!m_frame)
        return false;
    setZoomMultiplier(multiplier, true);
    return true;
}

bool WebView::setPageSizeMultiplier(float multiplier)
{
    if (!m_frame)
        return false;
    setZoomMultiplier(multiplier, false);
    return true;
}

void WebView::setZoomMultiplier(float multiplier, bool isTextOnly)
{
    m_zoomMultiplier = multiplier;
    m_zoomMultiplierIsTextOnly = isTextOnly;
	if (m_frame) {        
		if (m_zoomMultiplierIsTextOnly) {
			m_frame->setTextZoomFactor(multiplier);
			m_frame->setPageZoomFactor(multiplier);
		} else {
			m_frame->setTextZoomFactor(multiplier);
		}
	}
}

void WebView::textSizeMultiplier(float* multiplier)
{
    *multiplier = zoomMultiplier(true);
}

void WebView::pageSizeMultiplier(float* multiplier)
{
    *multiplier = zoomMultiplier(false);
}

float WebView::zoomMultiplier(bool isTextOnly)
{
    if (isTextOnly != m_zoomMultiplierIsTextOnly)
        return 1.0f;
    return m_zoomMultiplier;
}


// FIXME: This code should move into WebCore so it can be shared by all the WebKits.
#define MinimumZoomMultiplier   0.5f
#define MaximumZoomMultiplier   3.0f
#define ZoomMultiplierRatio     1.2f

bool WebView::canMakeTextLarger()
{
    return canZoomIn(true);
}

bool WebView::canZoomPageIn()
{
	return canZoomIn(false);
}

bool WebView::canZoomIn(bool isTextOnly)
{
    return zoomMultiplier(isTextOnly) * ZoomMultiplierRatio < MaximumZoomMultiplier;
}

bool WebView::makeTextLarger()
{
	return zoomIn(true);
}

bool WebView::zoomPageIn()
{
    return zoomIn(false);
}

bool WebView::zoomIn(bool isTextOnly)
{
    if (!canZoomIn(isTextOnly))
        return false;
    setZoomMultiplier(zoomMultiplier(isTextOnly) * ZoomMultiplierRatio, isTextOnly);
    return true;
}

bool WebView::canMakeTextSmaller()
{
    return canZoomOut(true);
}

bool WebView::canZoomPageOut()
{
    return canZoomOut(false);
}

bool WebView::canZoomOut(bool isTextOnly)
{
    return zoomMultiplier(isTextOnly) / ZoomMultiplierRatio > MinimumZoomMultiplier;
}

bool WebView::makeTextSmaller()
{
	return zoomOut(false);
}

bool WebView::zoomPageOut()
{
    return zoomOut(false);
}

bool WebView::zoomOut(bool isTextOnly)
{
    if (!canZoomOut(isTextOnly))
        return false;
    setZoomMultiplier(zoomMultiplier(isTextOnly) / ZoomMultiplierRatio, isTextOnly);
    return true;
}

bool WebView::canMakeTextStandardSize()
{
    return canResetZoom(true);
}

bool WebView::canResetPageZoom()
{
    return canResetZoom(false);
}

bool WebView::canResetZoom(bool isTextOnly)
{
    return zoomMultiplier(isTextOnly) != 1.0f;
}

bool WebView::makeTextStandardSize()
{
    return resetZoom(true);
}

bool WebView::resetPageZoom()
{
    return resetZoom(false);
}

bool WebView::resetZoom(bool isTextOnly)
{
    if (!canResetZoom(isTextOnly))
        return false;
    setZoomMultiplier(1.0f, isTextOnly);
    return true;
}

void WebView::didLayout()
{
	if (view() && g_iReaderData && (_tcslen(g_iReaderData->m_szFilePath) > 0)) {
		m_layoutEvent.set();
		view()->setScrollbarModes(ScrollbarAlwaysOff, ScrollbarAuto);
	}
}

void WebView::fontSizeZoom()
{
	if (m_data)
		m_data->reset();

	clearTextSelection();

	Settings* settings = m_page->settings();

	if (settings && g_iReaderData && (_tcslen(g_iReaderData->m_szFontFamily) > 0) && (g_iReaderData->m_nFontSize > 0)) {
		String fontFamily = settings->standardFontFamily();
		if (fontFamily.isEmpty() || (_tcsicmp(fontFamily.charactersWithNullTermination(), g_iReaderData->m_szFontFamily) != 0))
			settings->setStandardFontFamily(g_iReaderData->m_szFontFamily);

		settings->setDefaultFixedFontSize(g_iReaderData->m_nFontSize);
		settings->setDefaultFontSize(g_iReaderData->m_nFontSize);
		settings->setMinimumFontSize(g_iReaderData->m_nFontSize);
		settings->setMinimumLogicalFontSize(g_iReaderData->m_nFontSize);
	}

	if (view())
		view()->forceLayout();
}

bool WebView::findString(const String& target, TextCaseSensitivity caseSensitivity, bool shouldHighlight, unsigned limit, unsigned& matches, Vector<unsigned>& matchedPages)
{
	bool found = false;

	if (!view() || !frame() || !page())
		return false;

	matches = 0;
	matchedPages.clear();

	bool caseFlag = caseSensitivity == TextCaseSensitive;
	limit = limit ? (limit - matches) : 0;
	bool markMatches = true;

	Frame* frame = page()->mainFrame();
	do {
		frame->editor()->setMarkedTextMatchesAreHighlighted(shouldHighlight);
		//matches += frame->editor()->countMatchesForText(target, caseSensitivity == TextCaseSensitive, limit ? (limit - matches) : 0, true);

		RefPtr<Range> searchRange(rangeOfContents(m_frame->document()));

		ExceptionCode exception = 0;
		unsigned matchCount = 0;
		do {
			RefPtr<Range> resultRange(findPlainText(searchRange.get(), target, true, caseFlag));
			if (resultRange->collapsed(exception)) {
				if (!resultRange->startContainer()->isInShadowTree())
					break;

				searchRange = rangeOfContents(m_frame->document());
				searchRange->setStartAfter(resultRange->startContainer()->shadowAncestorNode(), exception);
				continue;
			}

			// Only treat the result as a match if it is visible
			if (frame->editor()->insideVisibleArea(resultRange.get())) {
				++matchCount;
				if (markMatches)
					frame->editor()->frame()->document()->markers()->addMarker(resultRange.get(), DocumentMarker::TextMatch);
				
				IntRect resultRect = resultRange->boundingBox();
				if (m_data && m_data->pageAvailable()) {
					for (int nIndex = 0; nIndex < m_data->pagesCount(); ++nIndex) {
						if (m_data->pageRect(nIndex).contains(resultRect)
							&& (matchedPages.find(nIndex) == static_cast<size_t>(-1))) {
							matchedPages.append(nIndex);
							break;
						}
					}
				}
			}

			// Stop looking if we hit the specified limit. A limit of 0 means no limit.
			if (limit > 0 && matchCount >= limit)
				break;

			// Set the new start for the search range to be the end of the previous
			// result range. There is no need to use a VisiblePosition here,
			// since findPlainText will use a TextIterator to go over the visible
			// text nodes. 
			searchRange->setStart(resultRange->endContainer(exception), resultRange->endOffset(exception), exception);

			Node* shadowTreeRoot = searchRange->shadowTreeRootNode();
			if (searchRange->collapsed(exception) && shadowTreeRoot)
				searchRange->setEnd(shadowTreeRoot, shadowTreeRoot->childNodeCount(), exception);
		} while (true);

		if (markMatches) {
			// Do a "fake" paint in order to execute the code that computes the rendered rect for each text match.
			if (frame->editor()->frame()->view() && frame->editor()->frame()->contentRenderer()) {
				frame->editor()->frame()->document()->updateLayout(); // Ensure layout is up to date.
				IntRect visibleRect = frame->editor()->frame()->view()->visibleContentRect();
				if (!visibleRect.isEmpty()) {
					GraphicsContext context((PlatformGraphicsContext*)0);
					context.setPaintingDisabled(true);
					frame->editor()->frame()->view()->paintContents(&context, visibleRect);
				}
			}
		}
		matches += matchCount;
		frame = frame->tree()->traverseNextWithWrap(false);
	} while (frame);

	return (matches > 0)?true:false;
}

#endif

bool WebView::handleMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static LONG globalClickCount;
    static IntPoint globalPrevPoint;
    static MouseButton globalPrevButton;
    static LONG globalPrevMouseDownTime;

#if ENABLE(DIVIDE_PAGES)
	// added by achellies
	if (m_enableReaderMode && !(g_iReaderData && (g_iReaderData->m_bPostil || g_iReaderData->m_bExcerpt || g_iReaderData->m_bTranslate)))
	{
		return true;
	}

#if ENABLE(WINCEONWINDOWS)
	bool bSeperateHtmlPages = false;
	WebCore::IntPoint absolutePoint;
	WebCore::IntPoint clickedPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	WebCore::IntSize scrollSize = view()->scrollOffset();
	WebCore::IntRect relativePageRect;

	if (m_data && m_data->pageAvailable()) {
		relativePageRect = m_data->curPageRect();
		int nTopAdjust = relativePageRect.y() - scrollSize.height();

		absolutePoint.setX(relativePageRect.x() + clickedPoint.x());
		absolutePoint.setY(relativePageRect.y() + clickedPoint.y());

		relativePageRect.setX(relativePageRect.x() - scrollSize.width());
		relativePageRect.setY(relativePageRect.y() - scrollSize.height());

		if (nTopAdjust > 0) {
			// the last page
			relativePageRect.setY(relativePageRect.y() - nTopAdjust);
		}

		bSeperateHtmlPages = true;
	} else {
		RECT rcClient = {0};

		absolutePoint.setX(clickedPoint.x() + scrollSize.width());
		absolutePoint.setY(clickedPoint.y() + scrollSize.height());

		GetClientRect(m_windowHandle, &rcClient);
		relativePageRect.setX(rcClient.left);
		relativePageRect.setY(rcClient.top);
		relativePageRect.setWidth(rcClient.right - rcClient.left);
		relativePageRect.setHeight(rcClient.bottom - rcClient.top);
	}

	if (bSeperateHtmlPages && !relativePageRect.contains(clickedPoint) && 
		(g_iReaderData && !g_iReaderData->m_bPostil && ((message == WM_LBUTTONDOWN) || (message == WM_MOUSEMOVE) || (message == WM_LBUTTONUP))))
		return true;
#endif

#endif

    // Create our event.
    // On WM_MOUSELEAVE we need to create a mouseout event, so we force the position
    // of the event to be at (MINSHORT, MINSHORT).
    PlatformMouseEvent mouseEvent(hWnd, message, wParam, lParam);

    bool insideThreshold = abs(globalPrevPoint.x() - mouseEvent.pos().x()) < ::GetSystemMetrics(SM_CXDOUBLECLK) &&
                           abs(globalPrevPoint.y() - mouseEvent.pos().y()) < ::GetSystemMetrics(SM_CYDOUBLECLK);
    LONG messageTime = 0;//::GetMessageTime();

    bool handled = false;
    if (message == WM_LBUTTONDOWN || message == WM_MBUTTONDOWN || message == WM_RBUTTONDOWN)
    {
#if ENABLE(DIVIDE_PAGES) && ENABLE(WINCEONWINDOWS)
		// added by achellies
		if (g_iReaderData && !g_iReaderData->m_bPostil && message == WM_LBUTTONDOWN) {
			selectNode(absolutePoint/*IntPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))*/, false);
		}
		else if (m_data && (message == WM_RBUTTONDOWN)) {
			if (!m_data->pageAvailable()) {
				dividePages(hWnd);

				firstPage();

				return true;
			}
			int nPageCount = m_data->pagesCount();

			WTF::String text;
			for (int i = 0; i < nPageCount; ++i)
				text = getPageText(i);

			if ((nPageCount > 0) && m_data && m_data->pageAvailable()) {
				int nCurPage = curPage();
				++nCurPage;
				if (nCurPage > m_data->pagesCount() - 1)
					nCurPage = 0;

				m_data->setCurPage(nCurPage);

				clearTextSelection();

				m_frame->view()->setScrollPosition(IntPoint(m_data->curPageRect().x(), m_data->curPageRect().y()));
				invalidateContents(m_data->curPageRect());

				InvalidateRect(m_windowHandle, NULL, TRUE);
				return true;
			}

			return true;
		}
		else if (message == WM_MBUTTONDOWN) {
			WTF::String screenText = selectNode(absolutePoint/*IntPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))*/, false);
			if (!screenText.isEmpty()) {
				WTF::String text;
				int nOffset = 0;
				if (m_data && m_data->pageAvailable()) {
					for (int i = 0; i <= curPage(); ++i) {
						text = getPageText(i);
						nOffset += text.length();
					}
				}

				//TCHAR szBuf[MAX_PATH] = {0};
				//_stprintf(szBuf, TEXT("当前取词为：\t%s\n当前页偏移量：\t%d\n当前页字数：\t%d"), screenText.charactersWithNullTermination(), nOffset, text.length());
				//MessageBox(m_windowHandle, szBuf, TEXT("取词"), MB_OK);

				if (page()) {
					unsigned matches = 0;
					Vector<unsigned> matchedPages;
					findString(screenText, TextCaseSensitive, true, 0, matches, matchedPages);
					//page()->findString(screenText, TextCaseInsensitive, FindDirectionForward, true);
					//page()->markAllMatchesForText(screenText, TextCaseInsensitive, true, 0);
					if (matches > 0)
						for (Vector<unsigned>::const_iterator citer = matchedPages.begin(); citer != matchedPages.end(); ++citer) {
							//_tprintf(_T("index = %d.\n"), *citer);
						}
				}
			}
			return true;
		}
#endif
#if ENABLE(DIVIDE_PAGES)
		m_isLButtonDown = true;

		if (g_iReaderData && g_iReaderData->m_bPostil && (message == WM_LBUTTONDOWN)) {
			clearTextSelection();
			WebCore::IntPoint clickedPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			m_postilMaker.TraceStart(clickedPoint, g_iReaderData->m_nPostilPenWidth, g_iReaderData->m_bErasePostil);
			return true;
		}
#endif
        // FIXME: I'm not sure if this is the "right" way to do this
        // but without this call, we never become focused since we don't allow
        // the default handling of mouse events.
        SetFocus(m_windowHandle);

        PlatformMouseEvent moveEvent(hWnd, WM_MOUSEMOVE, 0, lParam, false);
        moveEvent.setClickCount(0);
        m_page->mainFrame()->eventHandler()->handleMouseMoveEvent(moveEvent);

        // Always start capturing events when the mouse goes down in our HWND.
        SetCapture(m_windowHandle);

        if (insideThreshold && mouseEvent.button() == globalPrevButton)
            globalClickCount++;
        else
            // Reset the click count.
            globalClickCount = 1;
        globalPrevMouseDownTime = messageTime;
        globalPrevButton = mouseEvent.button();
        globalPrevPoint = mouseEvent.pos();

        mouseEvent.setClickCount(globalClickCount);
        handled = m_page->mainFrame()->eventHandler()->handleMousePressEvent(mouseEvent);
    }
    else if (message == WM_LBUTTONDBLCLK || message == WM_MBUTTONDBLCLK || message == WM_RBUTTONDBLCLK)
    {
        globalClickCount++;
        mouseEvent.setClickCount(globalClickCount);
        handled = m_page->mainFrame()->eventHandler()->handleMousePressEvent(mouseEvent);
    }
    else if (message == WM_LBUTTONUP || message == WM_MBUTTONUP || message == WM_RBUTTONUP)
    {
#if ENABLE(DIVIDE_PAGES)
		// added by achellies
		if (m_isLButtonDown && g_iReaderData && !g_iReaderData->m_bPostil && (message == WM_LBUTTONUP) && frame() && view()) {
			WTF::String selectedText = frame()->selectedText();
			if (!selectedText.isEmpty()) {
#if ENABLE(WINCEONWINDOWS)
				MessageBox(m_windowHandle, selectedText.charactersWithNullTermination(), TEXT("选择文字"), MB_OK);
#else
				if (m_data && m_data->pageAvailable() && g_iReaderData) {
					g_iReaderData->m_hasSelectionText = true;
					if (!g_stringManager)
						g_stringManager.Initialize(m_data->maximumText());

					if (g_stringManager)
						g_stringManager.FillBuffer(selectedText.charactersWithNullTermination(), selectedText.length());
					
					if (g_iReaderData->m_bTranslate) {
						RECT rcClient = {0};
						GetClientRect(hWnd, &rcClient);
						g_iReaderData->m_bSelectionTextPos = (mouseEvent.pos().y() <= (rcClient.top + (rcClient.bottom - rcClient.top)/2))?true:false;
						PostMessage(g_iReaderData->m_hNofityWnd, g_uInterProcMsg, IREADER_TRANSLATE, (LPARAM)0);
					}
				}
#endif
			}
			else if (g_iReaderData)
				g_iReaderData->m_hasSelectionText = false;
		}	

		if (m_isLButtonDown && g_iReaderData && g_iReaderData->m_bPostil && (message == WM_LBUTTONUP)) {
			WebCore::IntPoint clickedPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			m_postilMaker.TraceEnd(clickedPoint, g_iReaderData->m_nPostilPenWidth, g_iReaderData->m_bErasePostil);

			m_isLButtonDown = false;
			ReleaseCapture();
			return true;
		}

		m_isLButtonDown = false;
#endif
        // Record the global position and the button of the up.
        globalPrevButton = mouseEvent.button();
        globalPrevPoint = mouseEvent.pos();
        mouseEvent.setClickCount(globalClickCount);

        m_page->mainFrame()->eventHandler()->handleMouseReleaseEvent(mouseEvent);
        ReleaseCapture();
    }
    else if (message == WM_MOUSEMOVE)
    {
#if ENABLE(DIVIDE_PAGES)
		if (m_isLButtonDown && g_iReaderData && g_iReaderData->m_bPostil) {
			WebCore::IntPoint clickedPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			m_postilMaker.AddPoint(clickedPoint, g_iReaderData->m_nPostilPenWidth, g_iReaderData->m_bErasePostil);
			return true;
		}
#endif
        if (!insideThreshold)
            globalClickCount = 0;
        mouseEvent.setClickCount(globalClickCount);
        handled = m_page->mainFrame()->eventHandler()->mouseMoved(mouseEvent);
    }

    return handled;
}

bool WebView::handleMouseWheel(HWND hWnd, WPARAM wParam, LPARAM lParam, bool isHorizontal)
{
#if ENABLE(DIVIDE_PAGES)
	if (m_enableReaderMode && !(g_iReaderData && (g_iReaderData->m_bPostil || g_iReaderData->m_bExcerpt || g_iReaderData->m_bTranslate)))
	{
		return true;
	}

#if ENABLE(WINCEONWINDOWS)
	// Ctrl+Mouse wheel doesn't ever go into WebCore.  It is used to
	// zoom instead (Mac zooms the whole Desktop, but Windows browsers trigger their
	// own local zoom modes for Ctrl+wheel).
	if (m_data && (wParam & MK_CONTROL))
	{
		short delta = short(HIWORD(wParam));
		if (delta < 0)
			makeTextLarger();
		else
			makeTextSmaller();

		int nOffset = m_data->curPageOffset();

		m_data->reset();
		dividePages(m_windowHandle);

		if (m_data && m_data->pageAvailable())
			navigatePage(m_data->getPageByOffset(nOffset));

		return true;
	}
#endif
#endif

    PlatformWheelEvent wheelEvent(hWnd, wParam, lParam, isHorizontal);
    return frame()->eventHandler()->handleWheelEvent(wheelEvent);
}

bool WebView::handleKeyDown(WPARAM virtualKeyCode, LPARAM keyData, bool systemKeyDown)
{
#if ENABLE(DIVIDE_PAGES)
	if (m_enableReaderMode && !(g_iReaderData && (g_iReaderData->m_bPostil || g_iReaderData->m_bExcerpt || g_iReaderData->m_bTranslate)))
	{
		return true;
	}

#if ENABLE(WINCEONWINDOWS)
	// added by achellies
	//_tprintf(_T("WebView::handleKeyDown(), virtualKeyCode = 0x%2x.\n"), virtualKeyCode);
	if (virtualKeyCode == VK_RETURN) {

		if (m_data && !m_data->pageAvailable()) {
			m_data->reset();
			dividePages(m_windowHandle);

			firstPage();

			return true;
		}
	}
	else if ((virtualKeyCode == VK_PRIOR) || (virtualKeyCode == VK_DOWN) || (virtualKeyCode == VK_UP) || (virtualKeyCode == VK_NEXT)){

		if (m_data && m_data->pageAvailable()) {
			int nCurPage = curPage();
			switch (virtualKeyCode) {
				case VK_PRIOR:
				case VK_UP:
					prevPage();
					break;
				case VK_DOWN:
				case VK_NEXT:
					nextPage();
				default:
					break;
			}
			return true;
		}

		return true;
	}
#endif
#endif
    Frame* frame = m_page->focusController()->focusedOrMainFrame();
    if (virtualKeyCode == VK_CAPITAL)
        frame->eventHandler()->capsLockStateMayHaveChanged();

    PlatformKeyboardEvent keyEvent(m_windowHandle, virtualKeyCode, keyData, PlatformKeyboardEvent::RawKeyDown, systemKeyDown);
    bool handled = frame->eventHandler()->keyEvent(keyEvent);

    // These events cannot be canceled, and we have no default handling for them.
    // FIXME: match IE list more closely, see <http://msdn2.microsoft.com/en-us/library/ms536938.aspx>.
    if (systemKeyDown && virtualKeyCode != VK_RETURN)
        return false;

    if (handled) {
        // FIXME: remove WM_UNICHAR, too
        MSG msg;
        // WM_SYSCHAR events should not be removed, because access keys are implemented in WebCore in WM_SYSCHAR handler.
        if (!systemKeyDown)
            ::PeekMessage(&msg, m_windowHandle, WM_CHAR, WM_CHAR, PM_REMOVE);
        return true;
    }

    return handled;
}

bool WebView::handleKeyPress(WPARAM charCode, LPARAM keyData, bool systemKeyDown)
{
    Frame* frame = m_page->focusController()->focusedOrMainFrame();

    PlatformKeyboardEvent keyEvent(m_windowHandle, charCode, keyData, PlatformKeyboardEvent::Char, systemKeyDown);
    // IE does not dispatch keypress event for WM_SYSCHAR.
    if (systemKeyDown)
        return frame->eventHandler()->handleAccessKey(keyEvent);
    if (frame->eventHandler()->keyEvent(keyEvent))
        return true;

    return false;
}

bool WebView::handleKeyUp(WPARAM virtualKeyCode, LPARAM keyData, bool systemKeyDown)
{
    PlatformKeyboardEvent keyEvent(m_windowHandle, virtualKeyCode, keyData, PlatformKeyboardEvent::KeyUp, systemKeyDown);

    Frame* frame = m_page->focusController()->focusedOrMainFrame();
    return frame->eventHandler()->keyEvent(keyEvent);
}

bool WebView::handleSizeEvent(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	if (m_doubleBufferBitmap.get())
	{
		m_doubleBufferBitmap.clear();
	}

	if (m_doubleBufferDC.get())
	{
		m_doubleBufferDC.clear();
	}

#if ENABLE(DIVIDE_PAGES)
	if (m_data)
	{
		m_data->reset();
	}
#endif

	RECT windowRect;
	frameRect(&windowRect);

	IntSize contentsSize(windowRect.right - windowRect.left, windowRect.bottom - windowRect.top);
	view()->resize(contentsSize);
	view()->setContentsSize(contentsSize);
	view()->forceLayout();
	return true;
}

LRESULT WebView::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    bool handled = false;

    if (view())
    {
        switch (message)
        {
        case WM_PAINT:
            handled = handlePaint(hWnd);
            break;

        case WM_MOUSEMOVE:
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        case WM_MBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:
            if (frame()->eventHandler() && view()->didFirstLayout())
                handled = handleMouseEvent(hWnd, message, wParam, lParam);
            break;

        case WM_MOUSEWHEEL:
            if (frame()->eventHandler() && view()->didFirstLayout())
                handled = handleMouseWheel(hWnd, wParam, lParam, wParam & MK_SHIFT);
            break;

        case WM_SYSKEYDOWN:
            handled = handleKeyDown(wParam, lParam, true);
            break;

        case WM_KEYDOWN:
#if ENABLE(DIVIDE_PAGES)
			if (m_enableReaderMode && g_iReaderData){
				PostMessage(g_iReaderData->m_hNofityWnd, g_uInterProcMsg, IREADER_KEYEVENT, wParam);
			}else 
#endif
            handled = handleKeyDown(wParam, lParam, false);
            break;

        case WM_SYSKEYUP:
            handled = handleKeyUp(wParam, lParam, true);
            break;

        case WM_KEYUP:
#if ENABLE(DIVIDE_PAGES)
			if (m_enableReaderMode){
				PostMessage(g_iReaderData->m_hNofityWnd, g_uInterProcMsg, IREADER_KEYEVENT, wParam);
			}else 
#endif
            handled = handleKeyUp(wParam, lParam, false);
            break;

        case WM_SYSCHAR:
            handled = handleKeyPress(wParam, lParam, true);
            break;

        case WM_CHAR:
            handled = handleKeyPress(wParam, lParam, false);
            break;

        case WM_CLOSE:
            PostMessage(m_parentWindowHandle, WM_CLOSE, wParam, lParam);
            handled = true;
            break;

		case WM_SIZE:
			handled = handleSizeEvent(wParam, lParam);
			break;

        default:
            break;
        }
    }

    if (handled)
        return 0;

    return DefWindowProc(hWnd, message, wParam, lParam);
}