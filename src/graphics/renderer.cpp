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

void renderer::create_video_buffer() {
    D3D11_BUFFER_DESC buffer_descriptor;
                                        
    ZeroMemory(&buffer_descriptor, sizeof(buffer_descriptor));

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
  //  video_buffer->Release();
  //  vertex_buffer->Release();
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

void renderer::render(mesh &m) {
    update();

    UINT stride = sizeof(vertex);
    UINT offset = 0;
    //devcon->IASetVertexBuffers(0, 1, &video_buffer, &stride, &offset); 
    devcon->IASetIndexBuffer(m.index_buffer, DXGI_FORMAT_R16_UINT, 0);
    devcon->IASetVertexBuffers(0, 1, &m.vertex_buffer, &stride, &offset);
    devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    devcon->DrawIndexed(m.indices.size(), 0, 0); //m.indices.size(), 0, 0);
}

void renderer::present() {
    check_err(swap_chain->Present(0, 0));
}


void renderer::update() {
    rotation += 0.005f;
    if (rotation > PI * 2)
        rotation = 0;

    DirectX::XMMATRIX world = DirectX::XMMatrixRotationY(rotation);

    //Set the View matrix
    auto projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, (float)640 / 480, .01f, 100.0f);


    wvp buffer;
    buffer.matrix = XMMatrixTranspose(world * camera * projection);
    devcon->UpdateSubresource(constant_buffer, 0, nullptr, &buffer, 0, 0);
    devcon->VSSetConstantBuffers(0, 1, &constant_buffer);
}


