
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/graphics/Framebuffer.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/Skybox.hpp>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <engine/graphics/PointShadow.hpp>

namespace engine::graphics {
    GraphicsController::GraphicsController() = default;

    GraphicsController::~GraphicsController() = default;

    void GraphicsController::initialize() {
        const int opengl_initialized = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
        RG_GUARANTEE(opengl_initialized, "OpenGL failed to init!");

        auto platform               = engine::core::Controller::get<platform::PlatformController>();
        auto handle                 = platform->window()->handle_();
        m_perspective_params.FOV    = glm::radians(m_camera.Zoom);
        m_perspective_params.Width  = static_cast<float>(platform->window()->width());
        m_perspective_params.Height = static_cast<float>(platform->window()->height());
        m_perspective_params.Near   = 0.1f;
        m_perspective_params.Far    = 100.f;
        m_ortho_params.Bottom       = 0.0f;
        m_ortho_params.Top          = static_cast<float>(platform->window()->height());
        m_ortho_params.Left         = 0.0f;
        m_ortho_params.Right        = static_cast<float>(platform->window()->width());
        m_ortho_params.Near         = 0.1f;
        m_ortho_params.Far          = 100.0f;

        platform->register_platform_event_observer(std::make_unique<GraphicsPlatformEventObserver>(this));
        CHECKED_GL_CALL(glViewport, 0, 0, platform->window()->width(), platform->window()->height());

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;
        RG_GUARANTEE(ImGui_ImplGlfw_InitForOpenGL(handle, true), "ImGUI failed to initialize for OpenGL");
        RG_GUARANTEE(ImGui_ImplOpenGL3_Init("#version 330 core"), "ImGUI failed to initialize for OpenGL");

        m_framebuffer = std::make_unique<Framebuffer>();
        m_pointShadow = std::make_unique<PointShadow>();
    }

    void GraphicsController::terminate() {
        if (ImGui::GetCurrentContext()) {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }
        m_framebuffer.reset();
        m_pointShadow.reset();
    }

    void GraphicsPlatformEventObserver::on_window_resize(int width, int height) {
        m_graphics->perspective_params().Width  = static_cast<float>(width);
        m_graphics->perspective_params().Height = static_cast<float>(height);
        m_graphics->orthographic_params().Right = static_cast<float>(width);
        m_graphics->orthographic_params().Top   = static_cast<float>(height);
        CHECKED_GL_CALL(glViewport, 0, 0, width, height);
    }

    std::string_view GraphicsController::name() const {
        return "GraphicsController";
    }

    void GraphicsController::begin_gui() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void GraphicsController::end_gui() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void GraphicsController::draw_skybox(const resources::Shader *shader, const resources::Skybox *skybox) {
        glm::mat4 view = glm::mat4(glm::mat3(m_camera.view_matrix()));
        shader->use();
        shader->set_mat4("view", view);
        shader->set_mat4("projection", projection_matrix<>());
        CHECKED_GL_CALL(glDepthFunc, GL_LEQUAL);
        CHECKED_GL_CALL(glBindVertexArray, skybox->vao());
        CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE0);
        CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_CUBE_MAP, skybox->texture());
        CHECKED_GL_CALL(glDrawArrays, GL_TRIANGLES, 0, 36);
        CHECKED_GL_CALL(glBindVertexArray, 0);
        CHECKED_GL_CALL(glDepthFunc, GL_LESS); // set depth function back to default
        CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_CUBE_MAP, 0);
    }

    void GraphicsController::bind_frameBuffer() {
        if (m_framebuffer) {
            m_framebuffer->bind();
        }
    }

    void GraphicsController::unbind_frameBuffer() {
        if (m_framebuffer) {
            m_framebuffer->unbind();
        }
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void GraphicsController::draw_using_framebuffer(const resources::Shader *shader) {
        if (m_quadVAO == 0) {
            float quadVertices[] = {
                -1.0, 1.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f,
                1.0f, -1.0f, 1.0f, 0.0f,

                -1.0f, 1.0f, 0.0f, 1.0f,
                1.0f, -1.0f, 1.0f, 0.0f,
                1.0f, 1.0f, 1.0f, 1.0f,
            };
            glGenVertexArrays(1, &m_quadVAO);
            glGenBuffers(1, &m_quadVBO);
            glBindVertexArray(m_quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);

            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));
        }

        shader->use();
        shader->set_int("screenTexture", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(m_quadVAO);

        if (m_framebuffer) {
            glBindTexture(GL_TEXTURE_2D, m_framebuffer->textureId());
        }

        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glEnable(GL_DEPTH_TEST);
    }

    void GraphicsController::bind_PointShadow(const resources::Shader *shader, glm::vec3 lightPos) {
        if (m_pointShadow) {
            m_pointShadow->bind();
            m_pointShadow->applyUniformsToShader(shader, lightPos);
        }
    }

    void GraphicsController::unbind_PointShadow() {
        if (m_pointShadow) {
            m_pointShadow->unbind(1600, 900);
        }
    }

    unsigned int GraphicsController::pointShadowTextureId() const {
        if (m_pointShadow)
            return m_pointShadow->textureId();
        return 0;
    }
} // namespace engine::graphics
