#pragma once

#define NEAR_PLANE   0.01f
#define FAR_PLANE  500.00f

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
#define PLAYER_INDEX_SHIFT 20 // Only 2 bits needed for 4 players

// Render limits
#define TEXTURE_ARRAY_SIZE 1024
#define MAX_INSTANCE_DATA_COUNT 8192
#define MAX_INDIRECT_DRAW_COMMAND_COUNT 4096

// System audio
#define AUDIO_SELECT "UI_Select.wav"

// Cursors
#define HELL_CURSOR_ARROW           0x00036001
#define HELL_CURSOR_IBEAM           0x00036002
#define HELL_CURSOR_CROSSHAIR       0x00036003
#define HELL_CURSOR_HAND            0x00036004
#define HELL_CURSOR_HRESIZE         0x00036005
#define HELL_CURSOR_VRESIZE         0x00036006