// file:    single-webpage.cpp - minimal msedgewebview2 demo
// compile: [in x64, *NOT* x86, native tools] cl.exe single-webpage.cpp /EHsc /DUNICODE /D_UNICODE /I ".\wv2-sdk\include\" /I ".\ms-wil-include\" ".\wv2-sdk\WebView2Loader.dll.lib" /link user32.lib gdi32.lib ole32.lib oleaut32.lib
// author:  Ben Mullan 2026

#include <windows.h>
#include <wrl.h>
#include "./ms-wil-include/com.h"
#include "./wv2-sdk/include/WebView2.h"

using namespace Microsoft::WRL;

HWND g_hWnd = nullptr;
wil::com_ptr<ICoreWebView2Controller> g_controller;
wil::com_ptr<ICoreWebView2> g_webview;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {
        case WM_SIZE: {
            if (g_controller) {
                RECT bounds;
                GetClientRect(hWnd, &bounds);
                g_controller->put_Bounds(bounds);
            }
            break;
        }
        case WM_DESTROY: {
            PostQuitMessage(0);
            break;
        }
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);

}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int) {

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = L"WebView2Demo";
    RegisterClass(&wc);

    g_hWnd = CreateWindow(
        wc.lpszClassName, L"WebView2 Demo",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768,
        nullptr, nullptr, hInst, nullptr
    );

    CreateCoreWebView2EnvironmentWithOptions(
        nullptr, nullptr, nullptr,
        Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
            [](HRESULT hr, ICoreWebView2Environment* env) -> HRESULT {
                env->CreateCoreWebView2Controller(
                    g_hWnd,
                    Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                        [](HRESULT hr, ICoreWebView2Controller* controller) -> HRESULT {
                            if (controller) {

                                g_controller = controller;
                                g_controller->get_CoreWebView2(&g_webview);

                                RECT bounds;
                                GetClientRect(g_hWnd, &bounds);
                                g_controller->put_Bounds(bounds);

                                g_webview->Navigate(L"https://benmullan.github.io/");

                            }
                            return S_OK;
                        }
                    ).Get()
                );
                return S_OK;
            }
        ).Get()
    );

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;

}