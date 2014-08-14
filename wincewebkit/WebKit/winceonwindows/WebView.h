#ifndef WebView_h
#define WebView_h

#include "IntRect.h"
#include "OwnPtr.h"
#include "PassRefPtr.h"

#if ENABLE(DIVIDE_PAGES)

// added by achellies, include RenderText class's header file
#include "rendertext.h"

// added by achellies to include the class WebViewPrivate declaration
#include "frameview.h"

#include "iReader\iReader.h"
#include "iReader\PostilMaker.h"

class WebViewPrivate;

#endif

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

// added by achellies
class RenderObject;
}

#if ENABLE(DIVIDE_PAGES)

class CEvent
{
public:
	CEvent(LPCTSTR lpszName, BOOL bManualReset = TRUE, BOOL bInitialState = TRUE);
	~CEvent();
	void reset();
	void set();
	bool isSigned();
	bool wait(DWORD dwTimeOut = 30*1000);
private:
	HANDLE	m_hEvent;
};

// added by achellies
extern CMemroySharePtr<iReaderData,1>	g_iReaderData;
extern CStrSharingManager				g_stringManager;

extern UINT	g_uInterProcMsg;

#endif

class WebView
{
public:
    WebView(HWND hwnd, bool enableGradients = true, bool enableDoubleBuffer = true, bool enableReaderMode = false);
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

#if ENABLE(DIVIDE_PAGES)

	// added by achellies

	WTF::String getPageText(int nIndex);

	int dividePages(HWND hWnd);

	WTF::String selectedText();

	void clearCache();

	// navigate pages related routines
	int pageCount();
	int curPage();
	bool firstPage();
	bool lastPage();
	bool nextPage();
	bool prevPage();
	int getCurrOffset();
	int getPageOffset(int nPage);
	int getPageNumberByOffset(int nOffset);

	bool getPageTTSText(int nPage);
	bool getPageBeginText(int nPage);

	void clearTextSelection();

	// zoom related routines
	bool setTextSizeMultiplier(float multiplier);
	bool setPageSizeMultiplier(float multiplier);
	void setZoomMultiplier(float multiplier, bool isTextOnly);
	void textSizeMultiplier(float* multiplier);
	void pageSizeMultiplier(float* multiplier);
	float zoomMultiplier(bool isTextOnly);
	bool canMakeTextLarger();
	bool canZoomPageIn();
	bool canZoomIn(bool isTextOnly);
	bool makeTextLarger();
	bool zoomPageIn();
	bool zoomIn(bool isTextOnly);
	bool canMakeTextSmaller();
	bool canZoomPageOut();
	bool canZoomOut(bool isTextOnly);
	bool makeTextSmaller();
	bool zoomPageOut();
	bool zoomOut(bool isTextOnly);
	bool canMakeTextStandardSize();
	bool canResetPageZoom();
	bool canResetZoom(bool isTextOnly);
	bool makeTextStandardSize();
	bool resetPageZoom();
	bool resetZoom(bool isTextOnly);

	// check whether the Webkit has already layout the file
	bool didFirstLayout() const { return view()?view()->didFirstLayout():false; }

	// set the first layout is done
	void didLayout();

	// added by achellies, navigate the given page
	bool navigatePage(int nIndex);

	// change the font size
	void fontSizeZoom();

	// find the string
	bool findString(const String& target, TextCaseSensitivity, bool shouldHighlight, unsigned limit, unsigned& matches, Vector<unsigned>& matchedPages);
#endif

private:
    static LRESULT CALLBACK WebViewWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT WndProc(HWND, UINT, WPARAM, LPARAM);

    bool handlePaint(HWND hWnd);
    bool handleMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    bool handleMouseWheel(HWND hWnd, WPARAM wParam, LPARAM lParam, bool isHorizontal);
    bool handleKeyDown(WPARAM virtualKeyCode, LPARAM keyData, bool systemKeyDown);
    bool handleKeyPress(WPARAM charCode, LPARAM keyData, bool systemKeyDown);
    bool handleKeyUp(WPARAM virtualKeyCode, LPARAM keyData, bool systemKeyDown);

	// added by achellies
	bool handleSizeEvent(WPARAM wParam, LPARAM lParam);

#if ENABLE(DIVIDE_PAGES)

	// added by achellies, select the text in the whole page
	WTF::String getRecrusionNodeText(int nIndex, WebCore::Node* node);
	WTF::String getRecursionRenderObjectText(int nIndex, WebCore::RenderText* root);

	// added by achellies, seperate the html file to several pages
	void walkThroughRecrusionNode(Vector<WebCore::IntRect>& leafNodesRect, WebCore::Node* node);
	void getLeafNodeRect(Vector<WebCore::IntRect>& leafNodesRect, WebCore::Node* node);

	// added by achelies, this function is mainly to get the screen text by given Point
	WTF::String selectNode(const WebCore::IntPoint& clickedPoint, bool useRelativePostion = true);
	WebCore::Node* walkThroughRecrusionNode(const WebCore::IntPoint& absolutePoint, WebCore::Node* node);
	WebCore::Node* selectLeafNode(const WebCore::IntPoint& absolutePoint, WebCore::Node* node);

#endif

    void paint(HDC hDC, const WebCore::IntRect& clipRect);

    WebCore::Frame* m_frame;
    WebCore::Page* m_page;
    HWND m_parentWindowHandle;
    HWND m_windowHandle;
    bool m_enableDoubleBuffer;
    OwnPtr<HDC> m_doubleBufferDC;
    OwnPtr<HBITMAP> m_doubleBufferBitmap;
    WebCore::IntRect m_invalidatedRect;

#if ENABLE(DIVIDE_PAGES)
	bool m_enableReaderMode;	
    bool m_zoomMultiplierIsTextOnly;
	bool m_isLButtonDown;
	float m_zoomMultiplier;

	// this parameter is used to indicate whether the layout is finished or not
	WebViewPrivate* m_data;
	
public:	
	CPostilMaker m_postilMaker;
	CEvent	m_layoutEvent;
	CEvent	m_syncData;
#endif
};

#endif // WebView_h
