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
        auto camera   = graphics->camera();
        graphics->begin_gui();

        bool isPoliceEmergencyLightsActive = mainCtrl->isPoliceEmergencyLightsActive();
        bool isPoliceHeadLightsActive      = mainCtrl->isPoliceHeadLightsActive();
        bool isDrivingModeActive           = mainCtrl->isDrivingMode();
        bool isNightVisionModeActive       = mainCtrl->m_night_vision_mode();
        bool isGreyscaleModeActive         = mainCtrl->m_greyscale_mode();
        glm::vec3 localRedPos              = mainCtrl->get_m_local_red_pos();
        glm::vec3 localBluePos             = mainCtrl->get_m_local_blue_pos();
        glm::vec3 carPos                   = mainCtrl->get_car_pos();

        ImGui::Begin("Camera info. ");

        ImGui::Text("Police car position: (%f %f %f)", carPos.x, carPos.y, carPos.z);

        if (ImGui::SliderFloat3("Car Red light offset", &localRedPos.x, -20.0f, 20.0f)) {
            mainCtrl->set_m_local_red_pos(localRedPos);
        }
        if (ImGui::SliderFloat3("Car Blue light offset", &localBluePos.x, -20.0f, 20.0f)) {
            mainCtrl->set_m_local_blue_pos(localBluePos);
        }

        if (ImGui::Checkbox("Police Emergency Lights ('G' key while Driving Mode is Active)",
                            &isPoliceEmergencyLightsActive)) {
            mainCtrl->setPoliceEmergencyLightsActive(isPoliceEmergencyLightsActive);
        }
        if (ImGui::Checkbox("Police Head Lights ('F' key while Driving Mode is Active)", &isPoliceHeadLightsActive)) {
            mainCtrl->setPoliceHeadLightsActive(isPoliceHeadLightsActive);
        }
        if (ImGui::Checkbox("Third Person Driving Mode", &isDrivingModeActive)) {
            mainCtrl->setDrivingMode(isDrivingModeActive);
        }
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

        if (ImGui::Button("UFO Landing")) {
            mainCtrl->startUfoLanding();
        }

        if (ImGui::Button("UFO takeoff")) {
            mainCtrl->startUfoTakeoff();
        }

        ImGui::End();

        graphics->end_gui();
    }
} // app
