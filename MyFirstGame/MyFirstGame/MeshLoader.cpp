#include <MeshLoader.h>
#include <ErrorMessage.h>

using namespace std;

void MeshLoader::ParseMesh()
{

}

MeshLoader::MeshLoader()
{
    globalErrorMessage.ThrowError("Must provide stream path for mesh");
}

MeshLoader::MeshLoader(string meshLocation)
{
    inputStream.open(meshLocation, ios::out);
    if (!inputStream.is_open())
    {
        inputStream.close();
        string errMsg = "Failed to open file stream on location ";
        errMsg.append(meshLocation);
        globalErrorMessage.ThrowError(errMsg);
    }
    inputStream.close();
}


MeshLoader::~MeshLoader()
{
}
