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

        glm::vec3 currentUFOLightPos = mainCtrl->getUfoLightPos();

        ImGui::Begin("Camera info. ");

        ImGui::Text("Camera position: (%f %f %f)", camera->Position.x, camera->Position.y, camera->Position.z);

        if (ImGui::SliderFloat3("UFO Light Pos", &currentUFOLightPos.x, -20.0f, 20.0f)) {
            mainCtrl->setUfoLightPos(currentUFOLightPos);
        }

        ImGui::End();

        graphics->end_gui();
    }
} // app
