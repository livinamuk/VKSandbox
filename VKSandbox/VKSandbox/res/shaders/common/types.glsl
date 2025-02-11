
struct ViewportData {
    mat4 projection;
    mat4 inverseProjection;
    mat4 view;
    mat4 inverseView;
    mat4 projectionView;
    mat4 inverseProjectionView;
    mat4 skyboxProjectionView;
    float clipSpaceXMin;
    float clipSpaceXMax;
    float clipSpaceYMin;
    float clipSpaceYMax;
};

struct RendererData {
    float nearPlane;
    float farPlane;
    float gBufferWidth;
    float gBufferHeight;
    float hairBufferWidth;
    float hairBufferHeight;
    int splitscreenMode;
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