#include <vertex.h>
#include <renderer.h>
#include <string>
#include <DirectXColors.h>
#include <iostream>
using namespace DirectX;

constexpr double PI = 3.14;

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")


renderer::renderer(window &w) {
    DXGI_SWAP_CHAIN_DESC swap_chain_descriptor;
    ZeroMemory(&swap_chain_descriptor, sizeof(DXGI_SWAP_CHAIN_DESC));

    swap_chain_descriptor.BufferCount = 1;
    swap_chain_descriptor.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;    
    swap_chain_descriptor.BufferDesc.Height = w.height();
    swap_chain_descriptor.BufferDesc.Height = w.width();
    swap_chain_descriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_descriptor.OutputWindow = (HWND)w.native_window();
    swap_chain_descriptor.SampleDesc.Count = 1;
    swap_chain_descriptor.Windowed = true;
    swap_chain_descriptor.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    D3D11CreateDeviceAndSwapChain(nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr, D3D11_CREATE_DEVICE_DEBUG, nullptr, NULL,
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
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.Height = static_cast<float>(w.height());
    viewport.Width = static_cast<float>(w.width());
    devcon->RSSetViewports(1, &viewport);

    init_pipeline(w);
    create_video_buffer();
}

void renderer::init_pipeline(window &w) {
    std::string shader_path = "src/shaders/shaders.hlsl";

    check_err(D3DX11CompileFromFile(shader_path.c_str(), 0, 0, "v_shader", "vs_4_0", 0, 0, 0, &vertex_buffer, 0, 0));
    check_err(D3DX11CompileFromFile(shader_path.c_str(), 0, 0, "p_shader", "ps_4_0", 0, 0, 0, &pixel_buffer, 0, 0));
    check_err(dev->CreateVertexShader(vertex_buffer->GetBufferPointer(), vertex_buffer->GetBufferSize(), NULL, &vertex_shader));
    check_err(dev->CreatePixelShader(pixel_buffer->GetBufferPointer(), pixel_buffer->GetBufferSize(), NULL, &pixel_shader));
    devcon->VSSetShader(vertex_shader, 0, 0);
    devcon->PSSetShader(pixel_shader, 0, 0);

    D3D11_INPUT_ELEMENT_DESC ied[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    check_err(dev->CreateInputLayout(ied, 2, vertex_buffer->GetBufferPointer(), vertex_buffer->GetBufferSize(), &layout));
    devcon->IASetInputLayout(layout);

    init_depth_stencil(w);
}

void renderer::init_depth_stencil(window &w) {
    D3D11_TEXTURE2D_DESC stencil_descriptor;
        
    stencil_descriptor.Width = w.width();
    stencil_descriptor.Height = w.height();
    stencil_descriptor.MipLevels = 1;
    stencil_descriptor.ArraySize = 1;
    stencil_descriptor.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    stencil_descriptor.SampleDesc.Count = 1;
    stencil_descriptor.SampleDesc.Quality = 0;
    stencil_descriptor.Usage = D3D11_USAGE_DEFAULT;
    stencil_descriptor.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    stencil_descriptor.CPUAccessFlags = 0;
    stencil_descriptor.MiscFlags = 0;
    
    check_err(dev->CreateTexture2D(&stencil_descriptor, nullptr, &stencil_buffer));
    check_err(dev->CreateDepthStencilView(stencil_buffer, nullptr, &stencil_view));

    devcon->OMSetRenderTargets(1, &back_buffer, stencil_view);
}

int num_vert;

void renderer::create_video_buffer() {
    vertex vertices[] = {
    
       { -0.5f, -0.5f, -0.5f, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f) },
        { -0.5f,  0.5, -0.5f, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f) },
        {  0.5f,  0.5f, -0.5f, D3DXCOLOR(0.0f, 1.f, 1.0f, 1.0f) },
        {  0.5f,  -0.5f, -0.5f, D3DXCOLOR(1.0f, 0.f, 0.f, 1.0f) },

        { -0.5f, -0.5f, .5f, D3DXCOLOR(1.f, 0.0f, 1.f, 1.0f) },
        { -0.5f,  0.5, .5f, D3DXCOLOR(1.0f, 1.f, 0.0f, 1.0f) },
        { 0.5f,  0.5f, .5f, D3DXCOLOR(1.0f, 1.0f, 1.f, 1.0f) },
        { 0.5f,  -0.5f, .5f, D3DXCOLOR(0.f, 0.f, 0.f, 1.0f) },
   
    };

    WORD indices[] = {
         0, 1, 2,
         0, 2, 3,
         
         5, 0, 4,
         5, 1, 0,
         // Right face
         3, 2, 6,
         3, 6, 7,

         6, 4, 7,
         6, 5, 4,

         1, 5, 6,
         1, 6, 2,

         0, 4, 7,
         0, 7, 3,
   };

    num_vert = sizeof(indices) / sizeof(WORD);

    D3D11_BUFFER_DESC index_buffer_descriptor;
    ZeroMemory(&index_buffer_descriptor, sizeof(index_buffer_descriptor));
    index_buffer_descriptor.Usage = D3D11_USAGE_DEFAULT;
    index_buffer_descriptor.ByteWidth = sizeof(indices);
    index_buffer_descriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
    index_buffer_descriptor.CPUAccessFlags = 0;
    index_buffer_descriptor.MiscFlags = 0;

    ID3D11Buffer *index_buffer;
    D3D11_SUBRESOURCE_DATA index_data;
    index_data.pSysMem = indices;
    dev->CreateBuffer(&index_buffer_descriptor, &index_data, &index_buffer);
    devcon->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R16_UINT, 0);

    D3D11_BUFFER_DESC buffer_descriptor;
    ZeroMemory(&buffer_descriptor, sizeof(buffer_descriptor));

    buffer_descriptor.Usage = D3D11_USAGE_DYNAMIC;                
    buffer_descriptor.ByteWidth = sizeof(vertices);             
    buffer_descriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;     
    buffer_descriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    

    check_err(dev->CreateBuffer(&buffer_descriptor, nullptr, &video_buffer));

    D3D11_MAPPED_SUBRESOURCE ms;
    check_err(devcon->Map(video_buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms));
    memcpy(ms.pData, vertices, sizeof(vertices));
    devcon->Unmap(video_buffer, NULL);

    buffer_descriptor.Usage = D3D11_USAGE_DEFAULT;
    buffer_descriptor.ByteWidth = sizeof(wvp);
    buffer_descriptor.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    buffer_descriptor.CPUAccessFlags = 0;
    check_err(dev->CreateBuffer(&buffer_descriptor, nullptr, &constant_buffer));
   
}

