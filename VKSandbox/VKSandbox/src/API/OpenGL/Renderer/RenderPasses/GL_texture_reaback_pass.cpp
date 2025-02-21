#include "API/OpenGL/Renderer/GL_renderer.h"

#include "API/OpenGL/Types/GL_texture_readback.h"
#include "BackEnd/BackEnd.h"
#include "Input/Input.h"
#include "Tools/ImageTools.h"
#include "Util/Util.h"

namespace OpenGLRenderer {

    static OpenGLTextureReadBack g_mouseRayReadBack;
    bool g_mouseRayWorldPositionReadBackReady = false;
    glm::vec3 g_mouseRayWorldPosition = glm::vec3(0, 1, 0);

    void TextureReadBackPass() {
        if (!g_mouseRayReadBack.IsRequestInProgress()) {
            OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
            GLuint fboHandle = gBuffer->GetHandle();
            GLuint attachment = gBuffer->GetColorAttachmentSlotByName("WorldSpacePosition");
            int mappedMouseX = Util::MapRange(Input::GetMouseX(), 0, BackEnd::GetCurrentWindowWidth(), 0, gBuffer->GetWidth());
            int mappedMouseY = Util::MapRange(Input::GetMouseY(), 0, BackEnd::GetCurrentWindowHeight(), gBuffer->GetHeight(), 0);
            int xOffset = mappedMouseX;
            int yOffset = mappedMouseY;
            int width = 1;
            int height = 1;
            g_mouseRayReadBack.IssueDataRequest(fboHandle, attachment, xOffset, yOffset, width, height, GL_RGBA32F);
        }
        if (g_mouseRayReadBack.IsRequestInProgress()) {
            g_mouseRayReadBack.Update();
        }
        if (g_mouseRayReadBack.IsResultReady()) {
            g_mouseRayWorldPosition = g_mouseRayReadBack.GetFloatPixel(0);
            g_mouseRayWorldPositionReadBackReady = true;
            g_mouseRayReadBack.Reset();
        }
    }

    //void SaveHeightMap() {
    //    static OpenGLTextureReadBack readback;
    //    static bool savedFile = false;
    //    if (Input::KeyPressed(HELL_KEY_E)) {
    //        OpenGLFrameBuffer* heightmapFBO = GetFrameBuffer("HeightMap");
    //        GLuint fboHandle = heightmapFBO->GetHandle();
    //        GLuint attachment = heightmapFBO->GetColorAttachmentSlotByName("Color");
    //        int xOffset = 0;
    //        int yOffset = 0;
    //        int width = heightmapFBO->GetWidth();
    //        int height = heightmapFBO->GetHeight();
    //        readback.IssueDataRequest(fboHandle, attachment, xOffset, yOffset, width, height, GL_R16F);
    //        savedFile = false;
    //    }
    //    if (readback.IsRequestInProgress()) {
    //        readback.Update();
    //    }
    //    if (readback.IsResultReady() && !savedFile) {
    //        ImageTools::SaveFloatArrayTextureAsBitmap(readback.GetFloatData(), readback.GetWidth(), readback.GetHeight(), GL_R16F, "heightmap_export.bmp");
    //        std::cout << "saved r8 file\n";
    //        readback.Reset();
    //    }
    //}


    bool IsMouseRayWorldPositionReadBackReady() {
        return g_mouseRayWorldPositionReadBackReady;
    }
    
    glm::vec3 GetMouseRayWorldPostion() {
        return g_mouseRayWorldPosition;
    }
}