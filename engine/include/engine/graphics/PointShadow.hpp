//
// Created by nemanja on 8.2.26..
//

#ifndef MATF_RG_PROJECT_POINTSHADOW_HPP
#define MATF_RG_PROJECT_POINTSHADOW_HPP
#include <engine/resources/Shader.hpp>
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>

namespace engine::graphics {
class PointShadow {
public:
    PointShadow(unsigned int width = 2048, unsigned int height = 2048);

    PointShadow(const PointShadow &) = delete;

    PointShadow &operator=(const PointShadow &) = delete;

    void terminate();

    void bind();

    void bind_uniforms(const engine::resources::Shader *shader, glm::vec3 light_pos);

    void unbind(const unsigned int screen_width, const unsigned int screen_height);

    unsigned int texture_id() const {
        return m_depth_cube_map;
    }

private:
    unsigned int m_depth_cube_map = 0;
    unsigned int m_depth_map_fbo = 0;

    unsigned int m_shadow_width;
    unsigned int m_shadow_height;

    float m_shadow_near_plane = 1.0f;
    float m_shadow_far_plane = 50.0f;

    void cleanup();
};
}// namespace engine::graphics

#endif//MATF_RG_PROJECT_POINTSHADOW_HPP
