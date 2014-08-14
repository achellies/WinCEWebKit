/*
* Copyright (C) 2010, 2011 achellies <achellies@163.com>
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

#include "config.h"
#include <windows.h>
#include <tchar.h>
#include "iReader.h"
#include "WebView.h"
#include "resource.h"

#if ENABLE(WINCEONWINDOWS)
#include "IntRect.h"
#endif

static const LPCWSTR lpszIReaderTitle = L"iReader Based Webkit";

using namespace WebCore;

WebView* g_webView = NULL;

#if ENABLE(DIVIDE_PAGES)

UINT	g_uInterProcMsg = 0;
CMemroySharePtr<iReaderData,1>	g_iReaderData(IREADER_DATA);

CStrSharingManager				g_stringManager;

#endif

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	bool handled = false;
#if ENABLE(DIVIDE_PAGES)
	if (g_webView && g_iReaderData && (g_uInterProcMsg > 0) && (g_uInterProcMsg == message)) {
		//_tprintf(_T("WndProc, message = %d, wParam = %d, lParam = %d.\n"), message, wParam, lParam);
		handled = true;
		switch (wParam)
		{
		case IREADER_QUIT:
			PostQuitMessage(0);
			break;
		case IREADER_OPEN:
			if (_tcslen(g_iReaderData->m_szFilePath) > 0) {
				g_webView->load(g_iReaderData->m_szFilePath);
			}
			break;
		case IREADER_CLEAR_CACHE:
			g_webView->clearCache();
			break;
		case IREADER_CLEAR_POSTIL:
			g_webView->m_postilMaker.ClearPageData(lParam);
			break;
		case IREADER_INVALIDATE:
			InvalidateRect(g_webView->windowHandle(), NULL, TRUE);
			if (lParam)
				UpdateWindow(g_webView->windowHandle());
			break;
		case IREADER_DIVIDEPAGES:
			if (g_webView->didFirstLayout()) {
				g_webView->dividePages(g_webView->windowHandle());
				g_webView->m_syncData.set();
			}
			else
				g_webView->m_syncData.set();
			break;
		case IREADER_NAVIGATE:
			if (g_webView->didFirstLayout() && (lParam < g_webView->pageCount())) {
				handled = g_webView->navigatePage(lParam);
			}
			break;
		case IREADER_ZOOM:
			if (g_webView->didFirstLayout()) {
				g_webView->fontSizeZoom();
			}
			break;
		case IREADER_PAGENUMBYOFFSET:
			if (g_webView->didFirstLayout() && g_iReaderData) {
				g_iReaderData->m_nPageNumByOffset = g_webView->getPageNumberByOffset(static_cast<int>(lParam));
				g_webView->m_syncData.set();
			}
			else
				g_webView->m_syncData.set();
			break;
		case IREADER_PAGEOFFSET:
			if (g_webView->didFirstLayout() && g_iReaderData) {
				g_iReaderData->m_nCurOffset = g_webView->getPageOffset(static_cast<int>(lParam));
				g_webView->m_syncData.set();
			}
			else
				g_webView->m_syncData.set();
			break;
		case IREADER_CUROFFSET:
			if (g_webView->didFirstLayout() && g_iReaderData) {
				g_iReaderData->m_nCurOffset = g_webView->getCurrOffset();
				g_webView->m_syncData.set();
			}
			else
				g_webView->m_syncData.set();
			break;
		case IREADER_GET_BEGIN_TEXT:
			if (g_webView->didFirstLayout() && g_iReaderData) {
				g_webView->getPageBeginText(static_cast<int>(lParam));
				g_webView->m_syncData.set();
			}
			break;
		case IREADER_TTS:
			if (g_webView->didFirstLayout() && g_iReaderData) {
				g_webView->getPageTTSText(static_cast<int>(lParam));
				g_webView->m_syncData.set();
			}
			break;
		case IREADER_CLEARSELECTION:
			if (g_webView->didFirstLayout() && g_iReaderData) {
				g_webView->clearTextSelection();
				InvalidateRect(g_webView->windowHandle(), NULL, TRUE);
			}
			break;
		case IREADER_SEARCH:
			{
				static Vector<unsigned> matchedPages;
				switch (lParam)
				{
				// start a new searching task
				case 0:
					if (g_webView->didFirstLayout()
						&& g_iReaderData
						&& (_tcslen(g_iReaderData->m_szSearchText) > 0)) {
							g_iReaderData->m_bUnderSearch = false;
							g_iReaderData->m_nNextMatches = 0;
							g_iReaderData->m_nPrevMatches = 0;
							unsigned matches = 0;
							matchedPages.clear();
							if (g_webView->findString(g_iReaderData->m_szSearchText, TextCaseSensitive, true, 0, matches, matchedPages) && (matches > 0))
							{
								unsigned currentPage = static_cast<unsigned>(g_webView->curPage());
								if (static_cast<size_t>(-1) == matchedPages.find(currentPage)) {
									for (Vector<unsigned>::const_iterator citer = matchedPages.begin(); citer != matchedPages.end(); ++citer) {
										if (*citer > currentPage) {
											g_webView->navigatePage(*citer);
											break;
										}
									}
								}
								currentPage = static_cast<unsigned>(g_webView->curPage());

								for (Vector<unsigned>::const_iterator citer = matchedPages.begin(); citer != matchedPages.end(); ++citer) {
									if (*citer < currentPage)
										++g_iReaderData->m_nPrevMatches;
									else if (*citer > currentPage)
										++g_iReaderData->m_nNextMatches;
								}
								g_iReaderData->m_bUnderSearch = true;
							}							
					}
					g_webView->m_syncData.set();
					break;
				// get the previous matched pages
				case 1:
					if (g_webView->didFirstLayout()
						&& g_iReaderData
						&& (_tcslen(g_iReaderData->m_szSearchText) > 0)
						&& g_iReaderData->m_bUnderSearch
						&& (g_iReaderData->m_nPrevMatches >= 1)
						&& (g_iReaderData->m_nPrevMatches <= static_cast<int>(matchedPages.size()))
						&& !matchedPages.isEmpty()) {
							g_iReaderData->m_nNextMatches = 0;
							g_iReaderData->m_nPrevMatches = 0;
							unsigned nCurrentMatchedPage = static_cast<unsigned>(g_webView->curPage());
							unsigned nPrevMatchedPage = g_webView->pageCount();

							for (unsigned index = matchedPages.size() - 1; index >= 0; --index) {
								if (matchedPages.at(index) < nCurrentMatchedPage) {
									nPrevMatchedPage = matchedPages.at(index);
									break;
								}
							}

							if ((nPrevMatchedPage >= 0)
								&& (nPrevMatchedPage < g_webView->pageCount()))
								g_webView->navigatePage(nPrevMatchedPage);

							unsigned currentPage = static_cast<unsigned>(g_webView->curPage());
							for (Vector<unsigned>::const_iterator citer = matchedPages.begin(); citer != matchedPages.end(); ++citer) {
								if (*citer < currentPage)
									++g_iReaderData->m_nPrevMatches;
								else if (*citer > currentPage)
									++g_iReaderData->m_nNextMatches;
							}
					}
					g_webView->m_syncData.set();
					break;
				// get the next matched pages
				case 2:
					if (g_webView->didFirstLayout()
						&& g_iReaderData
						&& (_tcslen(g_iReaderData->m_szSearchText) > 0)
						&& g_iReaderData->m_bUnderSearch
						&& (g_iReaderData->m_nNextMatches > 0)
						&& (g_iReaderData->m_nNextMatches <= static_cast<int>(matchedPages.size()))
						&& !matchedPages.isEmpty()) {
							g_iReaderData->m_nNextMatches = 0;
							g_iReaderData->m_nPrevMatches = 0;
							int nCurrentMatchedPage = static_cast<unsigned>(g_webView->curPage());
							int nNextMatchedPage = g_webView->pageCount();

							for (unsigned index = 0; index < matchedPages.size(); ++index) {
								if (matchedPages.at(index) > nCurrentMatchedPage) {
									nNextMatchedPage = matchedPages.at(index);
									break;
								}
							}

							if ((nNextMatchedPage >= 0)
								&& (nNextMatchedPage < g_webView->pageCount()))
								g_webView->navigatePage(nNextMatchedPage);

							unsigned currentPage = static_cast<unsigned>(g_webView->curPage());
							for (Vector<unsigned>::const_iterator citer = matchedPages.begin(); citer != matchedPages.end(); ++citer) {
								if (*citer < currentPage)
									++g_iReaderData->m_nPrevMatches;
								else if (*citer > currentPage)
									++g_iReaderData->m_nNextMatches;
							}
					}
					g_webView->m_syncData.set();
					break;
				// get the first matched page
				case 3:
					if (g_webView->didFirstLayout()
						&& g_iReaderData
						&& (_tcslen(g_iReaderData->m_szSearchText) > 0)
						&& g_iReaderData->m_bUnderSearch
						&& !matchedPages.isEmpty()) {
							g_iReaderData->m_nNextMatches = 0;
							g_iReaderData->m_nPrevMatches = 0;

							int nFirstMatchedPage = matchedPages.at(0);

							if ((nFirstMatchedPage >= 0)
								&& (nFirstMatchedPage < g_webView->pageCount()))
								g_webView->navigatePage(nFirstMatchedPage);

							unsigned currentPage = static_cast<unsigned>(g_webView->curPage());
							for (Vector<unsigned>::const_iterator citer = matchedPages.begin(); citer != matchedPages.end(); ++citer) {
								if (*citer < currentPage)
									++g_iReaderData->m_nPrevMatches;
								else if (*citer > currentPage)
									++g_iReaderData->m_nNextMatches;
							}
					}
					g_webView->m_syncData.set();
					break;
				// get the last matched page
				case 4:
					if (g_webView->didFirstLayout()
						&& g_iReaderData
						&& (_tcslen(g_iReaderData->m_szSearchText) > 0)
						&& g_iReaderData->m_bUnderSearch
						&& !matchedPages.isEmpty()) {
							g_iReaderData->m_nNextMatches = 0;
							g_iReaderData->m_nPrevMatches = 0;

							int nLastMatchedPage = matchedPages.at(matchedPages.size() - 1);

							if ((nLastMatchedPage >= 0)
								&& (nLastMatchedPage < g_webView->pageCount()))
								g_webView->navigatePage(nLastMatchedPage);

							unsigned currentPage = static_cast<unsigned>(g_webView->curPage());
							for (Vector<unsigned>::const_iterator citer = matchedPages.begin(); citer != matchedPages.end(); ++citer) {
								if (*citer < currentPage)
									++g_iReaderData->m_nPrevMatches;
								else if (*citer > currentPage)
									++g_iReaderData->m_nNextMatches;
							}
					}
					g_webView->m_syncData.set();
					break;
				}
				break;
			}
		}
	}
#endif

    if (handled)
        return 0;

	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_SIZE:
		if (g_webView)
		{
			RECT rcClient;
			GetClientRect(hWnd, &rcClient);
			IntRect windowClient(rcClient);

			MoveWindow(g_webView->windowHandle(), 0, 0, windowClient.width(), windowClient.height(), TRUE);
		}
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

#if ENABLE(WINCEONWINDOWS)
int main (int argc, char *argv[])
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	int nCmdShow = SW_SHOW;
#else
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
#endif

	CoInitializeEx(0, COINIT_MULTITHREADED);
	WebView::initialize(hInstance);

	bool enableDoubleBuffer = true;
	bool enableGradients = true;
	bool fullscreen = false;
	bool enableReaderMode = true;

#if ENABLE(DIVIDE_PAGES)

	g_uInterProcMsg = RegisterWindowMessage(IREADER_INTERPROC_MSG);

#endif

#if ENABLE(WINCEONWINDOWS)
	//LPCWSTR homeUrl = L"file:///\\Program Files\\WinCELauncher\\chapter02\\test.html";
	//LPCWSTR homeUrl = L"file:///\\storage card\\webkit\\chapter01\\chapter01.html";
	LPCWSTR homeUrl = L"file:///E:/Webkit/354774475/chapter01.html";
#else
	LPCWSTR homeUrl = lpCmdLine;
	if (homeUrl[0] == '-') {
		for (; *homeUrl && *homeUrl != ' '; ++homeUrl) {
			switch (*homeUrl) {
			case 'd':
				enableDoubleBuffer = false;
				break;
			case 'f':
				fullscreen = true;
				break;
			case 'g':
				enableGradients = false;
				break;
			case 'r':
				enableReaderMode = true;
				break;
			case 'w':
				enableReaderMode = false;
				break;
			default:
				break;
			}
		}
		if (*homeUrl)
			++homeUrl;
	}
#endif

	DWORD styles = WS_VISIBLE;

	if (!fullscreen)
		styles |=
		WS_CAPTION       |
		WS_MAXIMIZEBOX   |
		WS_MINIMIZEBOX   |
		WS_OVERLAPPED    |
		WS_SYSMENU       |
		WS_THICKFRAME;

	WNDCLASSW wc;
	wc.style          = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc    = WndProc;
	wc.cbClsExtra     = 0;
	wc.cbWndExtra     = 0;
	wc.hInstance      = hInstance;
	wc.hIcon          = 0;
	wc.hCursor        = 0;
	wc.hbrBackground  = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wc.lpszMenuName   = NULL;
	wc.lpszClassName  = lpszIReaderWindowClassName;
	RegisterClass(&wc);

#if ENABLE(WINCEONWINDOWS)
	HWND hMainWindow = CreateWindowW(lpszIReaderWindowClassName, lpszIReaderTitle, styles,
		/*CW_USEDEFAULT, 0, CW_USEDEFAULT, 0*/0, 0, 619, 680, NULL, NULL, hInstance, NULL);
