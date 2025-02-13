#include "GL_renderer.h"

namespace OpenGLRenderer {

    void InitRasterizerStates() {
        OpenGLRasterizerState* geometryPassNonBlended = CreateRasterizerState("GeometryPass_NonBlended");
        geometryPassNonBlended->depthTestEnabled = true;
        geometryPassNonBlended->blendEnable = false;
        geometryPassNonBlended->cullfaceEnable = true;
        geometryPassNonBlended->depthMask = true;
        geometryPassNonBlended->depthFunc = GL_LESS;

        OpenGLRasterizerState* geometryPassBlended = CreateRasterizerState("GeometryPass_Blended");
        geometryPassBlended->depthTestEnabled = true;
        geometryPassBlended->blendEnable = true;
        geometryPassBlended->cullfaceEnable = false;
        geometryPassBlended->depthMask = false;
        geometryPassBlended->depthFunc = GL_LESS;
        geometryPassBlended->blendFuncSrcfactor = GL_SRC_ALPHA;
        geometryPassBlended->blendFuncDstfactor = GL_ONE_MINUS_SRC_ALPHA;

        OpenGLRasterizerState* hairPassViewspaceDepth = CreateRasterizerState("HairViewspaceDepth");
        hairPassViewspaceDepth->depthTestEnabled = true;
        hairPassViewspaceDepth->blendEnable = false;
        hairPassViewspaceDepth->cullfaceEnable = true;
        hairPassViewspaceDepth->depthMask = true;
        hairPassViewspaceDepth->depthFunc = GL_LESS;
        hairPassViewspaceDepth->blendFuncSrcfactor = GL_SRC_ALPHA;
        hairPassViewspaceDepth->blendFuncDstfactor = GL_ONE_MINUS_SRC_ALPHA;
        hairPassViewspaceDepth->pointSize = 8;

        OpenGLRasterizerState* hairPassLighting = CreateRasterizerState("HairLighting");
        hairPassLighting->depthTestEnabled = true;
        hairPassLighting->blendEnable = false;
        hairPassLighting->cullfaceEnable = true;
        hairPassLighting->depthMask = true;
        hairPassLighting->depthFunc = GL_EQUAL;
        hairPassLighting->blendFuncSrcfactor = GL_SRC_ALPHA;
        hairPassLighting->blendFuncDstfactor = GL_ONE_MINUS_SRC_ALPHA;
        hairPassLighting->pointSize = 8;

        OpenGLRasterizerState* spriteSheet = CreateRasterizerState("SpriteSheetPass");
        spriteSheet->depthTestEnabled = true;
        spriteSheet->blendEnable = true;
        spriteSheet->cullfaceEnable = false;
        spriteSheet->depthMask = false;
        spriteSheet->depthFunc = GL_LESS;
        spriteSheet->blendFuncSrcfactor = GL_SRC_ALPHA;
        spriteSheet->blendFuncDstfactor = GL_ONE_MINUS_SRC_ALPHA;

        OpenGLRasterizerState* skybox = CreateRasterizerState("SkyBox");
        skybox->depthTestEnabled = false;
        skybox->blendEnable = false;
        skybox->cullfaceEnable = false;
        skybox->depthMask = false;
        skybox->depthFunc = GL_LESS;
    }

}