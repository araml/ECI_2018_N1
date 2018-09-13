#pragma once


#include <DirectXMath.h>

class player {
public:
    player();
    ~player();
    void update_position(float x, float y);
    DirectX::XMMATRIX get_camera();
private:
    DirectX::XMVECTOR eye;
    DirectX::XMVECTOR at;
    DirectX::XMVECTOR up;
    DirectX::XMMATRIX camera;
};

