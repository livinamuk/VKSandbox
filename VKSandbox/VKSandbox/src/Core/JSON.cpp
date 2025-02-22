#pragma once
#include "nlohmann/json.hpp"
#include "json.h"
#include <fstream>

void to_json(nlohmann::json& j, const glm::vec3& v) {
    j = nlohmann::json{ v.x, v.y, v.z };
}

void from_json(const nlohmann::json& j, glm::vec3& v) {
    v.x = j.at(0).get<float>();
    v.y = j.at(1).get<float>();
    v.z = j.at(2).get<float>();
}

namespace JSON {

    nlohmann::json Vec3ToJson(const glm::vec3& v) {
        nlohmann::json j;
        to_json(j, v);
        return j;
    }

    glm::vec3 JsonToVec3(nlohmann::json& j) {
        glm::vec3 v;
        from_json(j, v);
        return v;
    }

    void SaveToFile(nlohmann::json& json, const std::string& filepath) {
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << json.dump(4);
            file.close();
        }
    }
}