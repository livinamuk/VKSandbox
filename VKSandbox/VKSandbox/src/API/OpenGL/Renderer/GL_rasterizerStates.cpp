#include "GL_renderer.h"

namespace OpenGLRenderer {

    void InitRasterizerStates() {
        OpenGLRasterizerState* geometryPassNonBlended = CreateRasterizerState("GeometryPass_NonBlended");
        geometryPassNonBlended->depthTestEnabled = true;
        geometryPassNonBlended->stencilEnable = false;
        geometryPassNonBlended->blendEnable = false;
        geometryPassNonBlended->cullfaceEnable = true;
        geometryPassNonBlended->depthMask = true;
        geometryPassNonBlended->depthFunc = GL_LESS;
        geometryPassNonBlended->blendFuncSrcfactor = GL_SRC_ALPHA;
        geometryPassNonBlended->blendFuncDstfactor = GL_ONE_MINUS_SRC_ALPHA;
        geometryPassNonBlended->pointSize = 8;

        OpenGLRasterizerState* geometryPassBlended = CreateRasterizerState("GeometryPass_Blended");
        geometryPassBlended->depthTestEnabled = true;
        geometryPassBlended->blendEnable = true;
        geometryPassBlended->stencilEnable = false;
        geometryPassBlended->cullfaceEnable = false;
        geometryPassBlended->depthMask = false;
        geometryPassBlended->depthFunc = GL_LESS;
        geometryPassBlended->blendFuncSrcfactor = GL_SRC_ALPHA;
        geometryPassBlended->blendFuncDstfactor = GL_ONE_MINUS_SRC_ALPHA;
        geometryPassBlended->pointSize = 8;

        OpenGLRasterizerState* hairPassViewspaceDepth = CreateRasterizerState("HairViewspaceDepth");
        hairPassViewspaceDepth->depthTestEnabled = true;
        hairPassViewspaceDepth->blendEnable = false;
        hairPassViewspaceDepth->stencilEnable = false;
        hairPassViewspaceDepth->cullfaceEnable = true;
        hairPassViewspaceDepth->depthMask = true;
        hairPassViewspaceDepth->depthFunc = GL_LESS;
        hairPassViewspaceDepth->blendFuncSrcfactor = GL_SRC_ALPHA;
        hairPassViewspaceDepth->blendFuncDstfactor = GL_ONE_MINUS_SRC_ALPHA;
        hairPassViewspaceDepth->pointSize = 8;

        OpenGLRasterizerState* hairPassLighting = CreateRasterizerState("HairLighting");
        hairPassLighting->depthTestEnabled = true;
        hairPassLighting->stencilEnable = false;
        hairPassLighting->blendEnable = false;
        hairPassLighting->cullfaceEnable = true;
        hairPassLighting->depthMask = true;
        hairPassLighting->depthFunc = GL_EQUAL;
        hairPassLighting->blendFuncSrcfactor = GL_SRC_ALPHA;
        hairPassLighting->blendFuncDstfactor = GL_ONE_MINUS_SRC_ALPHA;
        hairPassLighting->pointSize = 8;
    }

}