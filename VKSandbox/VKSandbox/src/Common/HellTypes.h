#pragma once
#define GLM_FORCE_SILENT_WARNINGS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <vector>
#include <iostream>
#include "HellEnums.h"
#include "HellDefines.h"
#include "Input/keycodes.h"

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
    glm::mat4 modelMatrix = glm::mat4(1);
    glm::mat4 inverseModelMatrix = glm::mat4(1);
    glm::vec4 aabbMin = glm::vec4(0);
    glm::vec4 aabbMax = glm::vec4(0);

    int meshIndex = 0;
    int baseColorTextureIndex = 0;
    int normalMapTextureIndex = 0;
    int rmaTextureIndex = 0;

    int mousePickType = 0;
    int mousePickIndex = 0;
    int baseSkinnedVertex = 0;
    int ignoredViewportIndex = -1;

    int exclusiveViewportIndex = -1;
    int skinned = 0;    // True or false
    float emissiveR = 0.0f;
    float emissiveG = 0.0f;

    float emissiveB = 0.0f;
    float padding0 = 0.0f;
    float padding1 = 0.0f;
    float padding2 = 0.0f;
};

//struct SkinnedRenderItem {
//    glm::mat4 modelMatrix = glm::mat4(1);
//    glm::mat4 inverseModelMatrix = glm::mat4(1);
//    glm::vec4 aabbMin = glm::vec4(0);
//    glm::vec4 aabbMax = glm::vec4(0);
//    int srcMeshIndex;
//    int baseColorTextureIndex;
//    int normalTextureIndex;
//    int rmaTextureIndex;
//    int baseVertex;
//    int padding0;
//    int padding1;
//    int padding2;
//};

struct RenderItem2D {
    glm::mat4 modelMatrix = glm::mat4(1);
    float colorTintR = 1.0f;
    float colorTintG = 1.0f;
    float colorTintB = 1.0f;
    int textureIndex = -1;
    int baseVertex = 0;
    int baseIndex = 0;
};

struct SpriteSheetRenderItem {
    glm::vec4 position;
    glm::vec4 rotation;
    glm::vec4 scale;
    int textureIndex;
    int frameIndex;
    int frameIndexNext;
    int rowCount;
    int columnCount;
    int isBillboard;
    float mixFactor;
    float padding;
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

struct WeightedVertex {
    glm::vec3 position = glm::vec3(0);
    glm::vec3 normal = glm::vec3(0);
    glm::vec2 uv = glm::vec2(0);
    glm::vec3 tangent = glm::vec3(0);
    glm::ivec4 boneID = glm::ivec4(0);
    glm::vec4 weight = glm::vec4(0);

    bool operator==(const Vertex& other) const {
        return position == other.position && normal == other.normal && uv == other.uv;
    }
};

struct TextureData {
    int m_width = 0;
    int m_height = 0;
    int m_channelCount = 0;
    int m_dataSize = 0;
    int m_format = 0;
    int m_internalFormat = 0;
    void* m_data = nullptr;
    ImageDataType m_imageDataType;
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

struct AnimatedTransform {
    AnimatedTransform() = default;
    AnimatedTransform(glm::mat4 matrix) {
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(matrix, scale, rotation, translation, skew, perspective);
    }
    glm::mat4 to_mat4() {
        glm::mat4 m = glm::translate(glm::mat4(1), translation);
        m *= glm::mat4_cast(rotation);
        m = glm::scale(m, scale);
        return m;
    };
    glm::vec3 to_forward_vector() {
        glm::quat q = glm::quat(rotation);
        return glm::normalize(q * glm::vec3(0.0f, 0.0f, 1.0f));
    }
    glm::vec3 translation = glm::vec3(0);
    glm::quat rotation = glm::quat(1, 0, 0, 0);
    glm::vec3 scale = glm::vec3(1);
};

struct Material {
    Material() {}
    std::string m_name = UNDEFINED_STRING;
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

struct ViewportData {
    glm::mat4 projection;
    glm::mat4 inverseProjection;
    glm::mat4 view;
    glm::mat4 inverseView;
    glm::mat4 projectionView;
    glm::mat4 inverseProjectionView;
    glm::mat4 skyboxProjectionView;
    glm::mat4 flashlightProjectionView;
    int xOffset;
    int yOffset;
    int width;
    int height;
    float posX;  // 0 t0 1 range
    float posY;  // 0 t0 1 range
    float sizeX; // 0 t0 1 range
    float sizeY; // 0 t0 1 range
    glm::vec4 frustumPlane0;
    glm::vec4 frustumPlane1;
    glm::vec4 frustumPlane2;
    glm::vec4 frustumPlane3;
    glm::vec4 frustumPlane4;
    glm::vec4 frustumPlane5;
    glm::vec4 flashlightDir;
    glm::vec4 flashlightPosition;
    float flashlightModifer;
    float padding0;
    float padding1;
    float padding2;
};

struct RendererData {
    float nearPlane;
    float farPlane;
    float gBufferWidth;
    float gBufferHeight;
    float hairBufferWidth;
    float hairBufferHeight;
    int splitscreenMode;
    float time;
};

struct Resolutions {
    glm::ivec2 gBuffer;
    glm::ivec2 finalImage;
    glm::ivec2 ui;
    glm::ivec2 hair;
};

struct DrawIndexedIndirectCommand {
    uint32_t indexCount = 0;
    uint32_t instanceCount = 0;
    uint32_t firstIndex = 0;
    int32_t  baseVertex = 0;
    uint32_t baseInstance = 0;
};

struct DrawArraysIndirectCommand {
    uint32_t vertexCount = 0;
    uint32_t instanceCount = 0;
    uint32_t firstVertex = 0;
    uint32_t baseInstance = 0;
};

struct DrawCommands {
    std::vector<DrawIndexedIndirectCommand> perViewport[4]; // One for each splitscreen player
};

struct DrawCommandsSet {
    DrawCommands geometry;
    DrawCommands geometryBlended;
    DrawCommands geometryAlphaDiscarded;
    DrawCommands hairTopLayer;
    DrawCommands hairBottomLayer;

