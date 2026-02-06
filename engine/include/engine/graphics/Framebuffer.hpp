//
// Created by nemanja on 4.2.26..
//

#ifndef MATF_RG_PROJECT_FRAMEBUFFER_HPP
#define MATF_RG_PROJECT_FRAMEBUFFER_HPP

namespace engine::graphics {
    class Framebuffer {
    public:
        Framebuffer();

        ~Framebuffer();

        void bind();

        void unbind();

        unsigned int textureId() const {
            return m_textureColorBuffer;
        }

    private:
        unsigned int m_textureColorBuffer = 0;
        unsigned int m_fbo                = 0;
        unsigned int m_rbo                = 0;
    };
} // engine

#endif //MATF_RG_PROJECT_FRAMEBUFFER_HPP
