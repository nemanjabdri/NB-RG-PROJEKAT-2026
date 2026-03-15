//
// Created by nemanja on 4.2.26..
//

#include <engine/graphics/Framebuffer.hpp>
#include <engine/util/Errors.hpp>
#include <glad/glad.h>
#include <cstddef>

namespace engine {
    graphics::Framebuffer::Framebuffer(int width, int height) : m_width(width)
                                                            , m_height(height) {
        create_framebuffer();
    }

    void graphics::Framebuffer::terminate() {
        delete_framebuffer();
    }

    void graphics::Framebuffer::create_framebuffer() {
        // 1. FBO
        glGenFramebuffers(1, &m_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

        // 2. Texture
        glGenTextures(1, &m_texture_color_buffer);
        glBindTexture(GL_TEXTURE_2D, m_texture_color_buffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1600, 900, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture_color_buffer, 0);

        // 3. RBO (Depth/Stencil)
        glGenRenderbuffers(1, &m_rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1600, 900);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            delete_framebuffer();
            throw engine::util::EngineError(
                engine::util::EngineError::Type::OpenGLError,
                "Framebuffer is not complete!");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void graphics::Framebuffer::delete_framebuffer() {
        if (m_fbo) {
            glDeleteFramebuffers(1, &m_fbo);
            m_fbo = 0;
        }
        if (m_texture_color_buffer) {
            glDeleteTextures(1, &m_texture_color_buffer);
            m_texture_color_buffer = 0;
        }
        if (m_rbo) {
            glDeleteRenderbuffers(1, &m_rbo);
            m_rbo = 0;
        }
    }

    void graphics::Framebuffer::bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        glEnable(GL_DEPTH_TEST);
    }

    void graphics::Framebuffer::unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
    }
} // engine
