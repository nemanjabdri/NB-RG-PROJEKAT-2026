//
// Created by nemanja on 31.1.26..
//

#ifndef MATF_RG_PROJECT_MAINCONTROLLER_HPP
#define MATF_RG_PROJECT_MAINCONTROLLER_HPP
#include <engine/core/Controller.hpp>
#include <glm/vec3.hpp>

namespace app {
    class MainController : public engine::core::Controller {
        void initialize() override;

        bool loop() override;

        void update() override;

        void begin_draw() override;

        void draw() override;

        void end_draw() override;

        void draw_model(std::string modelName, std::string shaderName, glm::vec3 translateModel, glm::vec3 scaleModel);

    public:
        std::string_view name() const override {
            return "MainController";
        }
    };
} // app

#endif //MATF_RG_PROJECT_MAINCONTROLLER_HPP
