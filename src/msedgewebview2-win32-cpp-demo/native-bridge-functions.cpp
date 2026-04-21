// file:    native-bridge-functions.cpp - msedgewebview2 native-bridge functions (ie, add_WebMessageReceived()) demo
// compile: [in x64, *NOT* x86, native tools] cl.exe native-bridge-functions.cpp /EHsc /DUNICODE /D_UNICODE /I ".\wv2-sdk\include\" /I ".\ms-wil-include\" ".\wv2-sdk\WebView2Loader.dll.lib" /link user32.lib gdi32.lib ole32.lib oleaut32.lib
// author:  Ben Mullan 2026

/*
    them from client-side javascript:
        (right-click -> inspect -> console)
        window.chrome.webview.postMessage("dialer");
        window.chrome.webview.postMessage("msgbox:some text here!");
*/

#include <windows.h>
#include <wrl.h>
#include <string>
#include "./ms-wil-include/com.h"
#include "./wv2-sdk/include/WebView2.h"

HWND g_hWnd = nullptr;
wil::com_ptr<ICoreWebView2Controller> g_controller;
wil::com_ptr<ICoreWebView2> g_webview;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {
        case WM_SIZE: {
            if (g_controller) {
                RECT bounds; GetClientRect(hWnd, &bounds);
                g_controller->put_Bounds(bounds);
            }
            break;
        } case WM_DESTROY: {
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
        wc.lpszClassName, L"WebView2 Security Demo",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768,
        nullptr, nullptr, hInst, nullptr
    );

    CreateCoreWebView2EnvironmentWithOptions(
        nullptr, nullptr, nullptr,
        Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
            [](HRESULT hr, ICoreWebView2Environment* env) -> HRESULT {

                env->CreateCoreWebView2Controller(
                    g_hWnd,
                    Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                        [](HRESULT hr, ICoreWebView2Controller* controller) -> HRESULT {

                            if (controller) {

                                g_controller = controller;
                                g_controller->get_CoreWebView2(&g_webview);

                                RECT bounds;
                                GetClientRect(g_hWnd, &bounds);
                                g_controller->put_Bounds(bounds);

                                // =================================
                                // NATIVE BRIDGE: WebMessageReceived
                                // =================================

                                EventRegistrationToken token;

                                g_webview->add_WebMessageReceived(
                                    Microsoft::WRL::Callback<ICoreWebView2WebMessageReceivedEventHandler>(
                                        [](ICoreWebView2* sender, ICoreWebView2WebMessageReceivedEventArgs* args) -> HRESULT {
                                            
                                            wil::unique_cotaskmem_string message;
                                            args->TryGetWebMessageAsString(&message);
                                            
                                            if (message) {

                                                // TryGetWebMessageAsString() returns JSON-encoded strings,
                                                // so a JS string "test" arrives in C++ as "\"test\"".
                                                std::wstring msg(message.get());

                                                // function 1: launch dialer.exe (RCE Proof)
                                                if (msg.find(L"dialer") != std::wstring::npos) {
                                                    WinExec("dialer.exe", SW_SHOW);
                                                    sender->ExecuteScript(L"console.log('Native Bridge: Executed dialer.exe');", nullptr);
                                                }

                                                // function 2: MessageBoxW() with Payload
                                                else if (msg.find(L"msgbox:") != std::wstring::npos) {
                                                    // Extract payload (ignoring the JSON quotes and 'msgbox:' prefix)
                                                    size_t start = msg.find(L"msgbox:") + 7;
                                                    size_t end = msg.find_last_of(L"\"");
                                                    if (start < end) {
                                                        std::wstring payload = msg.substr(start, end - start);
                                                        MessageBoxW(g_hWnd, payload.c_str(), L"SYSTEM ALERT", MB_OK | MB_ICONWARNING);
                                                    }
                                                }

                                            }

                                            return S_OK;

                                        }
                                    ).Get(),
                                    &token
                                );

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