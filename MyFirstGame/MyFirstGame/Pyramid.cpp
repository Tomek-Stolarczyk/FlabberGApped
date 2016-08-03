#include "Pyramid.h"
#include <hash_map>
#include <tuple>
#include "MeshLoader.h"
#include <Common.h>

Pyramid::Pyramid()
{
    MeshLoader DragonObjFile("..\\..\\Resources\\Pyramid.obj");
    std::tuple<int, int> result = DragonObjFile.ParseMesh(vertices, faces);
    NumOfVertecies = std::get<0>(result);
    NumOFIndecies = std::get<1>(result);
    NumOFIndecies *= 3;
}


Pyramid::~Pyramid()
{
}

VERTEX *Pyramid::GetVertexData()
{
    VERTEX* data = new VERTEX[NumOfVertecies];
    for (int i = 0; i < NumOfVertecies; i++)
    {
        data[i].pos.x = std::get<0>(vertices.at(i));
        data[i].pos.y = std::get<1>(vertices.at(i));
        data[i].pos.z = std::get<2>(vertices.at(i));
    }

    return data;
}

int Pyramid::GetNumOfVertices()
{
    return NumOfVertecies;
}

int * Pyramid::GetIndexData()
{
    int* data = new int[NumOFIndecies];
    for (int i = 0; i < (NumOFIndecies/3); i++)
    {
        data[i * 3 + 0] = std::get<0>(faces.at(i));
        data[i * 3 + 1] = std::get<1>(faces.at(i));
        data[i * 3 + 2] = std::get<2>(faces.at(i));
    }

    return data;
}

int Pyramid::GetNumOfIndecies()
{
    return NumOFIndecies;
}
