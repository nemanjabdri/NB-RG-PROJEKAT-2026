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
        PointShadow();

        ~PointShadow();

        void bind();

        void applyUniformsToShader(const engine::resources::Shader *shader, glm::vec3 lightPos);

        void unbind(const unsigned int SCR_WIDTH, const unsigned int SCR_HEIGHT);

        unsigned int textureId() const {
            return m_depthCubeMap;
        }

    private:
        unsigned int m_depthCubeMap      = 0;
        unsigned int m_depthMapFbo       = 0;
        const unsigned int SHADOW_WIDTH  = 1024;
        const unsigned int SHADOW_HEIGHT = 1024;
        float near_plane                 = 1.0f;
        float far_plane                  = 45.0f;
        glm::mat4 shadowProj;
        std::vector<glm::mat4> shadowTransforms;
    };
}

#endif //MATF_RG_PROJECT_POINTSHADOW_HPP
