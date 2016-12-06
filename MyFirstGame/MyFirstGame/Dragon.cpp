#include <Dragon.h>
#include <MeshLoader.h>


Dragon::Dragon()
{
    std::vector<std::tuple<float, float, float>> vertices;
    std::vector<std::tuple<int, int, int>> faces;
    MeshLoader DragonObjFile("..\\..\\Resources\\dragon.obj");
    DragonObjFile.ParseMesh(vertices, faces);
}


Dragon::~Dragon()
{
}
