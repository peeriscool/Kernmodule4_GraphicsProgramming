#include <iostream>
#include <fstream>
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
#include <sstream>
#include <string>
#include <vector>

std::vector<Material> loadMTL(const std::string& filename) {
    std::vector<Material> materials;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Could not open the file " << filename << std::endl;
        return materials;
    }

    std::string line;
    Material currentMaterial;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "newmtl") {
            if (!currentMaterial.name.empty()) {
                materials.push_back(currentMaterial);
            }
            iss >> currentMaterial.name;
        }
        else if (prefix == "Ns") {
            iss >> currentMaterial.Ns;
        }
        else if (prefix == "Ka") {
            iss >> currentMaterial.Ka[0] >> currentMaterial.Ka[1] >> currentMaterial.Ka[2];
        }
        else if (prefix == "Kd") {
            iss >> currentMaterial.Kd[0] >> currentMaterial.Kd[1] >> currentMaterial.Kd[2];
        }
        else if (prefix == "Ks") {
            iss >> currentMaterial.Ks[0] >> currentMaterial.Ks[1] >> currentMaterial.Ks[2];
        }
        else if (prefix == "Ke") {
            iss >> currentMaterial.Ke[0] >> currentMaterial.Ke[1] >> currentMaterial.Ke[2];
        }
        else if (prefix == "Ni") {
            iss >> currentMaterial.Ni;
        }
        else if (prefix == "d") {
            iss >> currentMaterial.d;
        }
        else if (prefix == "illum") {
            iss >> currentMaterial.illum;
        }
    }

    if (!currentMaterial.name.empty()) {
        materials.push_back(currentMaterial);
    }

    file.close();
    return materials;
}
/*
Step 3: Use the Loaded Materials
Once you have the materials loaded into your application, you can use them in your rendering loop or shader setup :
*/
std::vector<Material>  MTLHelperFunction() {
    std::string mtlFilePath = "resources/models/Ufo/Ufo.mtl";
    std::vector<Material> materials = loadMTL(mtlFilePath);

    for (const auto& material : materials) {
        std::cout << "Material: " << material.name << std::endl;
        std::cout << "  Ns: " << material.Ns << std::endl;
        std::cout << "  Ka: " << material.Ka[0] << " " << material.Ka[1] << " " << material.Ka[2] << std::endl;
        std::cout << "  Kd: " << material.Kd[0] << " " << material.Kd[1] << " " << material.Kd[2] << std::endl;
        std::cout << "  Ks: " << material.Ks[0] << " " << material.Ks[1] << " " << material.Ks[2] << std::endl;
        std::cout << "  Ke: " << material.Ke[0] << " " << material.Ke[1] << " " << material.Ke[2] << std::endl;
        std::cout << "  Ni: " << material.Ni << std::endl;
        std::cout << "  d: " << material.d << std::endl;
        std::cout << "  illum: " << material.illum << std::endl;
    }

    return materials;
}