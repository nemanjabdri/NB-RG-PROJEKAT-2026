//
// Created by nemanja on 31.1.26..
//

#ifndef MATF_RG_PROJECT_MAINCONTROLLER_HPP
#define MATF_RG_PROJECT_MAINCONTROLLER_HPP
#include <engine/core/Controller.hpp>

namespace app {
    class MainController : public engine::core::Controller {
        void initialize() override;

        bool loop() override;

        void update() override;

        void draw() override;

    public:
        std::string_view name() const override {
            return "MainController";
        }
    };
} // app

#endif //MATF_RG_PROJECT_MAINCONTROLLER_HPP
