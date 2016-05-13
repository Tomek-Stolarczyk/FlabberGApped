#pragma once

#include <DirectXMath.h>

typedef struct
{
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT2 texCoord;
} VERTEX, *pVertex;

struct cbPerObject
{
    DirectX::XMMATRIX  WVP;
};