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
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto mainCtrl = engine::core::Controller::get<MainController>();
        graphics->begin_gui();

        bool isPoliceEmergencyLightsActive = mainCtrl->is_police_emergency_lights_active();
        bool isPoliceHeadLightsActive      = mainCtrl->is_police_head_lights_active();
        bool isDrivingModeActive           = mainCtrl->is_driving_mode_active();
        bool isNightVisionModeActive       = mainCtrl->is_night_vision_mode_active();
        bool isGreyscaleModeActive         = mainCtrl->is_greyscale_mode_active();
        bool isFogModeActive               = mainCtrl->is_fog_mode_active();
        glm::vec3 carPos                   = mainCtrl->get_car_pos();

        float camDistance   = mainCtrl->get_camera_dist();
        float camHeight     = mainCtrl->get_camera_height();
        float camPitchAngle = mainCtrl->get_pitch_offset();

        float fogStart = mainCtrl->get_fog_start();
        float fogEnd   = mainCtrl->get_fog_end();

        ImGui::Begin("Camera info. ");

        ImGui::Text("Police car position: (%f %f %f)", carPos.x, carPos.y, carPos.z);

        if (ImGui::Checkbox("Third Person Driving Mode ('F1' key)", &isDrivingModeActive)) {
            mainCtrl->set_driving_mode(isDrivingModeActive);
        }

        ImGui::BeginDisabled(!mainCtrl->is_driving_mode_active());
        if (ImGui::SliderFloat("Camera Orbit Radius", &camDistance, 8.0f, 30.0f)) {
            mainCtrl->set_camera_dist(camDistance);
        }
        if (ImGui::SliderFloat("Camera Orbit Height", &camHeight, 3.0f, 30.0f)) {
            mainCtrl->set_camera_height(camHeight);
        }
        if (ImGui::SliderFloat("Camera Pitch Offset", &camPitchAngle, 0.0f, 20.0f)) {
            mainCtrl->set_pitch_offset(camPitchAngle);
        }

        if (ImGui::Checkbox("Police Emergency Lights ('G' key while Driving Mode is Active)",
                            &isPoliceEmergencyLightsActive)) {
            mainCtrl->set_police_emergency_lights(isPoliceEmergencyLightsActive);
        }
        if (ImGui::Checkbox("Police Head Lights ('F' key while Driving Mode is Active)", &isPoliceHeadLightsActive)) {
            mainCtrl->set_police_head_lights(isPoliceHeadLightsActive);
        }
        ImGui::EndDisabled();

        if (ImGui::Checkbox("Night Vision Mode", &isNightVisionModeActive)) {
            if (isGreyscaleModeActive) {
                mainCtrl->set_greyscale_mode(false);
            }
            mainCtrl->set_night_vision_mode(isNightVisionModeActive);
        }
        if (ImGui::Checkbox("Greyscale vision Mode", &isGreyscaleModeActive)) {
            if (isNightVisionModeActive) {
                mainCtrl->set_night_vision_mode(false);
            }
            mainCtrl->set_greyscale_mode(isGreyscaleModeActive);
        }

        if (ImGui::Checkbox("Fog Mode", &isFogModeActive)) {
            mainCtrl->set_fog_mode(isFogModeActive);
        }
        ImGui::BeginDisabled(!mainCtrl->is_fog_mode_active());
        if (ImGui::SliderFloat("Fog start", &fogStart, 0.0f, 40.0f)) {
            mainCtrl->set_fog_start(fogStart);
        }

        if (ImGui::SliderFloat("Fog end", &fogEnd, 30.0f, 95.0f)) {
            mainCtrl->set_fog_end(fogEnd);
        }
        ImGui::EndDisabled();

        ImGui::BeginDisabled(!(mainCtrl->get_ufo_state() == MainController::UfoState::SKY_IDLE));
        if (ImGui::Button("UFO Landing")) {
            mainCtrl->start_ufo_landing();
        }
        ImGui::EndDisabled();

        ImGui::BeginDisabled(!(mainCtrl->get_ufo_state() == MainController::UfoState::GROUND_IDLE));
        if (ImGui::Button("UFO takeoff")) {
            mainCtrl->start_ufo_takeoff();
        }
        ImGui::EndDisabled();

        ImGui::End();

        graphics->end_gui();
    }
} // app
