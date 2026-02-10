//
// Created by nemanja on 31.1.26..
//

#include "../include/MainController.hpp"

#include <glad/glad.h>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>
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

        if (m_ufoState != UfoState::GROUND_IDLE && m_ufoState != UfoState::WAITING_TO_TAKEOFF) {
            m_ufoRotation += 200.0f * deltaTime;
        }

        switch (m_ufoState) {
        case UfoState::WAITING_TO_LAND: m_stateTimer += deltaTime;
            if (m_stateTimer >= 2.0f) {
                m_ufoState = UfoState::DESCENDING;
            }
            break;

        case UfoState::DESCENDING:
            // Smanjujemo Y poziciju
            m_ufoPos.y -= UFO_SPEED * deltaTime;
            // Provera da li smo dotakli tlo
            if (m_ufoPos.y <= UFO_GROUND_Y) {
                m_ufoPos.y = UFO_GROUND_Y;
                m_ufoState = UfoState::GROUND_IDLE;
            }
            break;

        case UfoState::WAITING_TO_TAKEOFF: m_stateTimer += deltaTime;
            if (m_stateTimer >= 2.0f) {
                m_ufoState = UfoState::ASCENDING;
            }
            break;

        case UfoState::ASCENDING:
            // Povećavamo Y poziciju
            m_ufoPos.y += UFO_SPEED * deltaTime;

            // Provera da li smo stigli na nebo
            if (m_ufoPos.y >= UFO_SKY_Y) {
                m_ufoPos.y = UFO_SKY_Y;
                m_ufoState = UfoState::SKY_IDLE;
            }
            break;

        default: break;
        }

        if (m_drivingMode) {
            float moveSpeed = 15.0f * deltaTime;
            float rotSpeed  = 80.0f * deltaTime;

            if (m_firstEntry) {
                m_cameraOrbitAngle = glm::radians(m_carAngle);
                m_firstEntry       = false;
            }

            float camRotSpeed = 60.0f * deltaTime;

            if (platform->key(engine::platform::KeyId::KEY_LEFT).is_down()) {
                m_cameraOrbitAngle -= camRotSpeed;
            }
            if (platform->key(engine::platform::KeyId::KEY_RIGHT).is_down()) {
                m_cameraOrbitAngle += camRotSpeed;
            }

            // 1. Skretanje
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

            rad = glm::radians(m_cameraOrbitAngle);
            glm::vec3 cameraPos;
            cameraPos.x = m_carPos.x - sin(rad) * m_cameraDist;
            cameraPos.z = m_carPos.z - cos(rad) * m_cameraDist;
            cameraPos.y = m_carPos.y + m_cameraHeight;

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

            camera->rotate_camera(xOffset, yOffset + m_pitch_offset);

            if (platform->key(engine::platform::KeyId::KEY_F).state() == engine::platform::Key::State::JustPressed) {
                setPoliceHeadLightsActive(!m_policeHeadLightsActive);
            }
            if (platform->key(engine::platform::KeyId::KEY_G).state() == engine::platform::Key::State::JustPressed) {
                setPoliceEmergencyLightsActive(!m_policeEmergencyLightsActive);
            }
            if (platform->key(engine::platform::KeyId::KEY_F1).state() == engine::platform::Key::State::JustPressed) {
                setDrivingMode(false);
            }
        } else {
            m_firstEntry = true;

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
            if (platform->key(engine::platform::KeyId::KEY_F1).state() == engine::platform::Key::State::JustPressed) {
                setDrivingMode(true);
            }
        }
    }

    void MainController::begin_draw() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();

        auto shader_depth = resources->shader("shader_point_shadow_depth");

        graphics->bind_PointShadow(shader_depth, m_campFireLightPos);
        render_scene_geometry(shader_depth);
        graphics->unbind_PointShadow();

        if (m_nightVisionMode || m_greyscaleMode) {
            graphics->bind_frameBuffer();
        }

        engine::graphics::OpenGL::clear_buffers();
    }

    void MainController::draw() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();

        auto shader_universal = resources->shader("shader_model_universal");
        shader_universal->use();
        shader_universal->set_mat4("projection", graphics->projection_matrix());
        shader_universal->set_mat4("view", graphics->camera()->view_matrix());

        setup_scene_lights(shader_universal);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_CUBE_MAP, graphics->pointShadowTextureId());
        shader_universal->set_int("depthMap", 5);
        shader_universal->set_float("far_plane", 35.0f);

        render_scene_geometry(shader_universal);
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

    void MainController::render_scene_geometry(engine::resources::Shader *shader) {
        render_model_geometry("terrain", shader, glm::vec3(0.0f, -0.55f, -80.0f),
                              glm::vec3(0.0005f, 0.0008f, 0.0008f));
        render_model_geometry("terrain", shader, glm::vec3(0.0f, -0.55f, 60.0f),
                              glm::vec3(0.0005f, 0.0008f, 0.0008f), 180.0f);
        render_model_geometry("house", shader, glm::vec3(0.0f, -4.0f, -25.0f));
        render_model_geometry("convertible", shader, glm::vec3(-6.0f, -4.0f, -17.0f));
        render_model_geometry("road", shader, glm::vec3(0.0f, -4.7f, 0.1f), glm::vec3(2.0f, 0.1f, 0.25f));
        render_model_geometry("caravan", shader, glm::vec3(-17.0f, -0.6f, -33.0f), glm::vec3(0.045f), 20.0f);
        render_model_geometry("billboard", shader, glm::vec3(40.0f, -4.5f, 8.0f), glm::vec3(0.016f));
        render_model_geometry("police_car", shader, m_carPos, glm::vec3(0.09f), m_carAngle);
        render_model_geometry("farm_house", shader, glm::vec3(0.0f, -4.4f, 18.0f), glm::vec3(0.8f), 180.0f);
        render_model_geometry("tennis_court", shader, glm::vec3(36.0f, -4.0f, -22.0f), glm::vec3(1.6f));
        render_model_geometry("street_light", shader, glm::vec3(18.0f, -4.0f, 8.0f), glm::vec3(1.5f), 90.0f);
        render_model_geometry("street_light", shader, glm::vec3(-46.0f, -4.0f, 8.0f), glm::vec3(1.5f),
                              90.0f);
        render_model_geometry("street_light", shader, glm::vec3(55.0f, -3.9f, 8.0f), glm::vec3(1.5f),
                              90.0f);
        render_model_geometry("building", shader, glm::vec3(-30.0f, -4.0f, 12.0f), glm::vec3(1.3f),
                              180.0f);
        render_model_geometry("bakery", shader, glm::vec3(-55.0f, -4.0f, -12.0f), glm::vec3(1.0f),
                              0.0f);
        render_model_geometry("UFO", shader, m_ufoPos, glm::vec3(0.1f),
                              m_ufoRotation);
        render_model_geometry("bank", shader, glm::vec3(66.0f, -4.0f, -17.0f), glm::vec3(0.04f),
                              -90.0f);
        render_model_geometry("cinema", shader, glm::vec3(95.0f, -4.0f, -13.0f), glm::vec3(1.0f),
                              0.0f);
        render_model_geometry("saloon", shader, glm::vec3(25.0f, -4.0f, 16.0f), glm::vec3(1.0f),
                              180.0f);
        render_model_geometry("campfire", shader, glm::vec3(-30.0f, -3.4f, -30.0f), glm::vec3(1.5f),
                              0.0f);
        render_model_geometry("cactus", shader, glm::vec3(-30.0f, -4.0f, -40.0f), glm::vec3(0.3f),
                              0.0f);
        render_model_geometry("cactus", shader, glm::vec3(-22.0f, -4.0f, -26.0f), glm::vec3(0.25f),
                              45.0f);
        render_model_geometry("cactus", shader, glm::vec3(-30.0f, -4.0f, -21.0f), glm::vec3(0.25f),
                              20.0f);
        render_model_geometry("lawn_mower", shader, glm::vec3(-38.0f, -4.0f, -24.0f), glm::vec3(0.05f),
                              45.0f);
        render_model_geometry("wood_swing", shader, glm::vec3(-37.0f, -4.0f, -40.0f), glm::vec3(0.04f),
                              45.0f);
        render_model_geometry("farm_house", shader, glm::vec3(m_ufoPos.x, m_ufoPos.y + 6, m_ufoPos.z),
                              glm::vec3(0.01f));
    }

    void MainController::render_model_geometry(std::string modelName, engine::resources::Shader *shader,
                                               glm::vec3 translateModel,
                                               glm::vec3 scaleModel, float rotateModelAngle) {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();

        engine::resources::Model *model = resources->model(modelName);

        glm::mat4 modelTransform = glm::mat4(1.0f);
        modelTransform = glm::translate(modelTransform, translateModel);
        modelTransform = glm::rotate(modelTransform, glm::radians(rotateModelAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        modelTransform = glm::scale(modelTransform, scaleModel);
        shader->set_mat4("model", modelTransform);

        model->draw(shader);
    }

    void MainController::setup_scene_lights(engine::resources::Shader *shader) {
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

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

        shader->set_vec3("viewPos", graphics->camera()->Position);

        //POLICE RED LIGHT
        shader->set_vec3("pointLights[0].position", m_worldRedPos);
        shader->set_vec3("pointLights[0].ambient", glm::vec3(0.02f) * ambientLightsActive);
        shader->set_vec3("pointLights[0].diffuse", glm::vec3(1.0f, 0.0f, 0.0f) * m_redIntensity);
        shader->set_vec3("pointLights[0].specular", glm::vec3(0.0f));
        shader->set_float("pointLights[0].linear", 0.009f);
        shader->set_float("pointLights[0].quadratic", 0.0032f);

        //POLICE BLUE LIGHT
        shader->set_vec3("pointLights[1].position", m_worldBluePos);
        shader->set_vec3("pointLights[1].ambient", glm::vec3(0.02f) * ambientLightsActive);
        shader->set_vec3("pointLights[1].diffuse", glm::vec3(0.0f, 0.0f, 1.0f) * m_blueIntensity);
        shader->set_vec3("pointLights[1].specular", glm::vec3(0.0f));
        shader->set_float("pointLights[1].linear", 0.009f);
        shader->set_float("pointLights[1].quadratic", 0.0032f);

        float fireSpeed     = 2.0f;
        float fireIntensity = (sin(m_totalTime * fireSpeed) * 0.2f) + (sin(m_totalTime * fireSpeed * 2.1f) * 0.1f) +
                              0.7f;
        float baseLinear    = 0.009f;
        float baseQuadratic = 0.0032f;
        //Camp fire
        shader->set_vec3("pointLights[2].position", m_campFireLightPos);
        shader->set_vec3("pointLights[2].ambient", glm::vec3(0.005f));
        shader->set_vec3("pointLights[2].diffuse", glm::vec3(1.0f, 0.6f, 0.2f) * 1.5f * fireIntensity);
        shader->set_vec3("pointLights[2].specular", glm::vec3(0.5f) * fireIntensity);
        shader->set_float("pointLights[2].linear", baseLinear / fireIntensity);
        shader->set_float("pointLights[2].quadratic", baseQuadratic / fireIntensity);

        glm::vec3 ufoLightPos = glm::vec3(m_ufoPos.x, m_ufoPos.y + 6, m_ufoPos.z);

        //UFO cockpit LIGHT
        shader->set_vec3("pointLights[3].position", ufoLightPos);
        shader->set_vec3("pointLights[3].ambient", glm::vec3(0.03f));
        shader->set_vec3("pointLights[3].diffuse", glm::vec3(0.75f, 0.0f, 1.0f) * 2.5f);
        shader->set_vec3("pointLights[3].specular", glm::vec3(0.2f));
        shader->set_float("pointLights[3].linear", 0.09f);
        shader->set_float("pointLights[3].quadratic", 0.032f);

        // Far Levi
        shader->set_vec3("spotLights[0].position", m_worldFarLPos);
        shader->set_vec3("spotLights[0].direction", worldSpotDir); // Prilagodi smeru auta
        shader->set_float("spotLights[0].cutOff", glm::cos(glm::radians(14.5f)));
        shader->set_float("spotLights[0].outerCutOff", glm::cos(glm::radians(22.5f)));
        shader->set_vec3("spotLights[0].ambient", glm::vec3(0.001f) * policeHeadLightsActive);
        shader->set_vec3("spotLights[0].diffuse", glm::vec3(1.0f, 1.0f, 0.4f) * policeHeadLightsActive);
        shader->set_vec3("spotLights[0].specular", glm::vec3(0.5f) * policeHeadLightsActive);
        shader->set_float("spotLights[0].linear", 0.0009f);
        shader->set_float("spotLights[0].quadratic", 0.00032f);

        // Far Desni
        shader->set_vec3("spotLights[1].position", m_worldFarRPos);
        shader->set_vec3("spotLights[1].direction", worldSpotDir);
        shader->set_float("spotLights[1].cutOff", glm::cos(glm::radians(14.5f)));
        shader->set_float("spotLights[1].outerCutOff", glm::cos(glm::radians(22.5f)));
        shader->set_vec3("spotLights[1].ambient", glm::vec3(0.001f) * policeHeadLightsActive);
        shader->set_vec3("spotLights[1].diffuse", glm::vec3(1.0f, 1.0f, 0.4f) * policeHeadLightsActive);
        shader->set_vec3("spotLights[1].specular", glm::vec3(0.5f) * policeHeadLightsActive);
        shader->set_float("spotLights[1].linear", 0.009f);
        shader->set_float("spotLights[1].quadratic", 0.0032f);

        //street-light 1
        shader->set_vec3("spotLights[2].position", m_streetLight1Pos);
        shader->set_vec3("spotLights[2].direction", m_streetLightDirection);
        shader->set_float("spotLights[2].cutOff", glm::cos(glm::radians(12.5f)));
        shader->set_float("spotLights[2].outerCutOff", glm::cos(glm::radians(35.5f)));
        shader->set_vec3("spotLights[2].ambient", glm::vec3(0.1f));
        shader->set_vec3("spotLights[2].diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        shader->set_vec3("spotLights[2].specular", glm::vec3(0.3f));
        shader->set_float("spotLights[2].linear", 0.009f);
        shader->set_float("spotLights[2].quadratic", 0.0032f);
        //street-light 2
        shader->set_vec3("spotLights[3].position", m_streetLight2Pos);
        shader->set_vec3("spotLights[3].direction", m_streetLightDirection);
        shader->set_float("spotLights[3].cutOff", glm::cos(glm::radians(12.5f)));
        shader->set_float("spotLights[3].outerCutOff", glm::cos(glm::radians(35.5f)));
        shader->set_vec3("spotLights[3].ambient", glm::vec3(0.1f));
        shader->set_vec3("spotLights[3].diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        shader->set_vec3("spotLights[3].specular", glm::vec3(0.3f));
        shader->set_float("spotLights[3].linear", 0.009f);
        shader->set_float("spotLights[3].quadratic", 0.0032f);

        float flickerThreshold = 0.85f;
        float randomVal        = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        float streetLightFlicker;
        if (randomVal > flickerThreshold) {
            // Nagli pad ili potpuni mrak
            streetLightFlicker = (randomVal > 0.95f) ? 0.0f : 0.2f;
        } else {
            // Normalno svetlo sa blagim šumom
            streetLightFlicker = 0.9f + (randomVal * 0.1f);
        }

        //street-light 3
        shader->set_vec3("spotLights[4].position", m_streetLight3Pos);
        shader->set_vec3("spotLights[4].direction", m_streetLightDirection);
        shader->set_float("spotLights[4].cutOff", glm::cos(glm::radians(12.5f)));
        shader->set_float("spotLights[4].outerCutOff", glm::cos(glm::radians(35.5f)));
        shader->set_vec3("spotLights[4].ambient", glm::vec3(0.1f));
        shader->set_vec3("spotLights[4].diffuse", glm::vec3(1.0f, 1.0f, 1.0f) * streetLightFlicker);
        shader->set_vec3("spotLights[4].specular", glm::vec3(0.3f) * streetLightFlicker);
        shader->set_float("spotLights[4].linear", 0.009f);
        shader->set_float("spotLights[4].quadratic", 0.0032f);
    }

    void MainController::draw_skybox() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto skybox    = resources->skybox("skybox");
        auto shader    = resources->shader("shader_skybox");
        graphics->draw_skybox(shader, skybox);
    }
} // app
