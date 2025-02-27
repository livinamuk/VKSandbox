#pragma once
#include "HellTypes.h"
#include "CreateInfo.h"
#include <nlohmann/json.hpp>
#include <glm/glm.hpp>

namespace nlohmann {
    void to_json(nlohmann::json& j, const glm::vec3& v);
    void to_json(nlohmann::json& j, const MeshBlendingInfo& info);
    void to_json(nlohmann::json& j, const MeshMaterialInfo& info);
    void from_json(const nlohmann::json& j, glm::vec3& v);
    void from_json(const nlohmann::json& j, MeshBlendingInfo& info);
    void from_json(const nlohmann::json& j, MeshMaterialInfo& info);
}

namespace JSON {
    SectorCreateInfo LoadSector(const std::string& filepath);
    void SaveSector(const std::string& filepath, SectorCreateInfo& sectorCreateInfo);
    void SaveToFile(nlohmann::json& json, const std::string& filepath);
}