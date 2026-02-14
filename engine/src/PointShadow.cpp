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
#include <spdlog/spdlog.h>

namespace engine {
    graphics::PointShadow::PointShadow(unsigned int width, unsigned int height) : m_shadow_width(width)
      , m_shadow_height(height) {
        glGenFramebuffers(1, &m_depth_map_fbo);
        glGenTextures(1, &m_depth_cube_map);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_depth_cube_map);
        for (unsigned int i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, m_shadow_width, m_shadow_height, 0,
                         GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glBindFramebuffer(GL_FRAMEBUFFER, m_depth_map_fbo);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depth_cube_map, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            spdlog::error("PointShadow Framebuffer is not complete!");
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            cleanup();
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    graphics::PointShadow::~PointShadow() {
        cleanup();
    }

    void graphics::PointShadow::cleanup() {
        if (m_depth_map_fbo) {
            glDeleteFramebuffers(1, &m_depth_map_fbo);
            m_depth_map_fbo = 0;
        }
        if (m_depth_cube_map) {
            glDeleteTextures(1, &m_depth_cube_map);
            m_depth_cube_map = 0;
        }
    }

    /*void graphics::PointShadow::bind() {
        glViewport(0, 0, m_shadow_width, m_shadow_height);
        glBindFramebuffer(GL_FRAMEBUFFER, m_depth_map_fbo);

        glDisable(GL_SCISSOR_TEST);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glClear(GL_DEPTH_BUFFER_BIT);
    }*/

    void graphics::PointShadow::bind() {
        // 1. Podesi Viewport na rezoluciju senke (npr. 2048x2048)
        glViewport(0, 0, m_shadow_width, m_shadow_height);

        // 2. Binduj FBO
        glBindFramebuffer(GL_FRAMEBUFFER, m_depth_map_fbo);

        // --- FIX 1: Otkacinjanje teksture (Feedback Loop) ---
        // Ako je tekstura ostala vezana na slotu 5 u draw(), ovo je mora otkačiti
        // da bi drajver dozvolio pisanje u nju.
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        // ---------------------------------------------------

        // --- FIX 2: Resetovanje Depth Funkcije (Skybox problem) ---
        // Skybox često postavlja GL_LEQUAL. Ako to ostane, a brišemo bafer na 1.0,
        // ništa se neće upisati jer 1.0 nije manje od 1.0. Vraćamo na default.
        glDepthFunc(GL_LESS);
        // ----------------------------------------------------------

        // --- FIX 3: Resetovanje Depth Maske ---
        // Skybox gasi pisanje u dubinu. Ovde ga palimo.
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE); // --------------------------------------

        // --- FIX 4: Scissor Test (UI problem) ---
        // Ako imas UI biblioteke, one cesto seku ekran. Gasimo to.
        glDisable(GL_SCISSOR_TEST);
        // ----------------------------------------

        // Opciono: Isključi Face Culling za senke da bi izbegao "Peter Panning"
        // (Renderuje i prednje i zadnje strane, ili samo prednje ako zelis)
        // glDisable(GL_CULL_FACE);

        // Konačno brisanje - sada bi moralo da radi
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    void graphics::PointShadow::bind_uniforms(const engine::resources::Shader *shader, glm::vec3 light_pos) {
        glm::mat4 shadow_proj = glm::perspective(glm::radians(90.0f), (float) m_shadow_width / (float) m_shadow_height,
                                                 m_near_plane, m_far_plane);

        std::vector<glm::mat4> shadow_transforms;

        shadow_transforms.push_back(shadow_proj * glm::lookAt(light_pos,
                                                              light_pos + glm::vec3(1.0f, 0.0f, 0.0f),
                                                              glm::vec3(0.0f, -1.0f, 0.0f)));
        shadow_transforms.push_back(shadow_proj * glm::lookAt(light_pos,
                                                              light_pos + glm::vec3(-1.0f, 0.0f, 0.0f),
                                                              glm::vec3(0.0f, -1.0f, 0.0f)));
        shadow_transforms.push_back(shadow_proj * glm::lookAt(light_pos,
                                                              light_pos + glm::vec3(0.0f, 1.0f, 0.0f),
                                                              glm::vec3(0.0f, 0.0f, 1.0f)));
        shadow_transforms.push_back(shadow_proj * glm::lookAt(light_pos,
                                                              light_pos + glm::vec3(0.0f, -1.0f, 0.0f),
                                                              glm::vec3(0.0f, 0.0f, -1.0f)));
        shadow_transforms.push_back(shadow_proj * glm::lookAt(light_pos,
                                                              light_pos + glm::vec3(0.0f, 0.0f, 1.0f),
                                                              glm::vec3(0.0f, -1.0f, 0.0f)));
        shadow_transforms.push_back(shadow_proj * glm::lookAt(light_pos,
                                                              light_pos + glm::vec3(0.0f, 0.0f, -1.0f),
                                                              glm::vec3(0.0f, -1.0f, 0.0f)));

        shader->use();
        for (unsigned int i = 0; i < 6; ++i) {
            shader->set_mat4("shadowMatrices[" + std::to_string(i) + "]", shadow_transforms[i]);
        }
        shader->set_float("far_plane", m_far_plane);
        shader->set_vec3("lightPos", light_pos);
    }

    void graphics::PointShadow::unbind(const unsigned int screen_width, const unsigned int screen_height) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, screen_width, screen_height);
    }
}
