//
// Created by nemanja on 3.2.26..
//

#ifndef MATF_RG_PROJECT_GUICONTROLLER_HPP
#define MATF_RG_PROJECT_GUICONTROLLER_HPP
#include <engine/core/Controller.hpp>

namespace app {
class GuiController : public engine::core::Controller {
public:
    std::string_view name() const override {
        return "GuiController";
    }

private:
    void initialize() override;

    void poll_events() override;

    void draw() override;
};
}// namespace app

#endif//MATF_RG_PROJECT_GUICONTROLLER_HPP
