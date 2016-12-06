#pragma once

#include <vector>
#include <Common.h>

class Model
{
public:
    uint8_t * GetVertices();
    size_t SizeOfVertexBuffer();

    uint8_t * GetIndecies();
    size_t SizeOfIndexBuffer();

protected:
    std::vector<VERTEX> vertices;
    std::vector<INDEX> indecies;
};

class Dragon : public Model
{
public:
    Dragon();
    ~Dragon();
};


class Pyramid : public Model
{
public:
    Pyramid();
    ~Pyramid();
};
