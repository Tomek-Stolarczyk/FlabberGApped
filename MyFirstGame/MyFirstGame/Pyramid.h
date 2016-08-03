#pragma once

#include <vector>
#include <tuple>
#include "Common.h"

class Pyramid
{
public:
    Pyramid();
    ~Pyramid();
    int* GetIndexData();
    VERTEX *GetVertexData();
    int GetNumOfVertices();
    int GetNumOfIndecies();
    std::vector<std::tuple<float, float, float>> vertices;
    std::vector<std::tuple<int, int, int>> faces;

    int NumOFIndecies;
    int NumOfVertecies;
};

