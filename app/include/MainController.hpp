//
// Created by nemanja on 31.1.26..
//

#ifndef MATF_RG_PROJECT_MAINCONTROLLER_HPP
#define MATF_RG_PROJECT_MAINCONTROLLER_HPP
#include <engine/core/Controller.hpp>
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>

namespace engine::resources {
    class Shader;
}

namespace app {
    class MainController : public engine::core::Controller {
    private:
        float m_totalTime                  = 0.0f;
        glm::vec3 m_streetLight1Pos        = glm::vec3(18.0f, 20.0, 4.0f);
        glm::vec3 m_streetLight2Pos        = glm::vec3(-46.0f, 20.0, 4.0f);
        glm::vec3 m_streetLight3Pos        = glm::vec3(55.0f, 20.0, 4.0f);
        glm::vec3 m_streetLightDirection   = glm::vec3(0.0f, -1.0f, -0.1f);
        glm::vec3 m_localCarLightLeft      = glm::vec3(-1.395f, 1.395f, 2.8);
        glm::vec3 m_localCarLightRight     = glm::vec3(1.395f, 1.395f, 2.8);
        glm::vec3 m_localRedPos            = glm::vec3(0.765f, 3.6f, -1.48f);
        glm::vec3 m_localBluePos           = glm::vec3(-0.765f, 3.6f, -1.48f);
        glm::vec3 m_localSpotDir           = glm::vec3(0.0f, -0.2f, 1.0f);
        glm::vec3 m_worldFarLPos           = glm::vec3(-1.395f, 1.395f, 2.8);
        glm::vec3 m_worldFarRPos           = glm::vec3(1.395f, 1.395f, 2.8);
        glm::vec3 m_worldRedPos            = glm::vec3(0.765f, 4.6f, -1.48f);
        glm::vec3 m_worldBluePos           = glm::vec3(-0.765f, 4.6f, -1.48f);
        bool m_cursorEnabled               = false;
        bool m_policeEmergencyLightsActive = false;
        bool m_policeHeadLightsActive      = false;
        bool m_drivingMode                 = false;
        bool m_nightVisionMode             = false;
        bool m_greyscaleMode               = false;

        glm::vec3 m_carPos = glm::vec3(-17.0f, -4.0f, 0.0f);
        float m_carAngle   = 55.0f;

        void initialize() override;

        bool loop() override;

        void update() override;

        void begin_draw() override;

        void draw() override;

        void end_draw() override;

        void render_scene_geometry(engine::resources::Shader *shader);

        void render_model_geometry(std::string modelName, engine::resources::Shader *shader,
                                   glm::vec3 translateModel,
                                   glm::vec3 scaleModel = glm::vec3(1.0f), float rotateModelAngle = 0.0f);

        void setup_scene_lights(engine::resources::Shader *shader);

        void draw_skybox();

    public:
        bool isCursorEnabled() const {
            return m_cursorEnabled;
        }

        bool isPoliceEmergencyLightsActive() {
            return m_policeEmergencyLightsActive;
        }

        void setPoliceEmergencyLightsActive(const bool active) {
            m_policeEmergencyLightsActive = active;
        }

        bool isPoliceHeadLightsActive() {
            return m_policeHeadLightsActive;
        }

        void setPoliceHeadLightsActive(const bool active) {
            m_policeHeadLightsActive = active;
        }

        void setStreetLight1Pos(const glm::vec3 &pos) {
            m_streetLight1Pos = pos;
        }

        glm::vec3 getStreetLight1Pos() const {
            return m_streetLight1Pos;
        }

        void setStreetLight2Pos(const glm::vec3 &pos) {
            m_streetLight2Pos = pos;
        }

        glm::vec3 getStreetLight2Pos() const {
            return m_streetLight2Pos;
        }

        void setStreetLight3Pos(const glm::vec3 &pos) {
            m_streetLight3Pos = pos;
        }

        glm::vec3 getStreetLight3Pos() const {
            return m_streetLight3Pos;
        }

        std::string_view name() const override {
            return "MainController";
        }

        bool isDrivingMode() const {
            return m_drivingMode;
        }

        void setDrivingMode(bool mode) {
            m_drivingMode = mode;
        }

        glm::vec3 &getLocalFarLeft() {
            return m_localCarLightLeft;
        }

        void setLocalFarLeft(const glm::vec3 &pos) {
            m_localCarLightLeft = pos;
        }

        glm::vec3 &getLocalFarRight() {
            return m_localCarLightRight;
        }

        void setLocalFarRight(const glm::vec3 &pos) {
            m_localCarLightRight = pos;
        }

        glm::vec3 get_m_local_red_pos() const {
            return m_localRedPos;
        }

        void set_m_local_red_pos(const glm::vec3 &m_local_red_pos) {
            m_localRedPos = m_local_red_pos;
        }

        glm::vec3 get_m_local_blue_pos() const {
            return m_localBluePos;
        }

        void set_m_local_blue_pos(const glm::vec3 &m_local_blue_pos) {
            m_localBluePos = m_local_blue_pos;
        }

        bool m_night_vision_mode() const {
            return m_nightVisionMode;
        }

        void set_m_night_vision_mode(bool m_night_vision_mode) {
            m_nightVisionMode = m_night_vision_mode;
        }

        bool m_greyscale_mode() const {
            return m_greyscaleMode;
        }

        void set_m_greyscale_mode(bool m_greyscale_mode) {
            m_greyscaleMode = m_greyscale_mode;
        }
    };
} // app

#endif //MATF_RG_PROJECT_MAINCONTROLLER_HPP
