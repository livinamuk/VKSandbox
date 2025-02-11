#pragma once
#include "HellTypes.h"
#include <string>
#include "../File/FileFormats.h"

namespace AssimpImporter {
    ModelData ImportFbx(const std::string filepath);
    SkinnedModelData ImportSkinnedFbx(const std::string filepath);
}