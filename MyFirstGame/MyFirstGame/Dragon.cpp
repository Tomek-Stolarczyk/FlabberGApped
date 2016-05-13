#include <Dragon.h>
#include <MeshLoader.h>


Dragon::Dragon()
{
    MeshLoader DragonObjFile("..\\..\\Resources\\dragon.obj");
    DragonObjFile.ParseMesh();
}


Dragon::~Dragon()
{
}
