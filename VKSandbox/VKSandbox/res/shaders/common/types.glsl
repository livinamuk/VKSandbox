
struct PlayerData {
    mat4 projection;
    mat4 inverseProjection;
    mat4 view;
    mat4 inverseView;
    mat4 projectionView;
    mat4 inverseProjectionView;
    mat4 skyboxModelMatrix;
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