//
// Created by nemanja on 31.1.26..
//

#include "../include/MainController.hpp"

#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>

namespace app {
    void MainController::initialize() {
        engine::graphics::OpenGL::enable_depth_testing();
    }

    bool MainController::loop() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) {
            return false;
        }
        return true;
    }

    void MainController::update() {
        auto platform  = engine::core::Controller::get<engine::platform::PlatformController>();
        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera    = graphics->camera();
        auto deltaTime = platform->dt();

        if (platform->key(engine::platform::KeyId::KEY_W).is_down()) {
            camera->move_camera(engine::graphics::Camera::FORWARD, deltaTime);
        }
        if (platform->key(engine::platform::KeyId::KEY_S).is_down()) {
            camera->move_camera(engine::graphics::Camera::BACKWARD, deltaTime);
        }
        if (platform->key(engine::platform::KeyId::KEY_A).is_down()) {
            camera->move_camera(engine::graphics::Camera::LEFT, deltaTime);
        }
        if (platform->key(engine::platform::KeyId::KEY_D).is_down()) {
            camera->move_camera(engine::graphics::Camera::RIGHT, deltaTime);
        }
        if (platform->key(engine::platform::KeyId::KEY_E).is_down()) {
            camera->move_camera(engine::graphics::Camera::UP, deltaTime);
        }
        if (platform->key(engine::platform::KeyId::KEY_Q).is_down()) {
            camera->move_camera(engine::graphics::Camera::DOWN, deltaTime);
        }
    }

    void MainController::begin_draw() {
        engine::graphics::OpenGL::clear_buffers();
    }

    void MainController::draw() {
        draw_floatPlane();
    }

    void MainController::end_draw() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        platform->swap_buffers();
    }

    void MainController::draw_floatPlane() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();

        engine::resources::Model *modelFloatPlane = resources->model("floatplane");
        engine::resources::Shader *shader         = resources->shader("shader_floatPlane");

        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, glm::vec3(0.0f, -2.0f, -36.0f));
        model           = glm::scale(model, glm::vec3(0.2f));
        shader->set_mat4("model", model);
        modelFloatPlane->draw(shader);
    }
} // app
