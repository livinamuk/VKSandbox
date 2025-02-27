
struct ViewportData {
    mat4 projection;
    mat4 inverseProjection;
    mat4 view;
    mat4 inverseView;
    mat4 projectionView;
    mat4 inverseProjectionView;
    mat4 skyboxProjectionView;
    mat4 flashlightProjectionView;
    int xOffset;
    int yOffset;
    int width;
    int height;
    float posX;  // 0 t0 1 range
    float posY;  // 0 t0 1 range
    float sizeX; // 0 t0 1 range
    float sizeY; // 0 t0 1 range
    vec4 frustumPlane0;
    vec4 frustumPlane1;
    vec4 frustumPlane2;
    vec4 frustumPlane3;
    vec4 frustumPlane4;
    vec4 frustumPlane5;
    vec4 flashlightDir;
    vec4 flashlightPosition;
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
    float time;
    int splitscreenMode;    
    int rendererOverrideState;
};

struct RenderItem {
    mat4 modelMatrix;
    mat4 inverseModelMatrix;
    vec4 aabbMin;
    vec4 aabbMax;

    int meshIndex;
    int baseColorTextureIndex;
    int normalMapTextureIndex;
    int rmaTextureIndex;

    int mousePickType;
    int mousePickIndex;
    int baseSkinnedVertex;
    int ignoredViewportIndex;

    int exclusiveViewportIndex;
    int skinned;     // true or false
    float emissiveR;    
    float emissiveG;

    float emissiveB;
    float padding0;
    float padding1;
    float padding2;
};

struct Light {
    float posX;
    float posY;
    float posZ;
    float colorR;

    float colorG;
    float colorB;
    float strength;
    float radius;
};