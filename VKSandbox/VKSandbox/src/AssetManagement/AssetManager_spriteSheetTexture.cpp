#include "AssetManager.h"

void AssetManager::BuildSpriteSheetTextures() {
    std::vector<SpriteSheetTexture>& spriteSheetTextures = GetSpriteSheetTextures();
    for (SpriteSheetTexture& spriteSheetTexture : spriteSheetTextures) {
        spriteSheetTexture.Init();
    }
}