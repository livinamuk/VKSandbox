#pragma once

#define UNDEFINED_STRING "UNDEFINED_STRING"
#define TIME_WRAP 10000.0f

#define NEAR_PLANE 0.0025f
#define FAR_PLANE 500.0f

#define ORANGE        glm::vec4(1.00f, 0.65f, 0.00f, 1.0f)
#define BLACK         glm::vec4(0.00f, 0.00f, 0.00f, 1.0f)
#define WHITE         glm::vec4(1.00f, 1.00f, 1.00f, 1.0f)
#define RED           glm::vec4(1.00f, 0.00f, 0.00f, 1.0f)
#define GREEN         glm::vec4(0.00f, 1.00f, 0.00f, 1.0f)
#define BLUE          glm::vec4(0.00f, 0.00f, 1.00f, 1.0f)
#define YELLOW        glm::vec4(1.00f, 1.00f, 0.00f, 1.0f)
#define PURPLE        glm::vec4(1.00f, 0.00f, 1.00f, 1.0f)
#define GREY          glm::vec4(0.25f, 0.25f, 0.25f, 1.0f)
#define LIGHT_BLUE    glm::vec4(0.00f, 1.00f, 1.00f, 1.0f)
#define LIGHT_GREEN   glm::vec4(0.16f, 0.78f, 0.23f, 1.0f)
#define LIGHT_RED     glm::vec4(0.80f, 0.05f, 0.05f, 1.0f)
#define TRANSPARENT   glm::vec4(0.00f, 0.00f, 0.00f, 0.0f)

#define HELL_PI 3.14159265358979323846
#define VIEWPORT_INDEX_SHIFT 20 // Only 2 bits needed for 4 players

// Render limits
#define TEXTURE_ARRAY_SIZE 1024
#define MAX_INSTANCE_DATA_COUNT 8192
#define MAX_INDIRECT_DRAW_COMMAND_COUNT 4096
#define MAX_ANIMATED_TRANSFORMS 2048 // is this needed?
#define MAX_VIEWPORT_COUNT 4
#define SHADOW_CASCADE_COUNT 4
#define MAX_GPU_LIGHTS 512



// System audio
#define AUDIO_SELECT "UI_Select.wav"

// Cursors
#define HELL_CURSOR_ARROW           0x00036001
#define HELL_CURSOR_IBEAM           0x00036002
#define HELL_CURSOR_CROSSHAIR       0x00036003
#define HELL_CURSOR_HAND            0x00036004
#define HELL_CURSOR_HRESIZE         0x00036005
#define HELL_CURSOR_VRESIZE         0x00036006

// Editor
#define EDITOR_FILE_MENU_HEIGHT 40.0f
#define EDITOR_LEFT_PANEL_WIDTH 400.0f



// Fix this shit

#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }
#define ToRadian(x) (float)(((x) * HELL_PI / 180.0f))
#define ToDegree(x) (float)(((x) * 180.0f / HELL_PI))

#define POSITION_LOCATION    0
#define NORMAL_LOCATION		 1
#define TEX_COORD_LOCATION   2
#define TANGENT_LOCATION     3
#define BONE_ID_LOCATION     4
#define BONE_WEIGHT_LOCATION 5

#define GLOCK_CLIP_SIZE 12
#define GLOCK_MAX_AMMO_SIZE 200
#define AKS74U_MAG_SIZE 30
#define AKS74U_MAX_AMMO_SIZE 9999
#define SHOTGUN_AMMO_SIZE 8
#define SHOTGUN_MAX_AMMO_SIZE 9999


#define FLASHLIGHT_SHADOWMAP_SIZE 2048

#define HEIGHT_MAP_SIZE 256
#define HEIGHTMAP_BEGIN_Y -5.0f
#define HEIGHTMAP_SCALE_Y 25.0f
#define HEIGHTMAP_SCALE_XZ 0.25f

#define PLAYER_CAPSULE_HEIGHT 0.4f
#define PLAYER_CAPSULE_RADIUS 0.15f

