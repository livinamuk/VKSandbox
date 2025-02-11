#pragma once
#include <cstdint>

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
    LIGHT,
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
    DRAWING_WITH_SHOTGUN_PUMP,
    SPAWNING,
    RELOAD_SHOTGUN_BEGIN,
    RELOAD_SHOTGUN_SINGLE_SHELL,
    RELOAD_SHOTGUN_DOUBLE_SHELL,
    RELOAD_SHOTGUN_END,
    RELOAD_SHOTGUN_END_WITH_PUMP,
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

enum CollisionGroup : uint64_t {
    NO_COLLISION = 0,
    BULLET_CASING = 1,
    PLAYER = 2,
    ENVIROMENT_OBSTACLE = 4,
    GENERIC_BOUNCEABLE = 8,
    ITEM_PICK_UP = 16,
    RAGDOLL = 32,
    DOG_CHARACTER_CONTROLLER = 64,
    GENERTIC_INTERACTBLE = 128,
    ENVIROMENT_OBSTACLE_NO_DOG = 256,
    SHARK = 512,
    LADDER = 1024,
};



// Re-evaluate how this works, coz it alway fucks you up, 
// and PhysX this group bitmask is used for more than just raycasts, pretty sure
enum RaycastGroup {
    RAYCAST_DISABLED = 0,
    RAYCAST_ENABLED = 1,
    PLAYER_1_RAGDOLL = 2,
    PLAYER_2_RAGDOLL = 4,
    PLAYER_3_RAGDOLL = 8,
    PLAYER_4_RAGDOLL = 16,
    DOBERMAN = 32
};

enum DebugLineRenderMode {
    SHOW_NO_LINES,
    PATHFINDING_RECAST,
    PHYSX_ALL,
    PHYSX_RAYCAST,
    PHYSX_COLLISION,
    RAYTRACE_LAND,
    PHYSX_EDITOR,
    BOUNDING_BOXES,
    RTX_LAND_AABBS,
    RTX_LAND_TRIS,
    RTX_LAND_TOP_LEVEL_ACCELERATION_STRUCTURE,
    RTX_LAND_BOTTOM_LEVEL_ACCELERATION_STRUCTURES,
    RTX_LAND_TOP_AND_BOTTOM_LEVEL_ACCELERATION_STRUCTURES,
    DEBUG_LINE_MODE_COUNT,
};

enum struct LightType {
    LAMP_POST = 0
};