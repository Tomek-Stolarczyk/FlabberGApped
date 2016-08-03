#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <hash_map>
#include <tuple>

class MeshLoader
{
public:
    std::tuple<int, int> ParseMesh(std::vector<std::tuple<float, float, float>> &vertices, std::vector<std::tuple<int, int, int>> &faces);

    MeshLoader();
    MeshLoader(std::string meshLocation);
    ~MeshLoader();
    void Handle_Vertex(std::stringstream &lineContents, std::vector<std::tuple<float, float, float>> &vertices);
    void Handle_face(std::stringstream &ss, std::vector<std::tuple<int, int, int>> & faces);
private:
    std::ifstream inputStream;
    int NumOfVerticies;
    int NumOfFaces;
};


