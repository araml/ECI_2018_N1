#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <iostream>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

LRESULT CALLBACK window_proc(HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam);

IDXGISwapChain *swapchain;             // the pointer to the swap chain interface
ID3D11Device *dev;                     // the pointer to our Direct3D device interface
ID3D11DeviceContext *devcon;           // the pointer to our Direct3D device context

class window {
public:
    window(int width, int height, bool fullscreen) {
        ZeroMemory(&wc, sizeof(WNDCLASSEX));

        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = window_proc;
        wc.hInstance = nullptr;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
        wc.lpszClassName = "ECI2018";

        RegisterClassEx(&wc);

        RECT client_size{0, 0, width, height };
        AdjustWindowRect(&client_size, WS_OVERLAPPEDWINDOW, false);

        m_window = CreateWindow("ECI2018", "ECI 2018", WS_OVERLAPPEDWINDOW,
                                2500, 300, 
                                client_size.right - client_size.left, 
                                client_size.bottom - client_size.top,
                                nullptr, nullptr, nullptr, nullptr);
        ShowWindow(m_window, SW_SHOW);
    }

    ~window() {
        CloseWindow(m_window);
    }

private:
    HWND m_window;
    WNDCLASSEX wc;
};

bool process_events() {
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT)
            return true;

        TranslateMessage(&msg);
        DispatchMessage(&msg); 
    }

    return false;
}

// the entry point for any Windows program
int main() {
    FreeConsole();
    
    window w(500, 400, false);

    // main loop
    while (true) {
        if (process_events())
            break;
    }

    // return 0 to Windows
    return 0;
}

// this is the main message handler for the program
LRESULT CALLBACK window_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    // sort through and find what code to run for the message given
    switch (message) {
        // this message is read when the window is closed
        case WM_DESTROY: {
            // close the application entirely
            PostQuitMessage(0);
            return 0;
        } break;
    }

    // Handle any messages the switch statement didn't
    return DefWindowProc(hWnd, message, wParam, lParam);
}