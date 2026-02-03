//
// Created by nemanja on 31.1.26..
//

#include "../include/MainController.hpp"

#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

namespace app {
    class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
    public:
        void on_mouse_move(engine::platform::MousePosition position) override;
    };

    void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
        auto camera   = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
        auto mainCtrl = engine::core::Controller::get<app::MainController>();

        if (mainCtrl->isCursorEnabled()) {
            return;
        }

        float mouse_sensitivity = 0.04f;

        float xOffset = position.dx * mouse_sensitivity;
        float yOffset = position.dy * mouse_sensitivity;

        camera->rotate_camera(xOffset, yOffset);
    }

    void MainController::initialize() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

        m_cursorEnabled = false;
        platform->set_enable_cursor(m_cursorEnabled);

        platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());

        engine::graphics::OpenGL::enable_depth_testing();
    }

    bool MainController::loop() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) {
            return false;
        }
        return true;
    }

    void MainController::update() {
        auto platform  = engine::core::Controller::get<engine::platform::PlatformController>();
        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera    = graphics->camera();
        auto deltaTime = platform->dt();
        m_totalTime    += deltaTime;

        if (platform->key(engine::platform::KeyId::KEY_F2).state() == engine::platform::Key::State::JustPressed) {
            m_cursorEnabled = !m_cursorEnabled;
            platform->set_enable_cursor(m_cursorEnabled);
        }
        if (platform->key(engine::platform::KeyId::KEY_W).is_down() && !m_cursorEnabled) {
            camera->move_camera(engine::graphics::Camera::FORWARD, deltaTime);
        }
        if (platform->key(engine::platform::KeyId::KEY_S).is_down() && !m_cursorEnabled) {
            camera->move_camera(engine::graphics::Camera::BACKWARD, deltaTime);
        }
        if (platform->key(engine::platform::KeyId::KEY_A).is_down() && !m_cursorEnabled) {
            camera->move_camera(engine::graphics::Camera::LEFT, deltaTime);
        }
        if (platform->key(engine::platform::KeyId::KEY_D).is_down() && !m_cursorEnabled) {
            camera->move_camera(engine::graphics::Camera::RIGHT, deltaTime);
        }
        if (platform->key(engine::platform::KeyId::KEY_E).is_down() && !m_cursorEnabled) {
            camera->move_camera(engine::graphics::Camera::UP, deltaTime);
        }
        if (platform->key(engine::platform::KeyId::KEY_Q).is_down() && !m_cursorEnabled) {
            camera->move_camera(engine::graphics::Camera::DOWN, deltaTime);
        }
    }

    void MainController::begin_draw() {
        engine::graphics::OpenGL::clear_buffers();
    }

    void MainController::draw() {
        draw_model("terrain", "shader_model_universal", glm::vec3(0.0f, -0.55f, -80.0f),
                   glm::vec3(0.0005f, 0.0008f, 0.0008f));
        draw_model("terrain", "shader_model_universal", glm::vec3(0.0f, -0.55f, 60.0f),
                   glm::vec3(0.0005f, 0.0008f, 0.0008f), 180.0f);
        draw_model("house", "shader_model_universal", glm::vec3(0.0f, -4.0f, -25.0f));
        draw_model("convertible", "shader_model_universal", glm::vec3(-6.0f, -4.0f, -17.0f));
        draw_model("road", "shader_model_universal", glm::vec3(0.0f, -4.7f, 0.1f), glm::vec3(2.0f, 0.1f, 0.25f));
        draw_model("caravan", "shader_model_universal", glm::vec3(-25.0f, -1.0f, -22.0f), glm::vec3(0.04f), 20.0f);
        draw_model("billboard", "shader_model_universal", glm::vec3(40.0f, -4.5f, 8.0f), glm::vec3(0.016f));
        draw_model("police_car", "shader_model_universal", glm::vec3(-17.0f, -4.0f, 0.0f), glm::vec3(0.09f), 55.0f);
        draw_model("ufo", "shader_model_universal", glm::vec3(5.0f, -4.0f, 0.0f), glm::vec3(0.1f));
        draw_model("farm_house", "shader_model_universal", glm::vec3(0.0f, -4.4f, 17.0f), glm::vec3(0.8f), 180.0f);
        draw_model("tennis_court", "shader_model_universal", glm::vec3(36.0f, -4.0f, -22.0f), glm::vec3(1.6f));
        draw_model("farm_house", "shader_model_universal", m_policeCarLightLeft, glm::vec3(0.008f));
        draw_model("farm_house", "shader_model_universal", m_policeCarLightRight, glm::vec3(0.008f));

        draw_skybox();
    }

    void MainController::end_draw() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        platform->swap_buffers();
    }

    void MainController::draw_model(std::string modelName, std::string shaderName,
                                    glm::vec3 translateModel,
                                    glm::vec3 scaleModel, float rotateModelAngle) {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto platform  = engine::core::Controller::get<engine::platform::PlatformController>();

        engine::resources::Model *model   = resources->model(modelName);
        engine::resources::Shader *shader = resources->shader(shaderName);

        float speed = 5.0f;
        float m_redIntensity;
        float m_blueIntensity;

        if (m_policeEmergencyLightsActive) {
            m_redIntensity  = (sin(m_totalTime * speed) + 1.0f) / 2.0f;
            m_blueIntensity = (cos(m_totalTime * speed) + 1.0f) / 2.0f;
        } else {
            m_redIntensity  = 0.0f;
            m_blueIntensity = 0.0f;
        }
        float ambientLightsActive    = m_policeEmergencyLightsActive ? 1.0f : 0.0f;
        float policeHeadLightsActive = m_policeHeadLightsActive ? 1.0f : 0.0f;
        shader->use();
        shader->set_vec3("viewPos", graphics->camera()->Position);

        //POLICE RED LIGHT
        shader->set_vec3("pointLights[0].position", m_policeRedLightPos);
        shader->set_vec3("pointLights[0].ambient", glm::vec3(0.1f) * ambientLightsActive);
        shader->set_vec3("pointLights[0].diffuse", glm::vec3(1.0f, 0.0f, 0.0f) * m_redIntensity);
        shader->set_vec3("pointLights[0].specular", glm::vec3(0.0f));

        shader->set_float("pointLights[0].linear", 0.009f);
        shader->set_float("pointLights[0].quadratic", 0.0032f);

        //POLICE BLUE LIGHT
        shader->set_vec3("pointLights[1].position", m_policeBlueLightPos);
        shader->set_vec3("pointLights[1].ambient", glm::vec3(0.1f) * ambientLightsActive);
        shader->set_vec3("pointLights[1].diffuse", glm::vec3(0.0f, 0.0f, 1.0f) * m_blueIntensity);
        shader->set_vec3("pointLights[1].specular", glm::vec3(0.0f));

        shader->set_float("pointLights[1].linear", 0.009f);
        shader->set_float("pointLights[1].quadratic", 0.0032f);

        // Far 1 (Levi)
        shader->set_vec3("spotLights[0].position", m_policeCarLightLeft);
        shader->set_vec3("spotLights[0].direction", m_policeCarLightDirection); // Prilagodi smeru auta
        shader->set_float("spotLights[0].cutOff", glm::cos(glm::radians(12.5f)));
        shader->set_float("spotLights[0].outerCutOff", glm::cos(glm::radians(17.5f)));
        shader->set_vec3("spotLights[0].ambient", glm::vec3(0.1f) * policeHeadLightsActive);
        shader->set_vec3("spotLights[0].diffuse", glm::vec3(1.0f, 1.0f, 0.4f) * policeHeadLightsActive);
        shader->set_vec3("spotLights[0].specular", glm::vec3(0.2f) * policeHeadLightsActive);
        shader->set_float("spotLights[0].linear", 0.009f);
        shader->set_float("spotLights[0].quadratic", 0.0032f);

        // Far 2 (Desni)
        shader->set_vec3("spotLights[1].position", m_policeCarLightRight);
        shader->set_vec3("spotLights[1].direction", m_policeCarLightDirection);
        shader->set_float("spotLights[1].cutOff", glm::cos(glm::radians(12.5f)));
        shader->set_float("spotLights[1].outerCutOff", glm::cos(glm::radians(17.5f)));
        shader->set_vec3("spotLights[1].ambient", glm::vec3(0.1f) * policeHeadLightsActive);
        shader->set_vec3("spotLights[1].diffuse", glm::vec3(1.0f, 1.0f, 0.4f) * policeHeadLightsActive);
        shader->set_vec3("spotLights[1].specular", glm::vec3(0.2f) * policeHeadLightsActive);
        shader->set_float("spotLights[1].linear", 0.009f);
        shader->set_float("spotLights[1].quadratic", 0.0032f);

        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 modelTransform = glm::mat4(1.0f);
        modelTransform = glm::translate(modelTransform, translateModel);
        modelTransform = glm::rotate(modelTransform, glm::radians(rotateModelAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        modelTransform = glm::scale(modelTransform, scaleModel);
        shader->set_mat4("model", modelTransform);
        model->draw(shader);
    }

    void MainController::draw_skybox() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto skybox    = resources->skybox("skybox");
        auto shader    = resources->shader("shader_skybox");
        graphics->draw_skybox(shader, skybox);
    }
} // app
