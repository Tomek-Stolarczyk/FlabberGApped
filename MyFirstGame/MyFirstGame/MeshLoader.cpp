#include <MeshLoader.h>
#include <ErrorMessage.h>
#include <hash_map>
#include <tuple>
#include <sstream>
#include <vector>

using namespace std;

tuple<int, int> MeshLoader::ParseMesh(std::vector<std::tuple<float, float, float>> &vertices, 
                                std::vector<std::tuple<int, int, int>> &faces)
{
    string lineContents;
    char msg[5];
    while (inputStream)
    {
        getline(inputStream, lineContents);
        stringstream ss(lineContents);

        ss >> msg;

        if (strcmp(msg, "v") == 0) // vertex
            Handle_Vertex(ss, vertices);

        if (strcmp(msg, "f") == 0) // face
            Handle_face(ss, faces);
    }

    return tuple<int, int>(NumOfVerticies, NumOfFaces);
}

MeshLoader::MeshLoader()
{
    globalErrorMessage.ThrowError("Must provide stream path for mesh");
}

MeshLoader::MeshLoader(string meshLocation) :
    NumOfVerticies(0), 
    NumOfFaces(0)
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

void MeshLoader::Handle_Vertex(std::stringstream &lineContents, std::vector<std::tuple<float, float, float>> &vertices)
{
    float v1;
    float v2;
    float v3;

    lineContents >> v1;
    lineContents >> v2;
    lineContents >> v3;

    vertices.push_back(tuple<float, float, float>(v1, v2, v3));
    NumOfVerticies ++;
}

void MeshLoader::Handle_face(std::stringstream &lineContents, std::vector<std::tuple<int, int, int>> & faces)
{
    int i1;
    int i2;
    int i3;

    lineContents >> i1;
    lineContents >> i2;
    lineContents >> i3;

    faces.push_back(tuple<int, int, int>(i1, i2, i3));
    NumOfFaces++;
}
