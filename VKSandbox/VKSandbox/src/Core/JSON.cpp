#pragma once
#include "JSON.h"
#include "Util.h"
#include <fstream>

namespace nlohmann {
    void to_json(nlohmann::json& j, const glm::vec3& v) {
        j = json::array({ v.x, v.y, v.z });
    }

    void to_json(nlohmann::json& j, const MeshBlendingInfo& info) {
        j = nlohmann::json{
            {"meshName", info.meshName},
            {"blendingMode", Util::BlendingModeToString(info.blendingMode)}
        };
    }

    void to_json(nlohmann::json& j, const MeshMaterialInfo& info) {
        j = nlohmann::json{
            {"meshName", info.meshName},
            {"materialName", info.materialName}
        };
    }

    void from_json(const nlohmann::json& j, glm::vec3& v) {
        v.x = j.at(0).get<float>();
        v.y = j.at(1).get<float>();
        v.z = j.at(2).get<float>();
    }

    void from_json(const nlohmann::json& j, MeshBlendingInfo& info) {
        j.at("meshName").get_to(info.meshName);
        std::string modeStr;
        j.at("blendingMode").get_to(modeStr);
        info.blendingMode = Util::StringToBlendingMode(modeStr);
    }

    void from_json(const nlohmann::json& j, MeshMaterialInfo& info) {
        j.at("meshName").get_to(info.meshName);
        j.at("materialName").get_to(info.materialName);
    }
}

namespace JSON {

    SectorCreateInfo LoadSector(const std::string& filepath) {

        std::ifstream file(filepath);
        std::stringstream buffer;
        buffer << file.rdbuf();

        SectorCreateInfo sectorCreateInfo;
        nlohmann::json json = nlohmann::json::parse(buffer.str());

        // Load Game Objects
        for (auto& jsonObject : json["GameObjects"]) {
            GameObjectCreateInfo& createInfo = sectorCreateInfo.gameObjects.emplace_back();
            createInfo.position = jsonObject["position"];
            createInfo.rotation = jsonObject["rotation"];
            createInfo.scale = jsonObject["scale"];
            createInfo.modelName = jsonObject["modelName"];
            createInfo.baseMaterial = jsonObject["baseMaterial"];
            createInfo.meshMaterialInfo = jsonObject["meshMaterials"];
            createInfo.meshBlendingInfo = jsonObject["meshBlendingModes"];
        }

        // Load lights
        for (auto& jsonObject : json["Lights"]) {
            LightCreateInfo& createInfo = sectorCreateInfo.lights.emplace_back();
            createInfo.position = jsonObject["position"];
            createInfo.color = jsonObject["color"];
            createInfo.radius = jsonObject["radius"];
            createInfo.strength = jsonObject["strength"];
            createInfo.type = jsonObject["type"];
        }

        // Load Pickups
        for (auto& jsonObject : json["PickUps"]) {
            PickUpCreateInfo& createInfo = sectorCreateInfo.pickUps.emplace_back();
            createInfo.position = jsonObject["position"];
            createInfo.rotation = jsonObject["rotation"];
            createInfo.pickUpType = jsonObject["type"];
        }

        // Load Trees
        for (auto& jsonObject : json["Trees"]) {
            TreeCreateInfo& createInfo = sectorCreateInfo.trees.emplace_back();
            createInfo.position = jsonObject["position"];
            createInfo.rotation = jsonObject["rotation"];
            createInfo.scale = jsonObject["scale"];
            createInfo.type = jsonObject["type"];
        }

        return sectorCreateInfo;
    }

    void SaveSector(const std::string& filepath, SectorCreateInfo& sectorCreateInfo) {

        nlohmann::json json;
        json["Lights"] = nlohmann::json::array();
        json["GameObjects"] = nlohmann::json::array();

        // Save Game Objects
        for (const GameObjectCreateInfo& createInfo : sectorCreateInfo.gameObjects) {
            json["GameObjects"].push_back(nlohmann::json{
                { "position", createInfo.position },
                { "rotation", createInfo.rotation },
                { "scale", createInfo.scale },
                { "modelName", createInfo.modelName },
                { "baseMaterial", createInfo.baseMaterial },
                { "meshMaterials", createInfo.meshMaterialInfo },
                { "meshBlendingModes", createInfo.meshBlendingInfo },
            });
        }

        // Save lights
        for (const LightCreateInfo& createInfo : sectorCreateInfo.lights) {
            json["Lights"].push_back(nlohmann::json{
                { "position", createInfo.position },
                { "color", createInfo.color },
                { "type", createInfo.type },
                { "radius", createInfo.radius },
                { "strength", createInfo.strength }
            });
        }


        // Save PickUps
        for (const PickUpCreateInfo& createInfo : sectorCreateInfo.pickUps) {
            json["PickUps"].push_back(nlohmann::json{
                { "position", createInfo.position },
                { "rotation", createInfo.rotation },
                { "type", createInfo.pickUpType }
            });
        }

        // Save Trees
        for (const TreeCreateInfo& createInfo : sectorCreateInfo.trees) {
            json["Trees"].push_back(nlohmann::json{
                { "position", createInfo.position },
                { "rotation", createInfo.rotation },
                { "scale", createInfo.scale },
                { "type", (int)createInfo.type }
            });
        }

        JSON::SaveToFile(json, filepath);
    }

    void SaveToFile(nlohmann::json& json, const std::string& filepath) {
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << json.dump(4);
            file.close();
            std::cout << "Saved " << filepath << "\n";
        }
    }
}