#include "player.h"

using namespace DirectX;

player::player() {
    eye = XMVectorSet(-2.0f, 0.1f, -5.0f, 0.0f);
    at = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    camera = XMMatrixLookAtLH(eye, at, up);
}

void player::update_position(float x, float z) {
    at += XMVectorSet(x, 0, z, 0.f);
    eye += XMVectorSet(x, 0, z, 0.f);
    camera = XMMatrixLookAtLH(eye, at, up);
}

XMMATRIX player::get_camera() {
    return camera;
}

player::~player() {}
