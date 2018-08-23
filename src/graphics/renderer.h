#pragma once

#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

#include <window.h>

template <typename T> 
void check_err(T err) {
    if (err != S_OK) {
        printf("Error!\n");
    } else {
        printf("OK!\n");
    }
}

class renderer {
public:
    renderer(window &w);
    ~renderer();
    void present();
    void render();
    void clear();

private:
    void init_pipeline(window &w);
    void create_video_buffer();
    void init_depth_stencil(window &w);

    IDXGISwapChain *swap_chain;            // Swap chain interface.
    ID3D11Device *dev;                     // Direct3D device interface.
    ID3D11DeviceContext *devcon;           // Direct3D device context.
    ID3D11RenderTargetView *back_buffer;   // Backbuffer.
    ID3D10Blob *vertex_buffer, *pixel_buffer;
    ID3D11PixelShader *pixel_shader;
    ID3D11VertexShader *vertex_shader;
    ID3D11Buffer *video_buffer;
    ID3D11InputLayout *layout;
    //Depth stencil
    ID3D11DepthStencilView *stencil_view;
    ID3D11Texture2D *stencil_buffer;
};