renderer::~renderer() {
    vertex_shader->Release();
    pixel_shader->Release();
    layout->Release();
    video_buffer->Release();
    vertex_buffer->Release();
    pixel_buffer->Release();
    swap_chain->Release();
    back_buffer->Release();
    dev->Release();
    devcon->Release();
}

void renderer::clear() {
    devcon->ClearRenderTargetView(back_buffer, Colors::MidnightBlue);
    devcon->ClearDepthStencilView(stencil_view, D3D11_CLEAR_DEPTH , 1.0f, 0);
}

void renderer::render() {
    update();

    UINT stride = sizeof(vertex);
    UINT offset = 0;
    devcon->IASetVertexBuffers(0, 1, &video_buffer, &stride, &offset);
    devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    devcon->DrawIndexed(num_vert, 0, 0);
}

void renderer::present() {
    check_err(swap_chain->Present(0, 0));
}


void renderer::update() {
    rotation += 0.005f;
    if (rotation > PI * 2)
        rotation = 0;

    DirectX::XMMATRIX world = DirectX::XMMatrixRotationY(rotation);


    auto eye = XMVectorSet(0.0f, 1.0f, -3.0f, 0.0f);
    ///////////////**************new**************////////////////////
    auto at = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    auto up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    //Set the View matrix
    auto camera = XMMatrixLookAtLH(eye, at, up);
    auto projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, (float)640 / 480, .01f, 100.0f);



    wvp buffer;
    buffer.matrix = XMMatrixTranspose(world * camera * projection);
    devcon->UpdateSubresource(constant_buffer, 0, nullptr, &buffer, 0, 0);
    devcon->VSSetConstantBuffers(0, 1, &constant_buffer);
}


