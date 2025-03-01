#pragma once
#include "FileFormats.h"
#include <string>

namespace File {
    // Heightmaps
    HeightMapData LoadHeightMap(const std::string filename);
    void SaveHeightMap(const HeightMapData& heightmapData);
    void PrintHeightMapHeader(HeightMapHeader header);

    // Models
    void ExportModel(const ModelData& modelData);
    ModelData ImportModel(const std::string& filepath);
    ModelHeader ReadModelHeader(const std::string& filepath);

    // Skinned Models
    void ExportSkinnedModel(const SkinnedModelData& modelData);
    SkinnedModelData ImportSkinnedModel(const std::string& filepath);
    SkinnedModelHeader ReadSkinnedModelHeader(const std::string& filepath);
    
    // I/O
    void DeleteFile(const std::string& filePath);

    // Time
    uint64_t GetCurrentTimestamp();
    std::string TimestampToString(uint64_t timestamp);
    uint64_t GetLastModifiedTime(const std::string& filePath);

    // Debug
    void ExportMeshDataToOBJ(const std::string& filepath, const MeshData& mesh);
    void ExportSkinnedMeshDataToOBJ(const std::string& filepath, const SkinnedMeshData& mesh);
    void PrintSkinnedModelHeader(SkinnedModelHeader header, const std::string& identifier);
    void PrintSkinnedMeshHeader(SkinnedMeshHeader header, const std::string& identifier);
    void PrintModelHeader(ModelHeader header, const std::string& identifier);
    void PrintMeshHeader(MeshHeader header, const std::string& identifier);
}