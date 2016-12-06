#pragma once

#include <DirectXMath.h>

struct VERTEX
{
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT2 texCoord;

    VERTEX(float v1, float v2, float v3) :
        pos(v1, v2, v3),
        texCoord(0,0)
    {

    }

};

struct cbPerObject
{
    DirectX::XMMATRIX  WVP;
};

typedef int INDEX;