#ifndef MATERIAL_LOADER_H
#define MATERIAL_LOADER_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

struct Material {
    std::string name;
    float Ns;
    float Ka[3];
    float Kd[3];
    float Ks[3];
    float Ke[3];
    float Ni;
    float d;
    int illum;
};

std::vector<Material> loadMTL(const std::string& filename);

std::vector<Material> MTLHelperFunction();

#endif // MATERIAL_LOADER_H
