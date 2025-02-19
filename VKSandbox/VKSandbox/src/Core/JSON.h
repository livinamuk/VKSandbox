#pragma once
#include "HellTypes.h"
#include <nlohmann/json.hpp>
#include <glm/glm.hpp>

namespace JSON {
    nlohmann::json Vec3ToJson(const glm::vec3& v);
    void SaveToFile(nlohmann::json& json, const std::string& filepath);
}