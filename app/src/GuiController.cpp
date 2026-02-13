//
// Created by nemanja on 3.2.26..
//

#include "../include/GuiController.hpp"

#include <imgui.h>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/platform/PlatformController.hpp>
#include "MainController.hpp"

namespace engine::platform {
    class PlatformController;
}

namespace app {
    class MainController;

    void GuiController::initialize() {
        set_enable(false);
    }

    void GuiController::poll_events() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

        if (platform->key(engine::platform::KEY_F2).state() == engine::platform::Key::State::JustPressed) {
            set_enable(!is_enabled());
        }
    }

    void GuiController::draw() {
        auto graphics        = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto main_controller = engine::core::Controller::get<MainController>();
        graphics->begin_gui();

        bool is_police_emergency_lights_active = main_controller->is_police_emergency_lights_active();
        bool is_police_head_lights_active      = main_controller->is_police_head_lights_active();
        bool is_driving_mode_active            = main_controller->is_driving_mode_active();
        bool is_night_vision_mode_active       = main_controller->is_night_vision_mode_active();
        bool is_greyscale_mode_active          = main_controller->is_greyscale_mode_active();
        bool is_fog_mode_active                = main_controller->is_fog_mode_active();
        glm::vec3 car_pos                      = main_controller->get_car_pos();

        float camera_dist     = main_controller->get_camera_dist();
        float camera_height   = main_controller->get_camera_height();
        float cam_pitch_angle = main_controller->get_pitch_offset();

        float fog_start = main_controller->get_fog_start();
        float fog_end   = main_controller->get_fog_end();

        ImGui::Begin("Camera info. ");

        ImGui::Text("Police car position: (%f %f %f)", car_pos.x, car_pos.y, car_pos.z);

        if (ImGui::Checkbox("Third Person Driving Mode ('F1' key)", &is_driving_mode_active)) {
            main_controller->set_driving_mode(is_driving_mode_active);
        }

        ImGui::BeginDisabled(!main_controller->is_driving_mode_active());
        if (ImGui::SliderFloat("Camera Orbit Radius", &camera_dist, 8.0f, 30.0f)) {
            main_controller->set_camera_dist(camera_dist);
        }
        if (ImGui::SliderFloat("Camera Orbit Height", &camera_height, 3.0f, 30.0f)) {
            main_controller->set_camera_height(camera_height);
        }
        if (ImGui::SliderFloat("Camera Pitch Offset", &cam_pitch_angle, 0.0f, 20.0f)) {
            main_controller->set_pitch_offset(cam_pitch_angle);
        }

        if (ImGui::Checkbox("Police Emergency Lights ('G' key while Driving Mode is Active)",
                            &is_police_emergency_lights_active)) {
            main_controller->set_police_emergency_lights(is_police_emergency_lights_active);
        }
        if (ImGui::Checkbox("Police Head Lights ('F' key while Driving Mode is Active)",
                            &is_police_head_lights_active)) {
            main_controller->set_police_head_lights(is_police_head_lights_active);
        }
        ImGui::EndDisabled();

        if (ImGui::Checkbox("Night Vision Mode", &is_night_vision_mode_active)) {
            if (is_greyscale_mode_active) {
                main_controller->set_greyscale_mode(false);
            }
            main_controller->set_night_vision_mode(is_night_vision_mode_active);
        }
        if (ImGui::Checkbox("Greyscale vision Mode", &is_greyscale_mode_active)) {
            if (is_night_vision_mode_active) {
                main_controller->set_night_vision_mode(false);
            }
            main_controller->set_greyscale_mode(is_greyscale_mode_active);
        }

        if (ImGui::Checkbox("Fog Mode", &is_fog_mode_active)) {
            main_controller->set_fog_mode(is_fog_mode_active);
        }
        ImGui::BeginDisabled(!main_controller->is_fog_mode_active());
        if (ImGui::SliderFloat("Fog start", &fog_start, 0.0f, 40.0f)) {
            main_controller->set_fog_start(fog_start);
        }

        if (ImGui::SliderFloat("Fog end", &fog_end, 30.0f, 95.0f)) {
            main_controller->set_fog_end(fog_end);
        }
        ImGui::EndDisabled();

        ImGui::BeginDisabled(!(main_controller->get_ufo_state() == MainController::UfoState::SKY_IDLE));
        if (ImGui::Button("UFO Landing")) {
            main_controller->start_ufo_landing();
        }
        ImGui::EndDisabled();

        ImGui::BeginDisabled(!(main_controller->get_ufo_state() == MainController::UfoState::GROUND_IDLE));
        if (ImGui::Button("UFO takeoff")) {
            main_controller->start_ufo_takeoff();
        }
        ImGui::EndDisabled();

        ImGui::End();

        graphics->end_gui();
    }
} // app
