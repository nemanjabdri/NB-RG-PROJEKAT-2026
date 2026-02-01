//
// Created by nemanja on 31.1.26..
//

#include "../include/MainController.hpp"

#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>

namespace app {
    class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
    public:
        void on_mouse_move(engine::platform::MousePosition position) override;
    };

    void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
        auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();

        float mouse_sensitivity = 0.04f;

        float xOffset = position.dx * mouse_sensitivity;
        float yOffset = position.dy * mouse_sensitivity;

        camera->rotate_camera(xOffset, yOffset);
    }

    void MainController::initialize() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

        platform->set_enable_cursor(false);

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
        draw_model("house", "shader_house", glm::vec3(0.0f, -4.0f, -15.0f), glm::vec3(0.7f));
        draw_model("convertible", "shader_convertible", glm::vec3(-5.0f, -4.0f, -8.0f), glm::vec3(0.7f));
    }

    void MainController::end_draw() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        platform->swap_buffers();
    }

    void MainController::draw_model(std::string modelName, std::string shaderName, glm::vec3 translateModel,
                                    glm::vec3 scaleModel) {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();

        engine::resources::Model *model   = resources->model(modelName);
        engine::resources::Shader *shader = resources->shader(shaderName);

        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 modelTransform = glm::mat4(1.0f);
        modelTransform           = glm::translate(modelTransform, translateModel);
        modelTransform           = glm::scale(modelTransform, scaleModel);
        shader->set_mat4("model", modelTransform);
        model->draw(shader);
    }
} // app
