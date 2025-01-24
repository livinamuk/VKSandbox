#pragma once
#include "GL_backend.h"
#include "../AssetManagement/BakeQueue.h"
#include "../AssetManagement/AssetManager.h"
#include <string>
#include <iostream>
#include <vector>
#include "HellTypes.h"
#include "GL_util.hpp"
#include "../BackEnd/GLFWIntegration.h"
#include "Types/GL_pbo.hpp"

namespace OpenGLBackEnd {

    // PBO texture loading
    const size_t MAX_TEXTURE_WIDTH = 4096;
    const size_t MAX_TEXTURE_HEIGHT = 4096;
    const size_t MAX_CHANNEL_COUNT = 4;
    const size_t MAX_DATA_SIZE = MAX_TEXTURE_WIDTH * MAX_TEXTURE_HEIGHT * MAX_CHANNEL_COUNT;
    std::vector<PBO> g_textureBakingPBOs;
    PBO g_mousePickPBO;
    GLuint g_frameBufferHandle = 0;
    GLuint g_mousePickAttachmentSlot = 0;
    uint8_t g_mousePickR = 0;
    uint8_t g_mousePickG = 0;
    uint8_t g_mousePickB = 0;
    uint8_t g_mousePickA = 0;

    void Init() {

        GLFWIntegration::MakeContextCurrent();

        // Init glad
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD\n";
            return;
        }
        // Print some shit
        GLint major, minor;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);
        const GLubyte* vendor = glGetString(GL_VENDOR);
        const GLubyte* renderer = glGetString(GL_RENDERER);
        std::cout << "\nGPU: " << renderer << "\n";
        std::cout << "GL version: " << major << "." << minor << "\n\n";

        // Setup debug context
        //int flags;
        //glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        //if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        //    //std::cout << "Debug GL context enabled\n";
        //    glEnable(GL_DEBUG_OUTPUT);
        //    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // makes sure errors are displayed synchronously
        //    glDebugMessageCallback(glDebugOutput, nullptr);
        //}
        //else {
        //    std::cout << "Debug GL context not available\n";
        //}

        // Clear screen to black
        glClear(GL_COLOR_BUFFER_BIT);

        // Match vulkan matrix shit
        glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);

        // Texture baking PBOs
        for (int i = 0; i < 16; ++i) {
            PBO& pbo = g_textureBakingPBOs.emplace_back();
            pbo.Init(MAX_DATA_SIZE);
        }

        g_mousePickPBO.Init(2 * sizeof(uint16_t));

    }

    void SetMousePickHandles(GLuint frameBufferHandle, GLuint attachmentSlot) {
        g_frameBufferHandle = frameBufferHandle;
        g_mousePickAttachmentSlot = attachmentSlot;
    }

    void UpdateMousePicking(GLint x, GLint y) {
        g_mousePickPBO.UpdateState();        
        if (!g_mousePickPBO.IsSyncComplete()) {
            //std::cout << "Waiting for PBO sync: " << g_mousePickPBO.GetSyncStatusAsString() << "\n";
            return;
        }
        // Access the data in the PBO
        const uint16_t* mappedBuffer = reinterpret_cast<const uint16_t*>(g_mousePickPBO.GetPersistentBuffer());
        if (mappedBuffer) {
            g_mousePickR = mappedBuffer[0];
            g_mousePickG = mappedBuffer[1];
        }
        if (!g_mousePickPBO.IsSyncInProgress()) {
            glBindBuffer(GL_PIXEL_PACK_BUFFER, g_mousePickPBO.GetHandle());
            glBindFramebuffer(GL_FRAMEBUFFER, g_frameBufferHandle);
            glReadBuffer(g_mousePickAttachmentSlot);
            glReadPixels(x, y, 1, 1, GL_RG_INTEGER, GL_UNSIGNED_SHORT, nullptr);  // RG UINT
            //glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);       // RGBA
            glBindTexture(GL_TEXTURE_2D, 0);
            g_mousePickPBO.SyncStart();
        }
    }

    int GetMousePickR() {
        return g_mousePickR;
    }
    int GetMousePickG() {
        return g_mousePickG;
    }
    int GetMousePickB() {
        return g_mousePickB;
    }
    int GetMousePickA() {
        return g_mousePickA;
    }

    void AllocateTextureMemory(Texture& texture) {
        OpenGLTexture& glTexture = texture.GetGLTexture();
        GLuint& handle = glTexture.GetHandle();
        if (handle != 0) {
            return; // Perhaps handle this better, or be more descriptive in function name!
        }
        glGenTextures(1, &handle);
        glBindTexture(GL_TEXTURE_2D, handle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, OpenGLUtil::TextureWrapModeToGLEnum(texture.GetTextureWrapMode()));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, OpenGLUtil::TextureWrapModeToGLEnum(texture.GetTextureWrapMode()));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, OpenGLUtil::TextureFilterToGLEnum(texture.GetMinFilter()));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, OpenGLUtil::TextureFilterToGLEnum(texture.GetMagFilter()));
        int mipmapWidth = texture.GetWidth(0);
        int mipmapHeight = texture.GetHeight(0);
        int levelCount = texture.MipmapsAreRequested() ? texture.GetMipmapLevelCount() : 1;
        for (int i = 0; i < levelCount; i++) {
            if (texture.GetImageDataType() == ImageDataType::UNCOMPRESSED) {
                glTexImage2D(GL_TEXTURE_2D, i, texture.GetInternalFormat(), mipmapWidth, mipmapHeight, 0, texture.GetFormat(), GL_UNSIGNED_BYTE, nullptr);
            }
            if (texture.GetImageDataType() == ImageDataType::COMPRESSED) {
                glCompressedTexImage2D(GL_TEXTURE_2D, i, texture.GetInternalFormat(), mipmapWidth, mipmapHeight, 0, texture.GetDataSize(i), nullptr);
            }
            if (texture.GetImageDataType() == ImageDataType::EXR) {
                // TODO! glTexImage2D(GL_TEXTURE_2D, i, GL_RGB16, mipmapWidth, mipmapHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
            }
            mipmapWidth = std::max(1, mipmapWidth / 2);
            mipmapHeight = std::max(1, mipmapHeight / 2);
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void ImmediateBake(QueuedTextureBake& queuedTextureBake) {
        Texture* texture = static_cast<Texture*>(queuedTextureBake.texture);
        OpenGLTexture& glTexture = texture->GetGLTexture();
        int width = queuedTextureBake.width;
        int height = queuedTextureBake.height;
        int format = queuedTextureBake.format;
        int internalFormat = queuedTextureBake.internalFormat;
        int level = queuedTextureBake.mipmapLevel;
        int dataSize = queuedTextureBake.dataSize;
        const void* data = queuedTextureBake.data;

        // Bake texture data
        glBindTexture(GL_TEXTURE_2D, glTexture.GetHandle());
        if (texture->GetImageDataType() == ImageDataType::UNCOMPRESSED) {
            glTexImage2D(GL_TEXTURE_2D, level, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        }
        else if (texture->GetImageDataType() == ImageDataType::EXR) {
            //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16, glTexture.GetWidth(), glTexture.GetHeight(), 0, GL_RGBA, GL_FLOAT, glTexture.GetData());
        }
        else if (texture->GetImageDataType() == ImageDataType::COMPRESSED) {
            glCompressedTexSubImage2D(GL_TEXTURE_2D, level, 0, 0, width, height, internalFormat, dataSize, data);
        }
        texture->SetTextureDataLevelBakeState(level, BakeState::BAKE_COMPLETE);

        // Generate Mipmaps if none were supplied
        if (texture->MipmapsAreRequested()) {
            if (texture->GetTextureDataCount() == 1) {
                glGenerateMipmap(GL_TEXTURE_2D);
            }
        }
        // Cleanup
        glBindTexture(GL_TEXTURE_2D, 0);
        BakeQueue::RemoveQueuedTextureBakeByJobID(queuedTextureBake.jobID);
    }

    void UpdateTextureBaking() {
        int bakeCommandsIssuedPerFrame = g_textureBakingPBOs.size();
        for (int i = 0; i < bakeCommandsIssuedPerFrame; i++) {
            // Update pbo states
            for (PBO& pbo : g_textureBakingPBOs) {
                pbo.UpdateState();
            }
            // If any have completed, remove the job ID from the queue
            for (PBO& pbo : g_textureBakingPBOs) {
                uint32_t jobID = pbo.GetCustomValue();
                if (pbo.IsSyncComplete() && jobID != -1) {
                    QueuedTextureBake* queuedTextureBake = BakeQueue::GetQueuedTextureBakeByJobID(jobID);
                    Texture* texture = static_cast<Texture*>(queuedTextureBake->texture);
                    texture->SetTextureDataLevelBakeState(queuedTextureBake->mipmapLevel, BakeState::BAKE_COMPLETE);
                    // Generate Mipmaps if none were supplied
                    if (texture->MipmapsAreRequested()) {
                        if (texture->GetTextureDataCount() == 1) {
                            glBindTexture(GL_TEXTURE_2D, texture->GetGLTexture().GetHandle());
                            glGenerateMipmap(GL_TEXTURE_2D); 
                            glBindTexture(GL_TEXTURE_2D, 0);
                        }
                    }
                    BakeQueue::RemoveQueuedTextureBakeByJobID(jobID);
                    pbo.SetCustomValue(-1);
                }
            }
            // Bake next queued bake (should one exist)
            if (BakeQueue::GetQueuedTextureBakeJobCount() > 0) {
                QueuedTextureBake* queuedTextureBake = BakeQueue::GetNextQueuedTextureBake();
                if (queuedTextureBake) {
                    AsyncBakeQueuedTextureBake(*queuedTextureBake);
                }
            }
        }
    }

    void AsyncBakeQueuedTextureBake(QueuedTextureBake& queuedTextureBake) {
        // Get next free PBO
        PBO* pbo = nullptr;
        for (PBO& queryPbo : g_textureBakingPBOs) {
            if (queryPbo.IsSyncComplete()) {
                pbo = &queryPbo;
                break;
            }
        }
        // Return early if no free pbos
        if (!pbo) {
            //std::cout << "OpenGLBackend::UploadTexture() return early because there were no pbos avaliable!\n";
            return;
        }
        queuedTextureBake.inProgress = true;

        Texture* texture = static_cast<Texture*>(queuedTextureBake.texture);
        int jobID = queuedTextureBake.jobID;
        int width = queuedTextureBake.width;
        int height = queuedTextureBake.height;
        int format = queuedTextureBake.format;
        int internalFormat = queuedTextureBake.internalFormat;
        int level = queuedTextureBake.mipmapLevel;
        int dataSize = queuedTextureBake.dataSize;
        const void* data = queuedTextureBake.data;

        texture->SetTextureDataLevelBakeState(level, BakeState::BAKING_IN_PROGRESS);
        
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo->GetHandle());
        std::memcpy(pbo->GetPersistentBuffer(), data, dataSize);
        glBindTexture(GL_TEXTURE_2D, texture->GetGLTexture().GetHandle());
        
        if (texture->GetImageDataType() == ImageDataType::UNCOMPRESSED) {
            glTexSubImage2D(GL_TEXTURE_2D, level, 0, 0, width, height, format, GL_UNSIGNED_BYTE, nullptr);
        }
        if (texture->GetImageDataType() == ImageDataType::COMPRESSED) {
            glCompressedTexSubImage2D(GL_TEXTURE_2D, level, 0, 0, width, height, internalFormat, dataSize, nullptr);
        }
        if (texture->GetImageDataType() == ImageDataType::EXR) {
            //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, glTexture.GetWidth(), glTexture.GetHeight(), glTexture.GetFormat(), GL_FLOAT, nullptr);
        }
        pbo->SyncStart();
        pbo->SetCustomValue(jobID);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    }


    void CleanUpBakingPBOs() {
        for (PBO& pbo : g_textureBakingPBOs) {
            pbo.CleanUp();
        }
        g_textureBakingPBOs.clear();
    }
}