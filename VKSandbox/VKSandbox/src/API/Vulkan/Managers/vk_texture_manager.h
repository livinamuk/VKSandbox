#pragma once
#include "../../../Renderer/Types/Texture.h"

namespace VulkanTextureManager {
    void AllocateTexture(Texture& vkTexture);
    void CleanupTexture(Texture& vkTexture);
    void GenerateMipmaps(Texture& texture);
}
