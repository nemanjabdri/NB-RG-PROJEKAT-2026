//
// Created by nemanja on 4.2.26..
//

#ifndef MATF_RG_PROJECT_FRAMEBUFFER_HPP
#define MATF_RG_PROJECT_FRAMEBUFFER_HPP

namespace engine::graphics {
class Framebuffer {
public:
    Framebuffer(int width, int height);

    Framebuffer(const Framebuffer &) = delete;

    Framebuffer &operator=(const Framebuffer &) = delete;

    void terminate();

    void bind();

    void unbind();

    unsigned int texture_id() const {
        return m_texture_color_buffer;
    }

private:
    unsigned int m_texture_color_buffer = 0;
    unsigned int m_fbo = 0;
    unsigned int m_rbo = 0;
    int m_width = 0;
    int m_height = 0;

    void create_framebuffer();

    void delete_framebuffer();
};
}// namespace engine::graphics

#endif//MATF_RG_PROJECT_FRAMEBUFFER_HPP
