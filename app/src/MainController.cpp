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
#include <spdlog/fmt/bundled/chrono.h>

namespace app {
    class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
    public:
        void on_mouse_move(engine::platform::MousePosition position) override;
    };

    void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
        auto camera   = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
        auto mainCtrl = engine::core::Controller::get<app::MainController>();

        if (mainCtrl->isCursorEnabled() || mainCtrl->isDrivingMode()) {
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

        if (m_drivingMode) {
            float moveSpeed = 15.0f * deltaTime;
            float rotSpeed  = 80.0f * deltaTime;

            // 1. Skretanje (Menjamo ugao)
            if (platform->key(engine::platform::KeyId::KEY_A).is_down())
                m_carAngle += rotSpeed;
            if (platform->key(engine::platform::KeyId::KEY_D).is_down())
                m_carAngle -= rotSpeed;

            // 2. Izračunavanje smera (Forward Vector)
            float rad = glm::radians(m_carAngle);
            glm::vec3 forward;
            forward.x = sin(rad);
            forward.y = 0.0f;
            forward.z = cos(rad);
            forward   = glm::normalize(forward);

            // 3. Kretanje (Napred-Nazad u odnosu na smer auta)
            if (platform->key(engine::platform::KeyId::KEY_W).is_down())
                m_carPos += forward * moveSpeed;
            if (platform->key(engine::platform::KeyId::KEY_S).is_down())
                m_carPos -= forward * moveSpeed;

            float distanceBehind = 16.0f;
            float heightAbove    = 8.0f;

            rad = glm::radians(m_carAngle);
            glm::vec3 cameraPos;
            cameraPos.x = m_carPos.x - sin(rad) * distanceBehind;
            cameraPos.z = m_carPos.z - cos(rad) * distanceBehind;
            cameraPos.y = m_carPos.y + heightAbove;

            camera->Position = cameraPos;

            glm::vec3 direction = glm::normalize((m_carPos + glm::vec3(0.0f, 1.0f, 0.0f)) - camera->Position);
            float targetYaw     = glm::degrees(atan2(direction.z, direction.x));
            float targetPitch   = glm::degrees(asin(direction.y));

            float xOffset = targetYaw - camera->Yaw;
            float yOffset = targetPitch - camera->Pitch;

            while (xOffset <= -180.0f)
                xOffset += 360.0f;
            while (xOffset > 180.0f)
                xOffset -= 360.0f;

            if (m_carAngle >= 360.0f)
                m_carAngle -= 360.0f;
            if (m_carAngle < 0.0f)
                m_carAngle += 360.0f;

            camera->rotate_camera(xOffset, yOffset + 10.0f);

            if (platform->key(engine::platform::KeyId::KEY_F).state() == engine::platform::Key::State::JustPressed) {
                setPoliceHeadLightsActive(!m_policeHeadLightsActive);
            }
            if (platform->key(engine::platform::KeyId::KEY_G).state() == engine::platform::Key::State::JustPressed) {
                setPoliceEmergencyLightsActive(!m_policeEmergencyLightsActive);
            }
        } else {
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
    }

    void MainController::begin_draw() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();

        auto shader_depth = resources->shader("shader_point_shadow_depth");

        graphics->bind_PointShadow(shader_depth, m_worldRedPos);
        draw_models("shader_point_shadow_depth");
        graphics->unbind_PointShadow();

        if (m_nightVisionMode || m_greyscaleMode) {
            graphics->bind_frameBuffer();
        }

        engine::graphics::OpenGL::clear_buffers();
    }

    void MainController::draw() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();

        draw_models("shader_model_universal");
        draw_skybox();

        if (m_nightVisionMode || m_greyscaleMode) {
            graphics->unbind_frameBuffer();
            auto shader = m_nightVisionMode
                              ? resources->shader("night_vision_framebuffer_effect")
                              : resources->shader("grayscale _framebuffer_effect");

            graphics->draw_using_framebuffer(shader);
        }
    }

    void MainController::end_draw() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

        platform->swap_buffers();
    }

    void MainController::draw_models(const std::string shader) {
        draw_model("terrain", shader, glm::vec3(0.0f, -0.55f, -80.0f),
                   glm::vec3(0.0005f, 0.0008f, 0.0008f));
        draw_model("terrain", shader, glm::vec3(0.0f, -0.55f, 60.0f),
                   glm::vec3(0.0005f, 0.0008f, 0.0008f), 180.0f);
        draw_model("house", shader, glm::vec3(0.0f, -4.0f, -25.0f));
        draw_model("convertible", shader, glm::vec3(-6.0f, -4.0f, -17.0f));
        draw_model("road", shader, glm::vec3(0.0f, -4.7f, 0.1f), glm::vec3(2.0f, 0.1f, 0.25f));
        draw_model("caravan", shader, glm::vec3(-25.0f, -1.0f, -22.0f), glm::vec3(0.04f), 20.0f);
        draw_model("billboard", shader, glm::vec3(40.0f, -4.5f, 8.0f), glm::vec3(0.016f));
        draw_model("police_car", shader, m_carPos, glm::vec3(0.09f), m_carAngle);
        draw_model("farm_house", shader, glm::vec3(0.0f, -4.4f, 18.0f), glm::vec3(0.8f), 180.0f);
        draw_model("tennis_court", shader, glm::vec3(36.0f, -4.0f, -22.0f), glm::vec3(1.6f));
        draw_model("street_light", shader, glm::vec3(18.0f, -4.0f, 8.0f), glm::vec3(1.5f), 90.0f);
        draw_model("street_light", shader, glm::vec3(-46.0f, -4.0f, 8.0f), glm::vec3(1.5f),
                   90.0f);
        draw_model("street_light", shader, glm::vec3(55.0f, -3.9f, 8.0f), glm::vec3(1.5f),
                   90.0f);
        //draw_model("farm_house", shader, m_worldBluePos, glm::vec3(0.008f));
    }

    void MainController::draw_model(std::string modelName, std::string shaderName,
                                    glm::vec3 translateModel,
                                    glm::vec3 scaleModel, float rotateModelAngle) {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();

        engine::resources::Model *model   = resources->model(modelName);
        engine::resources::Shader *shader = resources->shader(shaderName);

        glm::mat4 carRot = glm::rotate(glm::mat4(1.0f), glm::radians(m_carAngle), glm::vec3(0, 1, 0));

        m_worldRedPos  = glm::vec3(carRot * glm::vec4(m_localRedPos, 1.0f)) + m_carPos;
        m_worldBluePos = glm::vec3(carRot * glm::vec4(m_localBluePos, 1.0f)) + m_carPos;
        m_worldFarLPos = glm::vec3(carRot * glm::vec4(m_localCarLightLeft, 1.0f)) + m_carPos;
        m_worldFarRPos = glm::vec3(carRot * glm::vec4(m_localCarLightRight, 1.0f)) + m_carPos;

        glm::vec3 worldSpotDir = glm::vec3(carRot * glm::vec4(m_localSpotDir, 0.0f));

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
        if (shaderName == "shader_model_universal") {
            shader->set_vec3("viewPos", graphics->camera()->Position);

            //POLICE RED LIGHT
            shader->set_vec3("pointLights[0].position", m_worldRedPos);
            shader->set_vec3("pointLights[0].ambient", glm::vec3(0.03f) * ambientLightsActive);
            shader->set_vec3("pointLights[0].diffuse", glm::vec3(1.0f, 0.0f, 0.0f) * m_redIntensity);
            shader->set_vec3("pointLights[0].specular", glm::vec3(0.0f));
            shader->set_float("pointLights[0].linear", 0.0009f);
            shader->set_float("pointLights[0].quadratic", 0.00032f);

            //POLICE BLUE LIGHT
            shader->set_vec3("pointLights[1].position", m_worldBluePos);
            shader->set_vec3("pointLights[1].ambient", glm::vec3(0.03f) * ambientLightsActive);
            shader->set_vec3("pointLights[1].diffuse", glm::vec3(0.0f, 0.0f, 1.0f) * m_blueIntensity);
            shader->set_vec3("pointLights[1].specular", glm::vec3(0.0f));
            shader->set_float("pointLights[1].linear", 0.0009f);
            shader->set_float("pointLights[1].quadratic", 0.00032f);

            // Far Levi
            shader->set_vec3("spotLights[0].position", m_worldFarLPos);
            shader->set_vec3("spotLights[0].direction", worldSpotDir); // Prilagodi smeru auta
            shader->set_float("spotLights[0].cutOff", glm::cos(glm::radians(14.5f)));
            shader->set_float("spotLights[0].outerCutOff", glm::cos(glm::radians(22.5f)));
            shader->set_vec3("spotLights[0].ambient", glm::vec3(0.03f) * policeHeadLightsActive);
            shader->set_vec3("spotLights[0].diffuse", glm::vec3(1.0f, 1.0f, 0.4f) * policeHeadLightsActive);
            shader->set_vec3("spotLights[0].specular", glm::vec3(0.2f) * policeHeadLightsActive);
            shader->set_float("spotLights[0].linear", 0.0009f);
            shader->set_float("spotLights[0].quadratic", 0.00032f);

            // Far Desni
            shader->set_vec3("spotLights[1].position", m_worldFarRPos);
            shader->set_vec3("spotLights[1].direction", worldSpotDir);
            shader->set_float("spotLights[1].cutOff", glm::cos(glm::radians(14.5f)));
            shader->set_float("spotLights[1].outerCutOff", glm::cos(glm::radians(22.5f)));
            shader->set_vec3("spotLights[1].ambient", glm::vec3(0.03f) * policeHeadLightsActive);
            shader->set_vec3("spotLights[1].diffuse", glm::vec3(1.0f, 1.0f, 0.4f) * policeHeadLightsActive);
            shader->set_vec3("spotLights[1].specular", glm::vec3(0.2f) * policeHeadLightsActive);
            shader->set_float("spotLights[1].linear", 0.0009f);
            shader->set_float("spotLights[1].quadratic", 0.00032f);

            //street-light 1
            shader->set_vec3("spotLights[2].position", m_streetLight1Pos);
            shader->set_vec3("spotLights[2].direction", m_streetLightDirection);
            shader->set_float("spotLights[2].cutOff", glm::cos(glm::radians(12.5f)));
            shader->set_float("spotLights[2].outerCutOff", glm::cos(glm::radians(25.5f)));
            shader->set_vec3("spotLights[2].ambient", glm::vec3(0.1f));
            shader->set_vec3("spotLights[2].diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
            shader->set_vec3("spotLights[2].specular", glm::vec3(0.2f));
            shader->set_float("spotLights[2].linear", 0.009f);
            shader->set_float("spotLights[2].quadratic", 0.0032f);
            //street-light 2
            shader->set_vec3("spotLights[3].position", m_streetLight2Pos);
            shader->set_vec3("spotLights[3].direction", m_streetLightDirection);
            shader->set_float("spotLights[3].cutOff", glm::cos(glm::radians(12.5f)));
            shader->set_float("spotLights[3].outerCutOff", glm::cos(glm::radians(25.5f)));
            shader->set_vec3("spotLights[3].ambient", glm::vec3(0.1f));
            shader->set_vec3("spotLights[3].diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
            shader->set_vec3("spotLights[3].specular", glm::vec3(0.2f));
            shader->set_float("spotLights[3].linear", 0.009f);
            shader->set_float("spotLights[3].quadratic", 0.0032f);

            //street-light 3
            shader->set_vec3("spotLights[4].position", m_streetLight3Pos);
            shader->set_vec3("spotLights[4].direction", m_streetLightDirection);
            shader->set_float("spotLights[4].cutOff", glm::cos(glm::radians(12.5f)));
            shader->set_float("spotLights[4].outerCutOff", glm::cos(glm::radians(25.5f)));
            shader->set_vec3("spotLights[4].ambient", glm::vec3(0.1f));
            shader->set_vec3("spotLights[4].diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
            shader->set_vec3("spotLights[4].specular", glm::vec3(0.2f));
            shader->set_float("spotLights[4].linear", 0.009f);
            shader->set_float("spotLights[4].quadratic", 0.0032f);

            shader->set_mat4("projection", graphics->projection_matrix());
            shader->set_mat4("view", graphics->camera()->view_matrix());
            glm::mat4 modelTransform = glm::mat4(1.0f);
            modelTransform = glm::translate(modelTransform, translateModel);
            modelTransform = glm::rotate(modelTransform, glm::radians(rotateModelAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            modelTransform = glm::scale(modelTransform, scaleModel);
            shader->set_mat4("model", modelTransform);

            model->draw(shader);
        } else if (shaderName == "shader_point_shadow_depth") {
            glm::mat4 modelTransform = glm::mat4(1.0f);
            modelTransform = glm::translate(modelTransform, translateModel);
            modelTransform = glm::rotate(modelTransform, glm::radians(rotateModelAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            modelTransform = glm::scale(modelTransform, scaleModel);
            shader->set_mat4("model", modelTransform);
            model->draw(shader);
        }
    }

    void MainController::render_scene_geometry(std::string modelName, std::string shaderName,
                                               glm::vec3 translateModel,
                                               glm::vec3 scaleModel, float rotateModelAngle) {
    }

    void MainController::setup_lights(std::string shaderName) {
    }

    void MainController::draw_skybox() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto skybox    = resources->skybox("skybox");
        auto shader    = resources->shader("shader_skybox");
        graphics->draw_skybox(shader, skybox);
    }
} // app
