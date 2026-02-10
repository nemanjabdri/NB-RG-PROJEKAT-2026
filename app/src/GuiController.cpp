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

        bool isPoliceEmergencyLightsActive = mainCtrl->isPoliceEmergencyLightsActive();
        bool isPoliceHeadLightsActive      = mainCtrl->isPoliceHeadLightsActive();
        bool isDrivingModeActive           = mainCtrl->isDrivingMode();
        bool isNightVisionModeActive       = mainCtrl->m_night_vision_mode();
        bool isGreyscaleModeActive         = mainCtrl->m_greyscale_mode();
        bool isFogModeActive               = mainCtrl->isFogModeActive();
        glm::vec3 carPos                   = mainCtrl->get_car_pos();

        float camDistance   = mainCtrl->get_camera_dist();
        float camHeight     = mainCtrl->get_camera_height();
        float camPitchAngle = mainCtrl->get_pitch_offset();

        float fogStart = mainCtrl->get_fog_start();
        float fogEnd   = mainCtrl->get_fog_end();

        ImGui::Begin("Camera info. ");

        ImGui::Text("Police car position: (%f %f %f)", carPos.x, carPos.y, carPos.z);

        if (ImGui::Checkbox("Third Person Driving Mode ('F1' key)", &isDrivingModeActive)) {
            mainCtrl->setDrivingMode(isDrivingModeActive);
        }

        ImGui::BeginDisabled(!mainCtrl->isDrivingMode());
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
            mainCtrl->setPoliceEmergencyLightsActive(isPoliceEmergencyLightsActive);
        }
        if (ImGui::Checkbox("Police Head Lights ('F' key while Driving Mode is Active)", &isPoliceHeadLightsActive)) {
            mainCtrl->setPoliceHeadLightsActive(isPoliceHeadLightsActive);
        }
        ImGui::EndDisabled();

        if (ImGui::Checkbox("Night Vision Mode", &isNightVisionModeActive)) {
            if (isGreyscaleModeActive) {
                mainCtrl->set_m_greyscale_mode(false);
            }
            mainCtrl->set_m_night_vision_mode(isNightVisionModeActive);
        }
        if (ImGui::Checkbox("Greyscale vision Mode", &isGreyscaleModeActive)) {
            if (isNightVisionModeActive) {
                mainCtrl->set_m_night_vision_mode(false);
            }
            mainCtrl->set_m_greyscale_mode(isGreyscaleModeActive);
        }

        if (ImGui::Checkbox("Fog Mode", &isFogModeActive)) {
            mainCtrl->set_fog_mode(isFogModeActive);
        }
        ImGui::BeginDisabled(!mainCtrl->isFogModeActive());
        if (ImGui::SliderFloat("Fog start", &fogStart, 0.0f, 40.0f)) {
            mainCtrl->set_fog_start(fogStart);
        }

        if (ImGui::SliderFloat("Fog end", &fogEnd, 30.0f, 95.0f)) {
            mainCtrl->set_fog_end(fogEnd);
        }
        ImGui::EndDisabled();

        ImGui::BeginDisabled(!(mainCtrl->getUFOState() == UfoState::SKY_IDLE));
        if (ImGui::Button("UFO Landing")) {
            mainCtrl->startUfoLanding();
        }
        ImGui::EndDisabled();

        ImGui::BeginDisabled(!(mainCtrl->getUFOState() == UfoState::GROUND_IDLE));
        if (ImGui::Button("UFO takeoff")) {
            mainCtrl->startUfoTakeoff();
        }
        ImGui::EndDisabled();

        ImGui::End();

        graphics->end_gui();
    }
} // app
