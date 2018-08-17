#pragma once

#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

#include "window.h"

class renderer {
public:
    renderer(window &w);
    ~renderer();
    void present();
    void clear();

private:
    IDXGISwapChain *swap_chain;            // Swap chain interface.
    ID3D11Device *dev;                     // Direct3D device interface.
    ID3D11DeviceContext *devcon;           // Direct3D device context.
    ID3D11RenderTargetView *back_buffer;   // Backbuffer.
};

