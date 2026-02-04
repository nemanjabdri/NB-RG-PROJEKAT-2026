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
        glm::vec3 localCarLightLeft        = mainCtrl->getLocalFarLeft();
        glm::vec3 localCarLightRight       = mainCtrl->getLocalFarRight();

        ImGui::Begin("Camera info. ");

        ImGui::Text("Camera position: (%f %f %f)", camera->Position.x, camera->Position.y, camera->Position.z);

        if (ImGui::SliderFloat3("Car light offset Left", &localCarLightLeft.x, -20.0f, 20.0f)) {
            mainCtrl->setLocalFarLeft(localCarLightLeft);
        }
        if (ImGui::SliderFloat3("Car light offset Right", &localCarLightRight.x, -20.0f, 20.0f)) {
            mainCtrl->setLocalFarRight(localCarLightRight);
        }

        if (ImGui::Checkbox("Police Emergency Lights", &isPoliceEmergencyLightsActive)) {
            mainCtrl->setPoliceEmergencyLightsActive(isPoliceEmergencyLightsActive);
        }
        if (ImGui::Checkbox("Police Head Lights ('F' key while Driving Mode is Active)", &isPoliceHeadLightsActive)) {
            mainCtrl->setPoliceHeadLightsActive(isPoliceHeadLightsActive);
        }
        if (ImGui::Checkbox("Third Person Driving Mode", &isDrivingModeActive)) {
            mainCtrl->setDrivingMode(isDrivingModeActive);
        }

        ImGui::End();

        graphics->end_gui();
    }
} // app