    DrawCommands skinnedGeometry;
};

struct RendererSettings {
    int depthPeelCount = 4;
};


struct AABB {
    AABB() {}
    AABB(glm::vec3 min, glm::vec3 max) {
        boundsMin = min;
        boundsMax = max;
        CalculateCenter();
    }
    void Grow(AABB& b) {
        if (b.boundsMin.x != 1e30f && b.boundsMin.x != -1e30f) {
            Grow(b.boundsMin); Grow(b.boundsMax);
        }
        CalculateCenter();
    }
    void Grow(glm::vec3 p) {
        boundsMin = glm::vec3(std::min(boundsMin.x, p.x), std::min(boundsMin.y, p.y), std::min(boundsMin.z, p.z));
        boundsMax = glm::vec3(std::max(boundsMax.x, p.x), std::max(boundsMax.y, p.y), std::max(boundsMax.z, p.z));
        CalculateCenter();
    }
    float Area() {
        glm::vec3 e = boundsMax - boundsMin; // box extent
        return e.x * e.y + e.y * e.z + e.z * e.x;
    }
    const glm::vec3 GetCenter() {
        return center;
    }
    const glm::vec3 GetBoundsMin() {
        return boundsMin;
    }
    const glm::vec3 GetBoundsMax() {
        return boundsMax;
    }

public: // make private later
    glm::vec3 center = glm::vec3(0);
    glm::vec3 boundsMin = glm::vec3(1e30f);
    glm::vec3 boundsMax = glm::vec3(-1e30f);
    glm::vec3 padding = glm::vec3(0);

    void CalculateCenter() {
        center = { (boundsMin.x + boundsMax.x) / 2, (boundsMin.y + boundsMax.y) / 2, (boundsMin.z + boundsMax.z) / 2 };
    }

    bool ContainsPoint(glm::vec3 point) const {
        return (point.x >= boundsMin.x && point.x <= boundsMax.x) &&
            (point.y >= boundsMin.y && point.y <= boundsMax.y) &&
            (point.z >= boundsMin.z && point.z <= boundsMax.z);
    }
};

struct WaterState {
    float heightBeneathWater = 0;
    float heightAboveWater = 0;

    bool cameraUnderWater = false;
    bool feetUnderWater = false;
    bool wading = false;
    bool swimming = false;

    // Previous frame
    bool cameraUnderWaterPrevious = false;
    bool feetUnderWaterPrevious = false;
    bool wadingPrevious = true;
    bool swimmingPrevious = true;
};

struct PlayerControls {
    unsigned int WALK_FORWARD = HELL_KEY_W;
    unsigned int WALK_BACKWARD = HELL_KEY_S;
    unsigned int WALK_LEFT = HELL_KEY_A;
    unsigned int WALK_RIGHT = HELL_KEY_D;
    unsigned int INTERACT = HELL_KEY_E;
    unsigned int RELOAD = HELL_KEY_R;
    unsigned int FIRE = HELL_MOUSE_LEFT;
    unsigned int ADS = HELL_MOUSE_RIGHT;
    unsigned int JUMP = HELL_KEY_SPACE;
    unsigned int CROUCH = HELL_KEY_WIN_CONTROL; // different mapping to the standard glfw HELL_KEY_LEFT_CONTROL
    unsigned int NEXT_WEAPON = HELL_KEY_Q;
    unsigned int ESCAPE = HELL_KEY_WIN_ESCAPE;
    unsigned int DEBUG_FULLSCREEN = HELL_KEY_G;
    unsigned int DEBUG_ONE = HELL_KEY_1;
    unsigned int DEBUG_TWO = HELL_KEY_2;
    unsigned int DEBUG_THREE = HELL_KEY_3;
    unsigned int DEBUG_FOUR = HELL_KEY_4;
    unsigned int MELEE = HELL_KEY_V;
    unsigned int FLASHLIGHT = HELL_KEY_F;
    unsigned int MISC_WEAPON_FUNCTION = HELL_KEY_T;
};

struct Bullet {
    glm::vec3 spawnPosition;
    glm::vec3 direction;
    //Weapon type;
    uint32_t raycastFlags;
    glm::vec3 parentPlayersViewRotation;
    int damage = 0;
    int parentPlayerIndex = -1;
};

struct PhysicsFilterData {
    RaycastGroup raycastGroup = RaycastGroup::RAYCAST_DISABLED;
    CollisionGroup collisionGroup = CollisionGroup::NO_COLLISION;
    CollisionGroup collidesWith = CollisionGroup::ENVIROMENT_OBSTACLE;
};

struct GPULight {
    float posX;
    float posY;
    float posZ;
    float colorR;

    float colorG;
    float colorB;
    float strength;
    float radius;
};

struct SelectionRectangleState {
    int beginX = 0;
    int beginY = 0;
    int currentX = 0;
    int currentY = 0;
};

struct PhysXRayResult {
    std::string hitObjectName;
    glm::vec3 hitPosition;
    glm::vec3 surfaceNormal;
    glm::vec3 rayDirection;
    bool hitFound;
    void* hitActor;
    //void* parent;
    //ObjectType objectType;
};