#include <Models.h>
#include <MeshLoader.h>


Dragon::Dragon()
{
    MeshLoader DragonObjFile("..\\..\\Resources\\dragon.obj");
    DragonObjFile.ParseMesh(&vertices, &indecies);
}


Dragon::~Dragon()
{
}


Pyramid::Pyramid()
{
    MeshLoader PyramidObjFile("..\\..\\Resources\\pyramid.obj");
    PyramidObjFile.ParseMesh(&vertices, &indecies);
}

Pyramid::~Pyramid()
{
}

uint8_t * Model::GetVertices()
{
    return reinterpret_cast<uint8_t *>(vertices.data());
}

size_t Model::SizeOfVertexBuffer()
{
    return vertices.size()*sizeof(VERTEX);
}

uint8_t * Model::GetIndecies()
{
    return reinterpret_cast<uint8_t *>(indecies.data());
}

size_t Model::SizeOfIndexBuffer()
{
    return indecies.size() * sizeof(INDEX);
}
