#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <iostream>
#include "window.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

IDXGISwapChain *swap_chain;             // the pointer to the swap chain interface
ID3D11Device *dev;                     // the pointer to our Direct3D device interface
ID3D11DeviceContext *devcon;           // the pointer to our Direct3D device context

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

void init_d3d(window &w) {
    DXGI_SWAP_CHAIN_DESC swap_chain_info;
    ZeroMemory(&swap_chain_info, sizeof(DXGI_ADAPTER_DESC));

    swap_chain_info.BufferCount = 1;
    swap_chain_info.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_info.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_info.OutputWindow = (HWND)w.native_window();
    swap_chain_info.BufferCount = 4;
    swap_chain_info.Windowed = true;

    D3D11CreateDeviceAndSwapChain(nullptr,
                                  D3D_DRIVER_TYPE_HARDWARE,
                                  nullptr, NULL, nullptr, NULL,
                                  D3D11_SDK_VERSION,
                                  &swap_chain_info,
                                  &swap_chain,
                                  &dev,
                                  nullptr,
                                  &devcon);
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

