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
        glm::vec3 m_carPos                 = glm::vec3(-17.0f, -4.0f, 0.0f);
        glm::vec3 m_campFireLightPos       = glm::vec3(-30.0f, 0.4f, -30.0f);
        glm::vec3 m_fogColor               = glm::vec3(0.15f, 0.15f, 0.15f);
        glm::vec3 m_ufoPos                 = glm::vec3(6.0f, m_UFO_SKY_Y, -13.0f);
        UfoState m_ufoState                = UfoState::SKY_IDLE;
        bool m_cursorEnabled               = false;
        bool m_policeEmergencyLightsActive = false;
        bool m_policeHeadLightsActive      = false;
        bool m_drivingMode                 = false;
        bool m_fogMode                     = true;
        bool m_nightVisionMode             = false;
        bool m_greyscaleMode               = false;
        bool m_firstEntry                  = true;
        float m_totalTime                  = 0.0f;
        float m_carAngle                   = 55.0f;
        float m_cameraOrbitAngle           = 0.0f;
        float m_cameraDist                 = 16.0f;
        float m_cameraHeight               = 7.0f;
        float m_pitch_offset               = 10.0f;
        float m_fogStart                   = 3.0f;
        float m_fogEnd                     = 95.0f;
        const float m_UFO_SKY_Y            = 90.0f;
        const float m_UFO_GROUND_Y         = -3.8f;
        const float m_UFO_SPEED            = 15.0f;
        float m_ufoRotation                = 0.0f;
        float m_stateTimer                 = 0.0f;

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
        enum class UfoState {
            SKY_IDLE,
            WAITING_TO_LAND,
            DESCENDING, GROUND_IDLE,
            WAITING_TO_TAKEOFF,
            ASCENDING
        };

        std::string_view name() const override {
            return "MainController";
        }

        bool is_cursor_enabled() const {
            return m_cursorEnabled;
        }

        bool is_police_emergency_lights_active() {
            return m_policeEmergencyLightsActive;
        }

        void set_police_emergency_lights(const bool active) {
            m_policeEmergencyLightsActive = active;
        }

        bool is_police_head_lights_active() {
            return m_policeHeadLightsActive;
        }

        void set_police_head_lights(const bool active) {
            m_policeHeadLightsActive = active;
        }

        bool is_driving_mode_active() const {
            return m_drivingMode;
        }

        void set_driving_mode(bool mode) {
            m_drivingMode = mode;
        }

        glm::vec3 &get_local_far_left() {
            return m_localCarLightLeft;
        }

        void set_local_far_left(const glm::vec3 &pos) {
            m_localCarLightLeft = pos;
        }

        glm::vec3 &get_local_far_right() {
            return m_localCarLightRight;
        }

        void set_local_far_right(const glm::vec3 &pos) {
            m_localCarLightRight = pos;
        }

        glm::vec3 get_local_red_pos() const {
            return m_localRedPos;
        }

        void set_local_red_pos(const glm::vec3 &m_local_red_pos) {
            m_localRedPos = m_local_red_pos;
        }

        glm::vec3 get_local_blue_pos() const {
            return m_localBluePos;
        }

        void set_local_blue_pos(const glm::vec3 &m_local_blue_pos) {
            m_localBluePos = m_local_blue_pos;
        }

        bool is_night_vision_mode_active() const {
            return m_nightVisionMode;
        }

        void set_night_vision_mode(bool m_night_vision_mode) {
            m_nightVisionMode = m_night_vision_mode;
        }

        bool is_greyscale_mode_active() const {
            return m_greyscaleMode;
        }

        void set_greyscale_mode(bool m_greyscale_mode) {
            m_greyscaleMode = m_greyscale_mode;
        }

        glm::vec3 get_car_pos() const {
            return m_carPos;
        }

        void set_car_pos(const glm::vec3 &m_car_pos) {
            m_carPos = m_car_pos;
        }

        void start_ufo_landing() {
            if (m_ufoState == UfoState::SKY_IDLE) {
                m_ufoState   = UfoState::WAITING_TO_LAND;
                m_stateTimer = 0.0f;
            }
        }

        void start_ufo_takeoff() {
            if (m_ufoState == UfoState::GROUND_IDLE) {
                m_ufoState   = UfoState::WAITING_TO_TAKEOFF;
                m_stateTimer = 0.0f;
            }
        }

        UfoState get_ufo_state() {
            return m_ufoState;
        }

        glm::vec3 get_ufo_pos() const {
            return m_ufoPos;
        }

        float get_ufo_rotation() const {
            return m_ufoRotation;
        }

        float get_camera_dist() const {
            return m_cameraDist;
        }

        void set_camera_dist(float m_camera_dist) {
            m_cameraDist = m_camera_dist;
        }

        float get_camera_height() const {
            return m_cameraHeight;
        }

        void set_camera_height(float m_camera_height) {
            m_cameraHeight = m_camera_height;
        }

        float get_pitch_offset() const {
            return m_pitch_offset;
        }

        void set_pitch_offset(float pitch_offset) {
            m_pitch_offset = pitch_offset;
        }

        bool is_fog_mode_active() const {
            return m_fogMode;
        }

        void set_fog_mode(bool fog_mode) {
            m_fogMode = fog_mode;
        }

        float get_fog_start() const {
            return m_fogStart;
        }

        void set_fog_start(float fog_start) {
            m_fogStart = fog_start;
        }

        float get_fog_end() const {
            return m_fogEnd;
        }

        void set_fog_end(float fog_end) {
            m_fogEnd = fog_end;
        }
    };
} // app

#endif //MATF_RG_PROJECT_MAINCONTROLLER_HPP
