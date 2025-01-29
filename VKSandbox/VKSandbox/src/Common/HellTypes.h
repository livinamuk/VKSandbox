#pragma once
#include "Common.h"
#include <stb_image.h>
#include <vector>
#include <glm/glm.hpp>
#include <iostream>
#include "HellEnums.h"

struct BlitRegion {
    int32_t originX = 0;
    int32_t originY = 0;
    int32_t width = 0;
    int32_t height = 0;
};

struct BlitRect {
    int32_t x0 = 0;
    int32_t y0 = 0;
    int32_t x1 = 0;
    int32_t y1 = 0;
};

struct RenderItem {
    glm::mat4 modelMatrix;
    int meshIndex;
    int baseColorTextureIndex;
    int normalTextureIndex;
    int rmaTextureIndex;
    int mousePickIndex;
};

struct RenderItem2D {
    glm::mat4 modelMatrix = glm::mat4(1);
    float colorTintR = 1.0f;
    float colorTintG = 1.0f;
    float colorTintB = 1.0f;
    int textureIndex = -1;
    int baseVertex;
    int baseIndex;
};

struct Vertex2D {
    glm::vec2 position;
    glm::vec2 uv;
    glm::vec4 color;
    int textureIndex;
};

struct MeshData2D {
    std::vector<Vertex2D> vertices;
    std::vector<uint32_t> indices;
};

struct Vertex {
    Vertex() = default;
    Vertex(glm::vec3 pos) {
        position = pos;
    }
    Vertex(glm::vec3 pos, glm::vec3 norm) {
        position = pos;
        normal = norm;
    }
    Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 texCoord) {
        position = pos;
        normal = norm;
        uv = texCoord;
    }
    Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 texCoord, glm::vec3 _tangent) {
        position = pos;
        normal = norm;
        uv = texCoord;
        tangent = _tangent;
    }
    glm::vec3 position = glm::vec3(0);
    glm::vec3 normal = glm::vec3(0);
    glm::vec2 uv = glm::vec2(0);
    glm::vec3 tangent = glm::vec3(0);
};

struct TextureData {
    int m_width = 0;
    int m_height = 0;
    int m_channelCount = 0;
    int m_dataSize = 0;
    int m_format = 0;
    int m_internalFormat = 0;
    void* m_data = nullptr;
    ImageDataType m_imadeDataType;
};

struct FileInfoOld {
    std::string fullpath;
    std::string directory;
    std::string filename;
    std::string filetype;
    std::string materialType;
};

struct FileInfo {
    std::string path;
    std::string name;
    std::string ext;
    std::string dir;
    std::string GetFileNameWithExtension() {
        if (ext.length() > 0) {
            return name + "." + ext;
        }
        else {
            return name;
        }
    }
};

struct Transform {
	glm::vec3 position = glm::vec3(0);
	glm::vec3 rotation = glm::vec3(0);
	glm::vec3 scale = glm::vec3(1);

    Transform() = default;

    explicit Transform(const glm::vec3& position, const glm::vec3& rotation = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f)) {
        this->position = position;
        this->rotation = rotation;
        this->scale = scale;
    }

	glm::mat4 to_mat4() {
		glm::mat4 m = glm::translate(glm::mat4(1), position);
		m *= glm::mat4_cast(glm::quat(rotation));
		m = glm::scale(m, scale);
		return m;
	};
};

struct Material {
    Material() {}
    std::string m_name = "UNDEFINED_STRING";
    int m_basecolor = 0;
    int m_normal = 0;
    int m_rma = 0;
};

struct QueuedTextureBake {
    void* texture = nullptr;
    int jobID = 0;
    int width = 0;
    int height = 0;
    int format = 0;
    int internalFormat = 0;
    int mipmapLevel = 0;
    int dataSize = 0;
    const void* data = nullptr;
    bool inProgress = false;
};

struct PlayerData {
    glm::mat4 projection;
    glm::mat4 inverseProjection;
    glm::mat4 view;
    glm::mat4 inverseView;
    glm::mat4 projectionView;
    glm::mat4 inverseProjectionView;
    glm::mat4 skyboxModelMatrix;
    int renderItemsCount;
    int renderItemsOffset;
    int renderItemsBlendedCount;
    int renderItemsBlendedOffset;
    int renderItemsAlphaDiscardedCount;
    int renderItemsAlphaDiscardedOffset;
    int renderItemsHairTopLayerCount;
    int renderItemsHairTopLayerOffset;
    int renderItemsHairBottomLayerCount;
    int renderItemsHairBottomLayerOffset;
};

struct RendererData {
    float gBufferWidth;
    float gBufferHeight;
    int splitscreenMode;
};

struct Resolutions {
    glm::ivec2 gBuffer;
    glm::ivec2 finalImage;
    glm::ivec2 ui;
    glm::ivec2 hair;
};