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

namespace {
    const std::string SHADER_UNIVERSAL   = "shader_model_universal";
    const std::string SHADER_SHADOW      = "shader_point_shadow_depth";
    const std::string SHADER_SKYBOX      = "shader_skybox";
    const std::string MODEL_TERRAIN      = "terrain";
    const std::string SKYBOX             = "skybox";
    const std::string MODEL_UFO          = "UFO";
    const std::string MODEL_HOUSE        = "house";
    const std::string MODEL_CONVERTIBLE  = "convertible";
    const std::string MODEL_ROAD         = "road";
    const std::string MODEL_CARAVAN      = "caravan";
    const std::string MODEL_POLICE_CAR   = "police_car";
    const std::string MODEL_FARM_HOUSE   = "farm_house";
    const std::string MODEL_TENNIS_COURT = "tennis_court";
    const std::string MODEL_STREET_LIGHT = "street_light";
    const std::string MODEL_BUILDING     = "building";
    const std::string MODEL_BAKERY       = "bakery";
    const std::string MODEL_BANK         = "bank";
    const std::string MODEL_CINEMA       = "cinema";
    const std::string MODEL_SALOON       = "saloon";
    const std::string MODEL_CAMPFIRE     = "campfire";
    const std::string MODEL_CACTUS       = "cactus";
    const std::string MODEL_LAWN_MOWER   = "lawn_mower";
    const std::string MODEL_WOOD_SWING   = "wood_swing";
}

