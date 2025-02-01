#include "GL_cubemapView.h"
#include <iostream>

OpenGLCubemapView::OpenGLCubemapView(const std::vector<GLuint>& tex2D) {
    CreateCubemap(tex2D);
}

void OpenGLCubemapView::CreateCubemap(const std::vector<GLuint>& tex2D) {
    if (tex2D.size() != 6) {
        std::cout << "Cubemap requires exactly 6 textures.\n";
        return;
    }

    GLuint texArray;
    glGenTextures(1, &texArray);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texArray);

    GLint width, height, format;
    glBindTexture(GL_TEXTURE_2D, tex2D[0]);  // Query properties from the first texture
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &format);

    // Allocate storage for the texture array
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, format, width, height, 6);

    // Copy each texture into the array
    for (int i = 0; i < 6; ++i) {
        glCopyImageSubData(tex2D[i], GL_TEXTURE_2D, 0, 0, 0, 0,
            texArray, GL_TEXTURE_2D_ARRAY, 0, 0, 0, i,
            width, height, 1);
    }

    // Now create a cubemap view from the 2D array
    glGenTextures(1, &m_handle);
    glTextureView(m_handle, GL_TEXTURE_CUBE_MAP, texArray, format, 0, 1, 0, 6);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}


GLuint OpenGLCubemapView::GetHandle() const {
    return m_handle;
}