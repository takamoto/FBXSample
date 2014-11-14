#pragma once
#include <memory>
#include <cstdio>
#include <Windows.h>
#include <tchar.h>

LRESULT CALLBACK DefaultWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam) {
	switch(msg) {
	case WM_KEYUP:
		if(wParam == VK_ESCAPE) ::DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	default:
		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0L;
}


HWND InitWindow(TCHAR* AppName, HINSTANCE hInstance, int width, int height, WNDPROC wndProc) {
	DWORD WindowStyle = WS_BORDER | WS_CAPTION | WS_SYSMENU;

	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = wndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(DWORD);
	wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
	wc.hIcon = nullptr;
	wc.hIconSm = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = AppName;
	wc.hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH);
	wc.hInstance = hInstance;
	::RegisterClassEx(&wc);

	RECT rc;
	::SetRect(&rc, 0, 0, width, height);
	::AdjustWindowRect(&rc, WindowStyle, FALSE);

	auto hWnd = ::CreateWindow(
		wc.lpszClassName,
		wc.lpszClassName,
		WindowStyle,
		0,
		0,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	if(hWnd == nullptr) throw;
	return hWnd;
}

struct WindowGuard {
	HINSTANCE hInstance;
	HWND hWnd;
	std::unique_ptr<TCHAR[]> appName;

	WindowGuard(
		TCHAR* appName, HINSTANCE hInstance, int width, int height, WNDPROC wndProc = DefaultWndProc
	){
		this->appName = std::unique_ptr<TCHAR[]>(new TCHAR[lstrlen(appName)+1]);
		lstrcpy(this->appName.get(), appName);
		this->hInstance = hInstance;
		this->hWnd = InitWindow(appName, hInstance, width, height, wndProc);
	}

	~WindowGuard() {
		::UnregisterClass(appName.get(), hInstance);
	}

	WindowGuard(const WindowGuard &) = delete;
	WindowGuard & operator=(const WindowGuard &) = delete;
};

