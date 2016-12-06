#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <Common.h>

class MeshLoader
{
public:
    void ParseMesh(std::vector<VERTEX> *vertices, std::vector<INDEX> *indecies);

    MeshLoader();
    MeshLoader(std::string meshLocation);
    ~MeshLoader();

private:
    std::ifstream inputStream;
};


