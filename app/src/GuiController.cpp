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

        glm::vec3 currentLightDir1 = mainCtrl->getStreetLight1Pos();
        glm::vec3 currentLightDir2 = mainCtrl->getStreetLight2Pos();
        glm::vec3 currentLightDir3 = mainCtrl->getStreetLight3Pos();

        bool isPoliceEmergencyLightsActive = mainCtrl->isPoliceEmergencyLightsActive();
        bool isPoliceHeadLightsActive      = mainCtrl->isPoliceHeadLightsActive();

        ImGui::Begin("Camera info. ");

        ImGui::Text("Camera position: (%f %f %f)", camera->Position.x, camera->Position.y, camera->Position.z);

        if (ImGui::SliderFloat3("Street light 1", &currentLightDir1.x, -20.0f, 20.0f)) {
            mainCtrl->setStreetLight1Pos(currentLightDir1);
        }
        if (ImGui::SliderFloat3("Street light 1", &currentLightDir2.x, -20.0f, 20.0f)) {
            mainCtrl->setStreetLight2Pos(currentLightDir2);
        }
        if (ImGui::SliderFloat3("Street light 1", &currentLightDir3.x, -20.0f, 20.0f)) {
            mainCtrl->setStreetLight3Pos(currentLightDir3);
        }

        if (ImGui::Checkbox("Police Emergency Lights", &isPoliceEmergencyLightsActive)) {
            mainCtrl->setPoliceEmergencyLightsActive(isPoliceEmergencyLightsActive);
        }
        if (ImGui::Checkbox("Police Head Lights", &isPoliceHeadLightsActive)) {
            mainCtrl->setPoliceHeadLightsActive(isPoliceHeadLightsActive);
        }

        ImGui::End();

        graphics->end_gui();
    }
} // app