namespace app {
    class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
    public:
        void on_mouse_move(engine::platform::MousePosition position) override;
    };

    void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
        auto camera          = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
        auto main_controller = engine::core::Controller::get<app::MainController>();

        if (main_controller->is_cursor_enabled() || main_controller->is_driving_mode_active()) {
            return;
        }

        float mouse_sensitivity = 0.04f;

        float x_offset = position.dx * mouse_sensitivity;
        float y_offset = position.dy * mouse_sensitivity;

        camera->rotate_camera(x_offset, y_offset);
    }

    void MainController::initialize() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

        m_cursor_enabled = false;
        platform->set_enable_cursor(m_cursor_enabled);

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

    void MainController::update_ufo(float delta_time) {
        if (m_ufo_state != UfoState::GROUND_IDLE && m_ufo_state != UfoState::WAITING_TO_TAKEOFF) {
            m_ufo_rotation += 200.0f * delta_time;
        }

        switch (m_ufo_state) {
        case UfoState::WAITING_TO_LAND: m_state_timer += delta_time;
            if (m_state_timer >= 2.0f) {
                m_ufo_state = UfoState::DESCENDING;
            }
            break;

        case UfoState::DESCENDING: m_ufo_pos.y -= m_UFO_SPEED * delta_time;
            // Provera da li smo dotakli tlo
            if (m_ufo_pos.y <= m_UFO_GROUND_Y) {
                m_ufo_pos.y = m_UFO_GROUND_Y;
                m_ufo_state = UfoState::GROUND_IDLE;
            }
            break;

        case UfoState::WAITING_TO_TAKEOFF: m_state_timer += delta_time;
            if (m_state_timer >= 2.0f) {
                m_ufo_state = UfoState::ASCENDING;
            }
            break;

        case UfoState::ASCENDING: m_ufo_pos.y += m_UFO_SPEED * delta_time;
            // Provera da li smo stigli na nebo
            if (m_ufo_pos.y >= m_UFO_SKY_Y) {
                m_ufo_pos.y = m_UFO_SKY_Y;
                m_ufo_state = UfoState::SKY_IDLE;
            }
            break;

        default: break;
        }
    }

    void MainController::update_car_camera_and_driving(float delta_time) {
        auto platform    = engine::core::Controller::get<engine::platform::PlatformController>();
        auto graphics    = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera      = graphics->camera();
        float move_speed = 15.0f * delta_time;
        float rot_speed  = 80.0f * delta_time;

        if (m_first_entry) {
            m_camera_orbit_angle = 0.0f;
            m_first_entry        = false;
        }

        float cam_rot_speed = 60.0f * delta_time;

        if (platform->key(engine::platform::KeyId::KEY_LEFT).is_down()) {
            m_camera_orbit_angle -= cam_rot_speed;
        }
        if (platform->key(engine::platform::KeyId::KEY_RIGHT).is_down()) {
            m_camera_orbit_angle += cam_rot_speed;
        }

        float rotation_direction = 1.0f;

        if (platform->key(engine::platform::KeyId::KEY_S).is_down() &&
            !platform->key(engine::platform::KeyId::KEY_W).is_down()) {
            rotation_direction = -1.0f;
        }

        if (platform->key(engine::platform::KeyId::KEY_A).is_down()) {
            m_car_angle += rot_speed * rotation_direction;
        }
        if (platform->key(engine::platform::KeyId::KEY_D).is_down()) {
            m_car_angle -= rot_speed * rotation_direction;
        }

        // Izračunavanje smera (Forward Vector)
        float rad = glm::radians(m_car_angle);
        glm::vec3 forward;
        forward.x = sin(rad);
        forward.y = 0.0f;
        forward.z = cos(rad);
        forward   = glm::normalize(forward);

        if (platform->key(engine::platform::KeyId::KEY_W).is_down()) {
            m_car_pos += forward * move_speed;
        }
        if (platform->key(engine::platform::KeyId::KEY_S).is_down()) {
            m_car_pos -= forward * move_speed;
        }

        float final_angle = m_car_angle + m_camera_orbit_angle - glm::pi<float>();
        rad               = glm::radians(final_angle);
        glm::vec3 camera_pos;
        camera_pos.x = m_car_pos.x - sin(rad) * m_camera_dist;
        camera_pos.z = m_car_pos.z - cos(rad) * m_camera_dist;
        camera_pos.y = m_car_pos.y + m_camera_height;

        camera->Position = camera_pos;

        glm::vec3 direction = glm::normalize((m_car_pos + glm::vec3(0.0f, 1.0f, 0.0f)) - camera->Position);
        float target_yaw    = glm::degrees(atan2(direction.z, direction.x));
        float target_pitch  = glm::degrees(asin(direction.y));

        float x_offset = target_yaw - camera->Yaw;
        float y_offset = target_pitch - camera->Pitch;

        while (x_offset <= -180.0f)
            x_offset += 360.0f;
        while (x_offset > 180.0f)
            x_offset -= 360.0f;

        if (m_car_angle >= 360.0f)
            m_car_angle -= 360.0f;
        if (m_car_angle < 0.0f)
            m_car_angle += 360.0f;

        camera->rotate_camera(x_offset, y_offset + m_pitch_offset);

        if (platform->key(engine::platform::KeyId::KEY_F).state() == engine::platform::Key::State::JustPressed) {
            set_police_head_lights(!m_police_head_lights);
        }
        if (platform->key(engine::platform::KeyId::KEY_G).state() == engine::platform::Key::State::JustPressed) {
            set_police_emergency_lights(!m_police_emergency_lights);
        }
        if (platform->key(engine::platform::KeyId::KEY_F1).state() == engine::platform::Key::State::JustPressed) {
            set_driving_mode(false);
        }
    }

    void MainController::update_free_fly_camera(float delta_time) {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera   = graphics->camera();

        m_first_entry = true;

        if (!m_cursor_enabled) {
            if (platform->key(engine::platform::KeyId::KEY_W).is_down()) {
                camera->move_camera(engine::graphics::Camera::FORWARD, delta_time);
            }
            if (platform->key(engine::platform::KeyId::KEY_S).is_down()) {
                camera->move_camera(engine::graphics::Camera::BACKWARD, delta_time);
            }
            if (platform->key(engine::platform::KeyId::KEY_A).is_down()) {
                camera->move_camera(engine::graphics::Camera::LEFT, delta_time);
            }
            if (platform->key(engine::platform::KeyId::KEY_D).is_down()) {
                camera->move_camera(engine::graphics::Camera::RIGHT, delta_time);
            }
            if (platform->key(engine::platform::KeyId::KEY_E).is_down()) {
                camera->move_camera(engine::graphics::Camera::UP, delta_time);
            }
            if (platform->key(engine::platform::KeyId::KEY_Q).is_down()) {
                camera->move_camera(engine::graphics::Camera::DOWN, delta_time);
            }
        }
        if (platform->key(engine::platform::KeyId::KEY_F1).state() == engine::platform::Key::State::JustPressed) {
            set_driving_mode(true);
        }
    }

    void MainController::handle_gui_button() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        if (platform->key(engine::platform::KeyId::KEY_F2).state() == engine::platform::Key::State::JustPressed) {
            m_cursor_enabled = !m_cursor_enabled;
            platform->set_enable_cursor(m_cursor_enabled);
        }
    }

    void MainController::update() {
        auto platform   = engine::core::Controller::get<engine::platform::PlatformController>();
        auto delta_time = platform->dt();
        m_total_time    += delta_time;

        handle_gui_button();
        update_ufo(delta_time);

        if (m_driving_mode) {
            update_car_camera_and_driving(delta_time);
        } else {
            update_free_fly_camera(delta_time);
        }

        float fire_speed = 2.0f;
        float fire_noise = (sin(m_total_time * fire_speed) * 0.2f) + (sin(m_total_time * fire_speed * 2.1f) * 0.1f);
        m_animated_fire_pos = m_camp_fire_light_pos;
        m_animated_fire_pos.y += fire_noise;
    }

    void MainController::begin_draw() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();

        auto shader_depth = resources->shader(SHADER_SHADOW);

        shader_depth->use();
        graphics->bind_point_shadow(shader_depth, m_animated_fire_pos);
        render_scene_geometry(shader_depth);
        graphics->unbind_point_shadow();

        glFinish();

        if (m_night_vision_mode || m_greyscale_mode) {
            graphics->bind_frameBuffer();
        }

        glClearColor(m_fog_color.r, m_fog_color.g, m_fog_color.b, 1.0f);
        engine::graphics::OpenGL::clear_buffers();
    }

    void MainController::draw() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();

        auto shader_universal = resources->shader(SHADER_UNIVERSAL);
        shader_universal->use();
        shader_universal->set_mat4("projection", graphics->projection_matrix());
        shader_universal->set_mat4("view", graphics->camera()->view_matrix());

        setup_scene_lights(shader_universal);
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_CUBE_MAP, graphics->point_shadow_texture_id());
        shader_universal->set_int("depthMap", 10);
        shader_universal->set_float("far_plane", 45.0f);
        shader_universal->set_bool("fogEnabled", m_fog_mode);
        shader_universal->set_vec3("fogColor", m_fog_color);
        shader_universal->set_float("fogStart", m_fog_start);
        shader_universal->set_float("fogEnd", m_fog_end);
        shader_universal->set_int("shadows", 2);

        render_scene_geometry(shader_universal);
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        glActiveTexture(GL_TEXTURE0);

        if (!m_fog_mode) {
            draw_skybox();
        } else {
            glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
            glDepthMask(GL_FALSE);
            draw_skybox();
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            glDepthMask(GL_TRUE);
        }

        if (m_night_vision_mode || m_greyscale_mode) {
            graphics->unbind_frameBuffer();
            auto shader = m_night_vision_mode
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
        render_model_geometry(MODEL_TERRAIN, shader, glm::vec3(0.0f, -0.55f, -80.0f),
                              glm::vec3(0.0005f, 0.0008f, 0.0008f));
        render_model_geometry(MODEL_TERRAIN, shader, glm::vec3(0.0f, -0.55f, 60.0f),
                              glm::vec3(0.0005f, 0.0008f, 0.0008f), 180.0f);
        render_model_geometry(MODEL_HOUSE, shader, glm::vec3(0.0f, -4.0f, -25.0f));
        render_model_geometry(MODEL_CONVERTIBLE, shader, glm::vec3(-6.0f, -4.0f, -17.0f));
        render_model_geometry(MODEL_ROAD, shader, glm::vec3(0.0f, -4.7f, 0.1f), glm::vec3(2.0f, 0.1f, 0.25f));
        render_model_geometry(MODEL_CARAVAN, shader, glm::vec3(-17.0f, -0.6f, -33.0f), glm::vec3(0.045f), 20.0f);
        render_model_geometry(MODEL_POLICE_CAR, shader, m_car_pos, glm::vec3(0.09f), m_car_angle);
        render_model_geometry(MODEL_FARM_HOUSE, shader, glm::vec3(0.0f, -4.4f, 18.0f), glm::vec3(0.8f), 180.0f);
        render_model_geometry(MODEL_TENNIS_COURT, shader, glm::vec3(36.0f, -4.0f, -22.0f), glm::vec3(1.6f));
        render_model_geometry(MODEL_STREET_LIGHT, shader, glm::vec3(18.0f, -4.0f, 8.0f), glm::vec3(1.5f), 90.0f);
        render_model_geometry(MODEL_STREET_LIGHT, shader, glm::vec3(-46.0f, -4.0f, 8.0f), glm::vec3(1.5f),
                              90.0f);
        render_model_geometry(MODEL_STREET_LIGHT, shader, glm::vec3(55.0f, -3.9f, 8.0f), glm::vec3(1.5f),
                              90.0f);
        render_model_geometry(MODEL_BUILDING, shader, glm::vec3(-30.0f, -4.0f, 12.0f), glm::vec3(1.3f),
                              180.0f);
        render_model_geometry(MODEL_BAKERY, shader, glm::vec3(-55.0f, -4.0f, -12.0f), glm::vec3(1.0f),
                              0.0f);
        render_model_geometry(MODEL_UFO, shader, m_ufo_pos, glm::vec3(0.1f),
                              m_ufo_rotation);
        render_model_geometry(MODEL_BANK, shader, glm::vec3(66.0f, -4.0f, -17.0f), glm::vec3(0.04f),
                              -90.0f);
        render_model_geometry(MODEL_CINEMA, shader, glm::vec3(95.0f, -4.0f, -13.0f), glm::vec3(1.0f),
                              0.0f);
        render_model_geometry(MODEL_SALOON, shader, glm::vec3(25.0f, -4.0f, 16.0f), glm::vec3(1.0f),
                              180.0f);
        render_model_geometry(MODEL_CAMPFIRE, shader, glm::vec3(-30.0f, -3.5f, -30.0f), glm::vec3(1.5f),
                              0.0f);
        render_model_geometry(MODEL_CACTUS, shader, glm::vec3(-30.0f, -4.2f, -40.0f), glm::vec3(0.3f),
                              0.0f);
        render_model_geometry(MODEL_CACTUS, shader, glm::vec3(-22.0f, -4.2f, -26.0f), glm::vec3(0.25f),
                              45.0f);
        render_model_geometry(MODEL_CACTUS, shader, glm::vec3(-30.0f, -4.2f, -21.0f), glm::vec3(0.25f),
                              20.0f);
        render_model_geometry(MODEL_LAWN_MOWER, shader, glm::vec3(-38.0f, -4.2f, -24.0f), glm::vec3(0.05f),
                              45.0f);
        render_model_geometry(MODEL_WOOD_SWING, shader, glm::vec3(-37.0f, -4.2f, -40.0f), glm::vec3(0.04f),
                              45.0f);
    }

    void MainController::render_model_geometry(const std::string &model_name, engine::resources::Shader *shader,
                                               glm::vec3 translate_model,
                                               glm::vec3 scale_model, float rotate_model_angle) {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();

        engine::resources::Model *model = resources->model(model_name);

        glm::mat4 model_transform = glm::mat4(1.0f);
        model_transform = glm::translate(model_transform, translate_model);
        model_transform = glm::rotate(model_transform, glm::radians(rotate_model_angle), glm::vec3(0.0f, 1.0f, 0.0f));
        model_transform = glm::scale(model_transform, scale_model);
        shader->set_mat4("model", model_transform);

        model->draw(shader);
    }

    void MainController::setup_scene_lights(engine::resources::Shader *shader) {
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

        glm::mat4 car_rot = glm::rotate(glm::mat4(1.0f), glm::radians(m_car_angle), glm::vec3(0, 1, 0));

        m_world_red_pos   = glm::vec3(car_rot * glm::vec4(m_local_red_pos, 1.0f)) + m_car_pos;
        m_world_blue_pos  = glm::vec3(car_rot * glm::vec4(m_local_blue_pos, 1.0f)) + m_car_pos;
        m_world_far_l_pos = glm::vec3(car_rot * glm::vec4(m_local_car_light_left, 1.0f)) + m_car_pos;
        m_world_far_r_pos = glm::vec3(car_rot * glm::vec4(m_local_car_light_right, 1.0f)) + m_car_pos;

        glm::vec3 world_spot_dir = glm::vec3(car_rot * glm::vec4(m_local_spot_dir, 0.0f));

        float speed = 5.0f;
        float m_red_intensity;
        float m_blue_intensity;

        if (m_police_emergency_lights) {
            m_red_intensity  = (sin(m_total_time * speed) + 1.0f) / 2.0f;
            m_blue_intensity = (cos(m_total_time * speed) + 1.0f) / 2.0f;
        } else {
            m_red_intensity  = 0.0f;
            m_blue_intensity = 0.0f;
        }
        float ambient_lights_active     = m_police_emergency_lights ? 1.0f : 0.0f;
        float police_head_lights_active = m_police_head_lights ? 1.0f : 0.0f;

        shader->use();

        shader->set_vec3("viewPos", graphics->camera()->Position);

        //POLICE RED LIGHT
        shader->set_vec3("pointLights[0].position", m_world_red_pos);
        shader->set_vec3("pointLights[0].ambient", glm::vec3(0.02f) * ambient_lights_active);
        shader->set_vec3("pointLights[0].diffuse", glm::vec3(1.0f, 0.0f, 0.0f) * m_red_intensity);
        shader->set_vec3("pointLights[0].specular", glm::vec3(0.0f));
        shader->set_float("pointLights[0].linear", 0.009f);
        shader->set_float("pointLights[0].quadratic", 0.0032f);

        //POLICE BLUE LIGHT
        shader->set_vec3("pointLights[1].position", m_world_blue_pos);
        shader->set_vec3("pointLights[1].ambient", glm::vec3(0.02f) * ambient_lights_active);
        shader->set_vec3("pointLights[1].diffuse", glm::vec3(0.0f, 0.0f, 1.0f) * m_blue_intensity);
        shader->set_vec3("pointLights[1].specular", glm::vec3(0.0f));
        shader->set_float("pointLights[1].linear", 0.009f);
        shader->set_float("pointLights[1].quadratic", 0.0032f);

        float fire_speed     = 2.0f;
        float fire_noise     = (sin(m_total_time * fire_speed) * 0.2f) + (sin(m_total_time * fire_speed * 2.1f) * 0.1f);
        float fire_intensity = fire_noise + 0.7f;
        float base_linear    = 0.009f;
        float base_quadratic = 0.0032f;
        //Camp fire
        shader->set_vec3("pointLights[2].position", m_animated_fire_pos);
        shader->set_vec3("pointLights[2].ambient", glm::vec3(0.005f));
        shader->set_vec3("pointLights[2].diffuse", glm::vec3(1.0f, 0.6f, 0.2f) * 1.5f * fire_intensity);
        shader->set_vec3("pointLights[2].specular", glm::vec3(0.5f) * fire_intensity);
        shader->set_float("pointLights[2].linear", base_linear / fire_intensity);
        shader->set_float("pointLights[2].quadratic", base_quadratic / fire_intensity);

        glm::vec3 ufo_light_pos = glm::vec3(m_ufo_pos.x, m_ufo_pos.y + 6, m_ufo_pos.z);

        //UFO cockpit LIGHT
        shader->set_vec3("pointLights[3].position", ufo_light_pos);
        shader->set_vec3("pointLights[3].ambient", glm::vec3(0.03f));
        shader->set_vec3("pointLights[3].diffuse", glm::vec3(0.75f, 0.0f, 1.0f) * 2.5f);
        shader->set_vec3("pointLights[3].specular", glm::vec3(0.2f));
        shader->set_float("pointLights[3].linear", 0.09f);
        shader->set_float("pointLights[3].quadratic", 0.032f);

        // Far Levi
        shader->set_vec3("spotLights[0].position", m_world_far_l_pos);
        shader->set_vec3("spotLights[0].direction", world_spot_dir);
        shader->set_float("spotLights[0].cutOff", glm::cos(glm::radians(14.5f)));
        shader->set_float("spotLights[0].outerCutOff", glm::cos(glm::radians(22.5f)));
        shader->set_vec3("spotLights[0].ambient", glm::vec3(0.001f) * police_head_lights_active);
        shader->set_vec3("spotLights[0].diffuse", glm::vec3(1.0f, 1.0f, 0.4f) * police_head_lights_active);
        shader->set_vec3("spotLights[0].specular", glm::vec3(0.5f) * police_head_lights_active);
        shader->set_float("spotLights[0].linear", 0.0009f);
        shader->set_float("spotLights[0].quadratic", 0.00032f);

        // Far Desni
        shader->set_vec3("spotLights[1].position", m_world_far_r_pos);
        shader->set_vec3("spotLights[1].direction", world_spot_dir);
        shader->set_float("spotLights[1].cutOff", glm::cos(glm::radians(14.5f)));
        shader->set_float("spotLights[1].outerCutOff", glm::cos(glm::radians(22.5f)));
        shader->set_vec3("spotLights[1].ambient", glm::vec3(0.001f) * police_head_lights_active);
        shader->set_vec3("spotLights[1].diffuse", glm::vec3(1.0f, 1.0f, 0.4f) * police_head_lights_active);
        shader->set_vec3("spotLights[1].specular", glm::vec3(0.5f) * police_head_lights_active);
        shader->set_float("spotLights[1].linear", 0.009f);
        shader->set_float("spotLights[1].quadratic", 0.0032f);

        //street-light 1
        shader->set_vec3("spotLights[2].position", m_street_light1_pos);
        shader->set_vec3("spotLights[2].direction", m_street_light_direction);
        shader->set_float("spotLights[2].cutOff", glm::cos(glm::radians(12.5f)));
        shader->set_float("spotLights[2].outerCutOff", glm::cos(glm::radians(35.5f)));
        shader->set_vec3("spotLights[2].ambient", glm::vec3(0.1f));
        shader->set_vec3("spotLights[2].diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        shader->set_vec3("spotLights[2].specular", glm::vec3(0.3f));
        shader->set_float("spotLights[2].linear", 0.009f);
        shader->set_float("spotLights[2].quadratic", 0.0032f);
        //street-light 2
        shader->set_vec3("spotLights[3].position", m_street_light2_pos);
        shader->set_vec3("spotLights[3].direction", m_street_light_direction);
        shader->set_float("spotLights[3].cutOff", glm::cos(glm::radians(12.5f)));
        shader->set_float("spotLights[3].outerCutOff", glm::cos(glm::radians(35.5f)));
        shader->set_vec3("spotLights[3].ambient", glm::vec3(0.1f));
        shader->set_vec3("spotLights[3].diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        shader->set_vec3("spotLights[3].specular", glm::vec3(0.3f));
        shader->set_float("spotLights[3].linear", 0.009f);
        shader->set_float("spotLights[3].quadratic", 0.0032f);

        float flicker_threshold = 0.85f;
        float random_val        = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        float street_light_flicker;
        if (random_val > flicker_threshold) {
            // Nagli pad ili potpuni mrak
            street_light_flicker = (random_val > 0.95f) ? 0.0f : 0.2f;
        } else {
            // Normalno svetlo sa blagim šumom
            street_light_flicker = 0.9f + (random_val * 0.1f);
        }

        //street-light 3
        shader->set_vec3("spotLights[4].position", m_street_light3_pos);
        shader->set_vec3("spotLights[4].direction", m_street_light_direction);
        shader->set_float("spotLights[4].cutOff", glm::cos(glm::radians(12.5f)));
        shader->set_float("spotLights[4].outerCutOff", glm::cos(glm::radians(35.5f)));
        shader->set_vec3("spotLights[4].ambient", glm::vec3(0.1f));
        shader->set_vec3("spotLights[4].diffuse", glm::vec3(1.0f, 1.0f, 1.0f) * street_light_flicker);
        shader->set_vec3("spotLights[4].specular", glm::vec3(0.3f) * street_light_flicker);
        shader->set_float("spotLights[4].linear", 0.009f);
        shader->set_float("spotLights[4].quadratic", 0.0032f);
    }

    void MainController::draw_skybox() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto skybox    = resources->skybox(SKYBOX);
        auto shader    = resources->shader(SHADER_SKYBOX);
        graphics->draw_skybox(shader, skybox);
    }
} // app
