//
// Created by nemanja on 8.2.26..
//

#include <cstddef>
#include <vector>
#include <engine/graphics/PointShadow.hpp>
#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <engine/resources/Shader.hpp>

namespace engine {
    graphics::PointShadow::PointShadow() {
        glGenFramebuffers(1, &m_depthMapFbo);
        glGenTextures(1, &m_depthCubeMap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubeMap);
        for (unsigned int i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0,
                         GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFbo);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthCubeMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    graphics::PointShadow::~PointShadow() {
        glDeleteFramebuffers(1, &m_depthMapFbo);
        glDeleteTextures(1, &m_depthCubeMap);
    }

    void graphics::PointShadow::bind() {
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFbo);
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    void graphics::PointShadow::applyUniformsToShader(const engine::resources::Shader *shader, glm::vec3 lightPos) {
        shadowProj = glm::perspective(glm::radians(90.0f), (float) SHADOW_WIDTH / (float) SHADOW_HEIGHT,
                                      near_plane, far_plane);

        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos,
                                                            lightPos + glm::vec3(1.0f, 0.0f, 0.0f),
                                                            glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos,
                                                            lightPos + glm::vec3(-1.0f, 0.0f, 0.0f),
                                                            glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos,
                                                            lightPos + glm::vec3(0.0f, 1.0f, 0.0f),
                                                            glm::vec3(0.0f, 0.0f, 1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos,
                                                            lightPos + glm::vec3(0.0f, -1.0f, 0.0f),
                                                            glm::vec3(0.0f, 0.0f, -1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos,
                                                            lightPos + glm::vec3(0.0f, 0.0f, 1.0f),
                                                            glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos,
                                                            lightPos + glm::vec3(0.0f, 0.0f, -1.0f),
                                                            glm::vec3(0.0f, -1.0f, 0.0f)));

        shader->use();
        for (unsigned int i = 0; i < 6; ++i) {
            shader->set_mat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
        }
        shader->set_float("far_plane", far_plane);
        shader->set_vec3("lightPos", lightPos);
    }

    void graphics::PointShadow::unbind(const unsigned int SCR_WIDTH, const unsigned int SCR_HEIGHT) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    }
}

#include "../include/engine/graphics/PointShadow.hpp"
