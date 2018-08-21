#include <vertex.h>
#include <renderer.h>
#include <string>

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
    swap_chain_descriptor.SampleDesc.Count = 4;
    swap_chain_descriptor.Windowed = true;
    swap_chain_descriptor.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

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

    init_pipeline();
    create_video_buffer();
}


void renderer::init_pipeline() {
    std::string shader_path = "src/shaders/shaders.hlsl";

    check_err(D3DX11CompileFromFile(shader_path.c_str(), 0, 0, "v_shader", "vs_4_0", 0, 0, 0, &vertex_buffer, 0, 0));
    check_err(D3DX11CompileFromFile(shader_path.c_str(), 0, 0, "p_shader", "ps_4_0", 0, 0, 0, &pixel_buffer, 0, 0));
    check_err(dev->CreateVertexShader(vertex_buffer->GetBufferPointer(), vertex_buffer->GetBufferSize(), NULL, &vertex_shader));
    check_err(dev->CreatePixelShader(pixel_buffer->GetBufferPointer(), pixel_buffer->GetBufferSize(), NULL, &pixel_shader));
    devcon->VSSetShader(vertex_shader, 0, 0);
    devcon->PSSetShader(pixel_shader, 0, 0);

    D3D11_INPUT_ELEMENT_DESC ied[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    check_err(dev->CreateInputLayout(ied, 2, vertex_buffer->GetBufferPointer(), vertex_buffer->GetBufferSize(), &layout));
    devcon->IASetInputLayout(layout);
}

void renderer::create_video_buffer() {
    vertex vertices[] = {
        { 0.0f, 0.5f, 0.0f, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f) },
        { 0.45f, -0.5, 0.0f, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f) },
        { -0.45f, -0.5f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f) }
    };

    D3D11_BUFFER_DESC buffer_descriptor;
    ZeroMemory(&buffer_descriptor, sizeof(buffer_descriptor));

    buffer_descriptor.Usage = D3D11_USAGE_DYNAMIC;                
    buffer_descriptor.ByteWidth = sizeof(vertex) * 3;             
    buffer_descriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;     
    buffer_descriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    

    check_err(dev->CreateBuffer(&buffer_descriptor, nullptr, &video_buffer));

    D3D11_MAPPED_SUBRESOURCE ms;
    check_err(devcon->Map(video_buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms));
    memcpy(ms.pData, vertices, sizeof(vertices));
    devcon->Unmap(video_buffer, NULL);
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
    devcon->ClearRenderTargetView(back_buffer, D3DXCOLOR(0.f, .2f, .4f, 1.f));
}

void renderer::render() {
    UINT stride = sizeof(vertex);
    UINT offset = 0;
    devcon->IASetVertexBuffers(0, 1, &video_buffer, &stride, &offset);
    devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    devcon->Draw(3, 0);
}

void renderer::present() {
    swap_chain->Present(0, 0);
}




