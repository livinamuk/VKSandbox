#include "GL_texture_readback.h"

/*

    *****************

      Example usage 
    
    ***************** (WARNING: still not async becoz pbo uses glBuffer usage instead of glBufferStorage... potentially stalling glReadPixels
    
    if (false) {
            static OpenGLTextureReadBackRGBA32F readback;
            static bool savedFile = false;
            if (Input::KeyPressed(HELL_KEY_SPACE)) {
                GLuint fboHandle = gBuffer.GetHandle();
                GLuint attachment = gBuffer.GetColorAttachmentSlotByName("WorldSpacePosition");
                int xOffset = 0;
                int yOffset = 0;
                int width = gBuffer.GetWidth();
                int height = gBuffer.GetHeight();
                readback.IssueDataRequest(fboHandle, attachment, xOffset, yOffset, width, height);
                savedFile = false;
            }
            if (readback.IsRequestInProgress()) {
                readback.Update();
            }
            if (readback.IsResultReady() && !savedFile) {
                ImageTools::SaveFloatArrayTextureAsBitmap(readback.GetData(), readback.GetWidth(), readback.GetHeight(), GL_RGB32F, "testcunt.bmp");
                std::cout << "saved rgba32f file\n";
                savedFile = true;
            }
        }
        else {
            static OpenGLTextureReadBackR8 readback;
            static bool savedFile = false;
            if (Input::KeyPressed(HELL_KEY_SPACE)) {
                GLuint fboHandle = g_frameBuffers["HeightMap"].GetHandle();
                GLuint attachment = g_frameBuffers["HeightMap"].GetColorAttachmentSlotByName("Color");
                int xOffset = 0;
                int yOffset = 0;
                int width = g_frameBuffers["HeightMap"].GetWidth();
                int height = g_frameBuffers["HeightMap"].GetHeight();
                readback.IssueDataRequest(fboHandle, attachment, xOffset, yOffset, width, height);
                savedFile = false;
            }
            if (readback.IsRequestInProgress()) {
                readback.Update();
            }
            if (readback.IsResultReady() && !savedFile) {
                ImageTools::SaveFloatArrayTextureAsBitmap(readback.GetData(), readback.GetWidth(), readback.GetHeight(), GL_R8, "testcunt.bmp");
                std::cout << "saved r8 file\n";
            }
        }

*/

void OpenGLTextureReadBackRGBA32F::IssueDataRequest(GLuint framebufferHandle, GLuint attachmentSlot, int offsetX, int offsetY, int width, int height) {
    m_framebufferHandle = framebufferHandle;
    m_attachmentSlot = attachmentSlot;
    m_offsetX = offsetX;
    m_offsetY = offsetY;
    m_width = width;
    m_height = height;
    m_data.resize(m_width * m_height * 4);
    m_bufferSize = m_width * m_height * sizeof(float) * 4;
    m_resultReady = false;
    m_requestInProgress = false;
    Update();
}

void OpenGLTextureReadBackRGBA32F::Update() {
    std::cout << "updating readback...\n";
    // Job done? then leave
    if (m_resultReady) {
        return;
    }

    // Create/recreate the pbo if it doesn't exist or requested data size exceeds allocated pbo buffer size
    if (m_pbo.GetHandle() == 0 || m_bufferSize > m_pbo.GetAllocatedBufferSize()) {
        m_pbo.CleanUp();
        m_pbo.Init(m_bufferSize);
    }

    m_pbo.UpdateState();

    // Still waiting for PBO sync? then return early
    if (!m_pbo.IsSyncComplete()) {
        return;
    }

    // PBO work done? The access/store the data in the PBO
    if (m_requestInProgress && m_pbo.IsSyncComplete()) {
        const float* mappedBuffer = reinterpret_cast<const float*>(m_pbo.GetPersistentBuffer());
        if (mappedBuffer) {
            m_data.assign(mappedBuffer, mappedBuffer + (m_width * m_height * 4));
            m_resultReady = true;
            m_requestInProgress = false;
            std::cout << "request complete\n";
            return;
        }
    }

    // If you made it this far, then the job is not done, and the pbo is available for work, so issue the work
    if (!m_pbo.IsSyncInProgress()) {
        glFinish();
        glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pbo.GetHandle());
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferHandle);
        glReadBuffer(m_attachmentSlot);
        glReadPixels(m_offsetX, m_offsetY, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr);
        std::cout << "sync started\n";
        m_pbo.SyncStart(); 
        m_pbo.UpdateState();
        m_requestInProgress = true;
    }
}

void OpenGLTextureReadBackR8::IssueDataRequest(GLuint framebufferHandle, GLuint attachmentSlot, int offsetX, int offsetY, int width, int height) {
    m_framebufferHandle = framebufferHandle;
    m_attachmentSlot = attachmentSlot;
    m_offsetX = offsetX;
    m_offsetY = offsetY;
    m_width = width;
    m_height = height;
    m_data.resize(m_width * m_height);
    m_bufferSize = m_width * m_height * sizeof(uint8_t); 
    m_resultReady = false;
    m_requestInProgress = false;
    Update();
}

void OpenGLTextureReadBackR8::Update() {
    // Job done? then leave
    if (m_resultReady) {
        return;
    }

    // Create/recreate the pbo if it doesn't exist or requested data size exceeds allocated pbo buffer size
    if (m_pbo.GetHandle() == 0 || m_bufferSize > m_pbo.GetAllocatedBufferSize()) {
        m_pbo.CleanUp();
        m_pbo.Init(m_bufferSize);
    }

    m_pbo.UpdateState();

    // Still waiting for PBO sync? then return early
    if (!m_pbo.IsSyncComplete()) {
        return;
    }

    // PBO work done? The access/store the data in the PBO
    if (m_requestInProgress && m_pbo.IsSyncComplete()) {
        const uint8_t* mappedBuffer = reinterpret_cast<const uint8_t*>(m_pbo.GetPersistentBuffer());
        if (mappedBuffer) {
            m_data.assign(mappedBuffer, mappedBuffer + (m_width * m_height)); // 1 byte per pixel
            m_resultReady = true;
            m_requestInProgress = false;
            return;
        }
    }

    // If you made it this far, then the job is not done, and the pbo is available for work, so issue the work
    if (!m_pbo.IsSyncInProgress()) {
        glFinish();
        glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pbo.GetHandle());
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferHandle);
        glReadBuffer(m_attachmentSlot); 
        glReadPixels(m_offsetX, m_offsetY, m_width, m_height, GL_RED, GL_UNSIGNED_BYTE, nullptr);
        m_pbo.SyncStart();
        m_pbo.UpdateState();
        m_requestInProgress = true;
    }
}