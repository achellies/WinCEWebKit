/*
* Copyright (C) 2010 Patrick Gansterer <paroga@paroga.com>
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
#include "WebView.h"
#include "resource.h"

#if ENABLE(WINCEONWINDOWS)
#include "IntRect.h"
#endif

static const LPCWSTR kMainWindowTitle = L"WebKit for WinCE";
static const LPCWSTR kMainWindowClassName = L"MainWindowClass";

using namespace WebCore;

WebView* g_webView = NULL;

#if ENABLE(DIVIDE_PAGES)

UINT	g_uInterProcMsg = 0;
CMemroySharePtr<iReaderData,1>	g_iReaderData(IREADER_DATA);

CStrSharingManager				g_stringManager;

#endif


static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
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

	// disabled by achellies
	//LPCWSTR homeUrl = lpCmdLine;

	bool enableDoubleBuffer = true;
	bool enableGradients = true;
	bool fullscreen = false;

#if 1
	LPCWSTR homeUrl = L"file:///\\Program Files\\WinCELauncher\\chapter02\\test.html";
	//LPCWSTR homeUrl = L"file:///\\storage card\\webkit\\chapter01\\chapter01.html";
#else
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
	wc.lpszClassName  = kMainWindowClassName;
	RegisterClass(&wc);

#if 0
	HWND hMainWindow = CreateWindowW(kMainWindowClassName, kMainWindowTitle, styles,
		0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, NULL, hInstance, NULL);
#else
	HWND hMainWindow = CreateWindowW(kMainWindowClassName, kMainWindowTitle, styles,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
#endif

	if (fullscreen)
		SetWindowPos(hMainWindow, HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_SHOWWINDOW);

	ShowWindow(hMainWindow, nCmdShow);
	UpdateWindow(hMainWindow);

	g_webView = new WebView(hMainWindow, enableGradients, enableDoubleBuffer);
	g_webView->load(homeUrl);

	// Main message loop:
	MSG msg;
	BOOL bRet;
	while (bRet = GetMessage(&msg, NULL, 0, 0)) {
		if (bRet == -1) {
			// handle the error and possibly exit
		} else {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	delete g_webView;
	g_webView = NULL;

	DestroyWindow(hMainWindow);

	WebView::cleanup();
	CoUninitialize();

	return msg.wParam;
}
