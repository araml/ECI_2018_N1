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

IDXGISwapChain *swap_chain;            // Swap chain interface.
ID3D11Device *dev;                     // Direct3D device interface.
ID3D11DeviceContext *devcon;           // Direct3D device context.
ID3D11RenderTargetView *back_buffer;   // Backbuffer.

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
    DXGI_SWAP_CHAIN_DESC swap_chain_descriptor;
    ZeroMemory(&swap_chain_descriptor, sizeof(DXGI_SWAP_CHAIN_DESC));

    swap_chain_descriptor.BufferCount = 1;
    swap_chain_descriptor.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_descriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_descriptor.OutputWindow = (HWND)w.native_window();
    swap_chain_descriptor.SampleDesc.Count = 1;
    swap_chain_descriptor.SampleDesc.Quality = 0;
    swap_chain_descriptor.Windowed = true;

    D3D11CreateDeviceAndSwapChain(nullptr,
                                  D3D_DRIVER_TYPE_HARDWARE,
                                  nullptr, NULL, nullptr, NULL,
                                  D3D11_SDK_VERSION,
                                  &swap_chain_descriptor,
                                  &swap_chain,
                                  &dev,
                                  nullptr,
                                  &devcon);

    ID3D11Texture2D *temp_back_buffer;
    swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&temp_back_buffer);

    dev->CreateRenderTargetView(temp_back_buffer, nullptr, &back_buffer);
    temp_back_buffer->Release();
    devcon->OMSetRenderTargets(1, &back_buffer, nullptr);

    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Height = static_cast<float>(w.height());
    viewport.Width = static_cast<float>(w.width());
    devcon->RSSetViewports(1, &viewport);
}

void render_frame() {
    devcon->ClearRenderTargetView(back_buffer, D3DXCOLOR(0.f, .2f, .4f, 1.f));
    swap_chain->Present(0, 0);
}

void close_d3d() {
    swap_chain->Release();
    back_buffer->Release();
    dev->Release();
    devcon->Release();
}

// the entry point for any Windows program
int main() {
    FreeConsole();
    
    window w(500, 400, false);
    init_d3d(w);
    // main loop
    while (true) {
        if (process_events())
            break;

        render_frame();
    }

    close_d3d();
    // return 0 to Windows
    return 0;
}