#else
	HWND hMainWindow = CreateWindowEx(WS_EX_TOOLWINDOW, lpszIReaderWindowClassName, lpszIReaderTitle, WS_VISIBLE,
		0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, NULL, hInstance, NULL);
#endif

	if (fullscreen)
		SetWindowPos(hMainWindow, HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_SHOWWINDOW);

	ShowWindow(hMainWindow, nCmdShow);
	UpdateWindow(hMainWindow);

	g_webView = new WebView(hMainWindow, enableGradients, enableDoubleBuffer, enableReaderMode);
	if (g_webView == NULL)
	{
		DestroyWindow(hMainWindow);
		CoUninitialize();

		return 0;
	}

	g_webView->load(homeUrl);

#if !ENABLE(WINCEONWINDOWS)
	RECT rcClient = {0};
	GetClientRect(hMainWindow, &rcClient);
	IntRect windowClient(rcClient);

	MoveWindow(g_webView->windowHandle(), 0, 0, windowClient.width(), windowClient.height(), TRUE);
#endif

	// Main message loop:
	MSG msg;
	BOOL bRet;

	try
	{
		while (bRet = GetMessage(&msg, NULL, 0, 0)) {
			if (bRet == -1) {
				// handle the error and possibly exit
			} else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
	catch(...)
	{
		g_webView->m_layoutEvent.set();
		g_webView->m_syncData.set();
		if (g_iReaderData)
			PostMessage(g_iReaderData->m_hNofityWnd, g_uInterProcMsg, IREADER_EXCEPTION_RAISED, (LPARAM)0);
	}

	delete g_webView;
	g_webView = NULL;

	DestroyWindow(hMainWindow);

	WebView::cleanup();
	CoUninitialize();

	return msg.wParam;
}
