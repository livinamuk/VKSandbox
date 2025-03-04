#include "File.h"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include "Util.h"

namespace File {

    HeightMapData LoadHeightMap(const std::string filename) {
        HeightMapData heightmapData;
        HeightMapHeader header;

        // Open the file
        std::string filepath = "res/height_maps/" + filename;
        std::ifstream ifs(filepath, std::ios::binary);
        if (!ifs) {
            std::cerr << "File::LoadHeightMap() failed to open file for reading: " << filepath << "\n";
            return heightmapData;
        }

        // Read header
        ifs.read(reinterpret_cast<char*>(&header), sizeof(header));

        // Set dimensions and name from header
        heightmapData.name = header.name;
        heightmapData.width = header.width;
        heightmapData.height = header.height;

        // Read the pixel data
        size_t floatCount = static_cast<size_t>(header.width) * header.height;
        heightmapData.data.resize(floatCount);
        ifs.read(reinterpret_cast<char*>(heightmapData.data.data()), floatCount * sizeof(float));

        // Close file
        ifs.close();

        //std::cout << "Loaded: " << filepath << " " << Util::GetFileSize(filepath) << " bytes\n";
        //PrintHeightMapHeader(header);
    }

    void SaveHeightMap(const HeightMapData& heightmapData) {
        HeightMapHeader header {};

        // Error check
        size_t floatCount = heightmapData.width * heightmapData.height;
        if (heightmapData.data.size() != floatCount) {
            std::cout << "File::SaveHeightMap() failed because data.size() was " << heightmapData.data.size() << " but width(" << heightmapData.width << ") * height(" << heightmapData.height << ") equals " << floatCount << "\n";
            return;
        }

        // Fill the header signature
        std::memset(header.signature, 0, sizeof(header.signature));
        strncpy_s(header.signature, sizeof(header.signature), HEIGHT_MAP_SIGNATURE, _TRUNCATE);

        // Fill the header name
        std::memset(header.name, 0, sizeof(header.name));
        strncpy_s(header.name, sizeof(header.name), heightmapData.name.c_str(), _TRUNCATE);

        // Fill the header dimensions
        header.width = heightmapData.width;
        header.height = heightmapData.height;

        // Write the header
        const std::string filepath = "res/height_maps/" + heightmapData.name + ".heightmap";
        std::ofstream ofs(filepath, std::ios::binary);
        ofs.write(reinterpret_cast<const char*>(&header), sizeof(header));

        // Write the pixel data
        ofs.write(reinterpret_cast<const char*>(heightmapData.data.data()), floatCount * sizeof(float));

        // Close file
        ofs.close();

        std::cout << "Saved: " << filepath << " " << Util::GetFileSize(filepath) << " bytes\n";
        PrintHeightMapHeader(header);
    }

    void PrintHeightMapHeader(HeightMapHeader header) {
        std::cout << HEIGHT_MAP_SIGNATURE << "\n";
        std::cout << " - name:   " << header.name << "\n";
        std::cout << " - width:  " << header.width << "\n";
        std::cout << " - height: " << header.height << "\n";
    }
}