/*#pragma once
#include "Map.h"
#include "JSON.h"

namespace Map {

    MapData Load(const std::string& filepath) {
        MapData mapData;
        return mapData;
    }
    
    void Save(const std::string& filepath, const MapData& mapData) {
        nlohmann::json json;
        json["doors"] = nlohmann::json::array();
        json["lights"] = nlohmann::json::array();
        json["game_objects"] = nlohmann::json::array();
        json["windows"] = nlohmann::json::array();

        for (const LightCreateInfo& createInfo : mapData.lights) {
            json["lights"].push_back(nlohmann::json{
                { "position", JSON::Vec3ToJson(createInfo.position)},
                { "color", JSON::Vec3ToJson(createInfo.color) },
                { "type", createInfo.type },
                { "radius", createInfo.radius },
                { "strength", createInfo.strength }
            });
        }

        JSON::SaveToFile(json, filepath);
    }

}*/