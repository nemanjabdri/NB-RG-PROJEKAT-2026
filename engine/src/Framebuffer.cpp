//
// Created by nemanja on 4.2.26..
//

#include <engine/graphics/Framebuffer.hpp>
#include <glad/glad.h>
#include <cstddef>

namespace engine {
    graphics::Framebuffer::Framebuffer() {
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
            // poruka: sve je ok
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    graphics::Framebuffer::~Framebuffer() {
        glDeleteFramebuffers(1, &m_fbo);
        glDeleteTextures(1, &m_texture_color_buffer);
        glDeleteRenderbuffers(1, &m_rbo);
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
