#pragma once
#include "HellTypes.h"
#include "GL_pbo.hpp"

struct OpenGLTextureReadBackRGBA32F {
    void IssueDataRequest(GLuint framebufferHandle, GLuint textureHandle, int offsetX, int offsetY, int width, int height);
    void Update();

    int GetWidth()                  { return m_width; }
    int GetHeight()                 { return m_height; }
    bool IsResultReady()            { return m_resultReady; }
    bool IsRequestInProgress()      { return m_requestInProgress; }
    std::vector<float>& GetData()   { return m_data; }

private:
    GLuint m_framebufferHandle = 0;
    GLuint m_attachmentSlot = 0;
    int m_offsetX = 0;
    int m_offsetY = 0;
    int m_width = 0;
    int m_height = 0;
    bool m_resultReady = false;
    bool m_requestInProgress = false;
    size_t m_bufferSize = 0;
    PBO m_pbo;
    std::vector<float> m_data;
};

struct OpenGLTextureReadBackR8 {
    void IssueDataRequest(GLuint framebufferHandle, GLuint textureHandle, int offsetX, int offsetY, int width, int height);
    void Update();

    int GetWidth() { return m_width; }
    int GetHeight() { return m_height; }
    bool IsResultReady() { return m_resultReady; }
    bool IsRequestInProgress() { return m_requestInProgress; }
    std::vector<float>& GetData() { return m_data; }

private:
    GLuint m_framebufferHandle = 0;
    GLuint m_attachmentSlot = 0;
    int m_offsetX = 0;
    int m_offsetY = 0;
    int m_width = 0;
    int m_height = 0;
    bool m_resultReady = false;
    bool m_requestInProgress = false;
    size_t m_bufferSize = 0;
    PBO m_pbo;
    std::vector<float> m_data;
};