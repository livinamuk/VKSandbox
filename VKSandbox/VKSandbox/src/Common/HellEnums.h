#pragma once

enum class API {
    OPENGL,
    VULKAN,
    UNDEFINED
};

enum class WindowedMode { 
    WINDOWED, 
    FULLSCREEN 
};

enum Alignment {
    CENTERED,
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT
};

enum class LoadingState {
    AWAITING_LOADING_FROM_DISK,
    LOADING_FROM_DISK,
    LOADING_COMPLETE
};

enum class BakeState {
    AWAITING_BAKE,
    BAKING_IN_PROGRESS,
    BAKE_COMPLETE,
    UNDEFINED
};

enum class BlendingMode { 
    NONE, 
    BLENDED,
    ALPHA_DISCARDED,
    HAIR_UNDER_LAYER,
    HAIR_TOP_LAYER,
    DO_NOT_RENDER 
};

enum class ImageDataType {
    UNCOMPRESSED,
    COMPRESSED,
    EXR,
    UNDEFINED
};

enum class TextureWrapMode {
    REPEAT,
    MIRRORED_REPEAT,
    CLAMP_TO_EDGE,
    CLAMP_TO_BORDER,
    UNDEFINED
};

enum class TextureFilter {
    NEAREST,
    LINEAR,
    LINEAR_MIPMAP,
    UNDEFINED
};

enum class EditorObjectType {
    NONE = 0,
    GAME_OBJECT,
    DOOR,
    WINDOW,
    CSG
}; 

enum class Axis {
    X,
    Y,
    Z,
    NONE,
};

enum class SplitscreenMode { 
    FULLSCREEN, 
    TWO_PLAYER, 
    FOUR_PLAYER, 
    SPLITSCREEN_MODE_COUNT 
};