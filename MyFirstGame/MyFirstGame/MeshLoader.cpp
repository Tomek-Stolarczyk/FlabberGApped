#include <MeshLoader.h>
#include <ErrorMessage.h>
#include <sstream>

using namespace std;

void MeshLoader::ParseMesh(vector<VERTEX> *vertices, vector<INDEX> *indecies)
{
    string lineContents;
    
    while (getline(inputStream, lineContents))
    {
        stringstream ss(lineContents);
        string header;

        ss >> header;

        if (header.compare("v") == 0)
        {
            float v1, v2, v3;
            ss >> v1;
            ss >> v2;
            ss >> v3;
            vertices->push_back(VERTEX(v1, v2, v3));
        }
        
        else if (header.compare("f") == 0)
        {
            int i1, i2, i3;
            ss >> i1;
            ss >> i2;
            ss >> i3;
            indecies->push_back(i1);
            indecies->push_back(i2);
            indecies->push_back(i3);
        }
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
