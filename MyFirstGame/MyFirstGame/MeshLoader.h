#pragma once

#include <iostream>
#include <fstream>
#include <string>

class MeshLoader
{
public:
    void ParseMesh();

    MeshLoader();
    MeshLoader(std::string meshLocation);
    ~MeshLoader();

private:
    std::ifstream inputStream;
};


