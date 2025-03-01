#pragma once
#include "Renderer/Types/TextureArray.h"

namespace HeightMapManager {
    void Init();

    OpenGLTextureArray& GetGLTextureArray();
}