#include "AssetManager.h"

void AssetManager::BuildSpriteSheetTextures() {
    std::vector<SpriteSheetTexture>& spriteSheetTextures = GetSpriteSheetTextures();
    for (SpriteSheetTexture& spriteSheetTexture : spriteSheetTextures) {
        spriteSheetTexture.Init();
    }
}

SpriteSheetTexture* AssetManager::GetSpriteSheetTextureByName(const std::string& textureName) {
    std::vector<SpriteSheetTexture>& spriteSheetTextures = GetSpriteSheetTextures();
    for (SpriteSheetTexture& spriteSheetTexture : spriteSheetTextures) {
        if (spriteSheetTexture.GetFileInfo().name == textureName) {
            return &spriteSheetTexture;
        }
    }
    return nullptr;
}