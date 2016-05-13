#include <MeshLoader.h>
#include <ErrorMessage.h>

using namespace std;

void MeshLoader::ParseMesh()
{
    string lineContents;
    while (inputStream)
    {
        getline(inputStream, lineContents);

        if (lineContents[0] == 'v') // vertex
            globalErrorMessage.LogMessage(lineContents);
    }
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
}


MeshLoader::~MeshLoader()
{
    inputStream.close();
}
