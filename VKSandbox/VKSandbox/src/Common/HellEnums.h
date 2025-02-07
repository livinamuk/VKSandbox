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
    CENTERED_HORIZONTAL,
    CENTERED_VERTICAL,
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

enum class ShadingMode {
    SHADED,
    WIREFRAME,
    WIREFRAME_OVERLAY,
    SHADING_MODE_COUNT
};

enum class CameraView {
    PERSPECTIVE,
    ORTHO,
    FRONT,
    BACK,
    RIGHT,
    LEFT,
    TOP,
    BOTTOM,
    UNDEFINED
};

enum struct ViewportResizeState {
    IDLE,
    RESIZING_HORIZONTAL,
    RESIZING_VERTICAL,
    RESIZING_HORIZONTAL_VERTICAL,
};

enum WeaponAction {
    IDLE = 0,
    FIRE,
    RELOAD,
    RELOAD_FROM_EMPTY,
    DRAW_BEGIN,
    DRAWING,
    SPAWNING,
    RELOAD_SHOTGUN_BEGIN,
    RELOAD_SHOTGUN_SINGLE_SHELL,
    RELOAD_SHOTGUN_DOUBLE_SHELL,
    RELOAD_SHOTGUN_END,
    RELOAD_REVOLVER_BEGIN,
    RELOAD_REVOLVER_LOOP,
    RELOAD_REVOLVER_END,
    ADS_IN,
    ADS_OUT,
    ADS_IDLE,
    ADS_FIRE,
    MELEE
};

enum class ShellEjectionState {
    IDLE, AWAITING_SHELL_EJECTION
};

enum InputType {
    KEYBOARD_AND_MOUSE,
    CONTROLLER
};

// Fix me

enum VB_TYPES {
    INDEX_BUFFER,
    POS_VB,
    NORMAL_VB,
    TEXCOORD_VB,
    TANGENT_VB,
    BITANGENT_VB,
    BONE_VB,
    SMOOTH_NORMAL_VB,
    NUM_VBs
};

enum Weapon { KNIFE = 0, GLOCK, SHOTGUN, AKS74U, MP7, WEAPON_COUNT };