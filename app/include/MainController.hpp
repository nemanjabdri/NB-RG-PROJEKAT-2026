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
    public:
        enum class UfoState {
            SKY_IDLE,
            WAITING_TO_LAND,
            DESCENDING,
            GROUND_IDLE,
            WAITING_TO_TAKEOFF,
            ASCENDING
        };

    private:
        float m_total_time                 = 0.0f;
        float m_car_angle                  = 55.0f;
        float m_camera_orbit_angle         = 0.0f;
        float m_camera_dist                = 16.0f;
        float m_camera_height              = 7.0f;
        float m_pitch_offset               = 10.0f;
        float m_fog_start                  = 3.0f;
        float m_fog_end                    = 95.0f;
        const float m_UFO_SKY_Y            = 90.0f;
        const float m_UFO_GROUND_Y         = -4.1f;
        const float m_UFO_SPEED            = 15.0f;
        float m_ufo_rotation               = 0.0f;
        float m_state_timer                = 0.0f;
        float m_red_intensity              = 0.0f;
        float m_blue_intensity             = 0.0f;
        bool m_cursor_enabled              = false;
        bool m_police_emergency_lights     = false;
        bool m_police_head_lights          = false;
        bool m_driving_mode                = false;
        bool m_fog_mode                    = true;
        bool m_night_vision_mode           = false;
        bool m_greyscale_mode              = false;
        bool m_first_entry                 = true;
        bool m_flicker_active              = false;
        UfoState m_ufo_state               = UfoState::SKY_IDLE;
        glm::vec3 m_street_light1_pos      = glm::vec3(18.0f, 20.0, 4.0f);
        glm::vec3 m_street_light2_pos      = glm::vec3(-46.0f, 20.0, 4.0f);
        glm::vec3 m_street_light3_pos      = glm::vec3(55.0f, 20.0, 4.0f);
        glm::vec3 m_street_light_direction = glm::vec3(0.0f, -1.0f, -0.1f);
        glm::vec3 m_local_car_light_left   = glm::vec3(-1.395f, 1.395f, 2.8);
        glm::vec3 m_local_car_light_right  = glm::vec3(1.395f, 1.395f, 2.8);
        glm::vec3 m_local_red_pos          = glm::vec3(0.765f, 3.6f, -1.48f);
        glm::vec3 m_local_blue_pos         = glm::vec3(-0.765f, 3.6f, -1.48f);
        glm::vec3 m_local_spot_dir         = glm::vec3(0.0f, -0.2f, 1.0f);
        glm::vec3 m_world_far_l_pos        = glm::vec3(-1.395f, 1.395f, 2.8);
        glm::vec3 m_world_far_r_pos        = glm::vec3(1.395f, 1.395f, 2.8);
        glm::vec3 m_world_red_pos          = glm::vec3(0.765f, 4.6f, -1.48f);
        glm::vec3 m_world_blue_pos         = glm::vec3(-0.765f, 4.6f, -1.48f);
        glm::vec3 m_car_pos                = glm::vec3(-17.0f, -4.0f, 0.0f);
        glm::vec3 m_camp_fire_light_pos    = glm::vec3(-30.0f, 2.5f, -30.0f);
        glm::vec3 m_animated_fire_pos      = glm::vec3(-30.0f, 2.5f, -30.0f);
        glm::vec3 m_fog_color              = glm::vec3(0.15f, 0.15f, 0.15f);
        glm::vec3 m_ufo_pos                = glm::vec3(-19.0f, m_UFO_SKY_Y, -12.5f);

        void initialize() override;

        bool loop() override;

        void update() override;

        void handle_gui_button();

        void update_ufo(float delta_time);

        void update_car_camera_and_driving(float delta_time);

        void update_free_fly_camera(float delta_time);

        void begin_draw() override;

        void draw() override;

        void end_draw() override;

        void render_scene_geometry(engine::resources::Shader *shader);

        void render_model_geometry(const std::string &model_name, engine::resources::Shader *shader,
                                   glm::vec3 translate_model,
                                   glm::vec3 scale_model = glm::vec3(1.0f), float rotate_model_angle = 0.0f);

        void setup_scene_lights(engine::resources::Shader *shader);

        float calculate_flicker_factor();

        glm::vec3 update_car_light_positions();

        void update_police_light_intensities();

        void draw_skybox();

    public:
        std::string_view name() const override {
            return "MainController";
        }

        bool is_cursor_enabled() const {
            return m_cursor_enabled;
        }

        bool is_police_emergency_lights_active() const {
            return m_police_emergency_lights;
        }

        void set_police_emergency_lights(const bool active) {
            m_police_emergency_lights = active;
        }

        bool is_police_head_lights_active() const {
            return m_police_head_lights;
        }

        void set_police_head_lights(const bool active) {
            m_police_head_lights = active;
        }

        bool is_driving_mode_active() const {
            return m_driving_mode;
        }

        void set_driving_mode(bool mode) {
            m_driving_mode = mode;
        }

        glm::vec3 get_local_far_left() const {
            return m_local_car_light_left;
        }

        void set_local_far_left(const glm::vec3 &pos) {
            m_local_car_light_left = pos;
        }

        glm::vec3 get_local_far_right() const {
            return m_local_car_light_right;
        }

        void set_local_far_right(const glm::vec3 &pos) {
            m_local_car_light_right = pos;
        }

        glm::vec3 get_local_red_pos() const {
            return m_local_red_pos;
        }

        void set_local_red_pos(const glm::vec3 &local_red_pos) {
            m_local_red_pos = local_red_pos;
        }

        glm::vec3 get_local_blue_pos() const {
            return m_local_blue_pos;
        }

        void set_local_blue_pos(const glm::vec3 &local_blue_pos) {
            m_local_blue_pos = local_blue_pos;
        }

        bool is_night_vision_mode_active() const {
            return m_night_vision_mode;
        }

        void set_night_vision_mode(bool night_vision_mode) {
            m_night_vision_mode = night_vision_mode;
        }

        void set_flicker_active(bool flicker_active) {
            m_flicker_active = flicker_active;
        }

        bool is_greyscale_mode_active() const {
            return m_greyscale_mode;
        }

        void set_greyscale_mode(bool greyscale_mode) {
            m_greyscale_mode = greyscale_mode;
        }

        glm::vec3 get_car_pos() const {
            return m_car_pos;
        }

        void set_car_pos(const glm::vec3 &car_pos) {
            m_car_pos = car_pos;
        }

        void start_ufo_landing() {
            if (m_ufo_state == UfoState::SKY_IDLE) {
                m_ufo_state   = UfoState::WAITING_TO_LAND;
                m_state_timer = 0.0f;
            }
        }

        void start_ufo_takeoff() {
            if (m_ufo_state == UfoState::GROUND_IDLE) {
                m_ufo_state   = UfoState::WAITING_TO_TAKEOFF;
                m_state_timer = 0.0f;
            }
        }

        UfoState get_ufo_state() const {
            return m_ufo_state;
        }

        glm::vec3 get_ufo_pos() const {
            return m_ufo_pos;
        }

        float get_ufo_rotation() const {
            return m_ufo_rotation;
        }

        float get_camera_dist() const {
            return m_camera_dist;
        }

        void set_camera_dist(float camera_dist) {
            m_camera_dist = camera_dist;
        }

        float get_camera_height() const {
            return m_camera_height;
        }

        void set_camera_height(float camera_height) {
            m_camera_height = camera_height;
        }

        float get_pitch_offset() const {
            return m_pitch_offset;
        }

        void set_pitch_offset(float pitch_offset) {
            m_pitch_offset = pitch_offset;
        }

        bool is_fog_mode_active() const {
            return m_fog_mode;
        }

        void set_fog_mode(bool fog_mode) {
            m_fog_mode = fog_mode;
        }

        float get_fog_start() const {
            return m_fog_start;
        }

        void set_fog_start(float fog_start) {
            m_fog_start = fog_start;
        }

        float get_fog_end() const {
            return m_fog_end;
        }

        void set_fog_end(float fog_end) {
            m_fog_end = fog_end;
        }
    };
} // app

#endif //MATF_RG_PROJECT_MAINCONTROLLER_HPP
