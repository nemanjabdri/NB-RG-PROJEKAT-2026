//
// Created by nemanja on 31.1.26..
//

#ifndef MATF_RG_PROJECT_MAINCONTROLLER_HPP
#define MATF_RG_PROJECT_MAINCONTROLLER_HPP
#include <engine/core/Controller.hpp>
#include <glm/vec3.hpp>

namespace app {
    class MainController : public engine::core::Controller {
    private:
        glm::vec3 m_ufoLightPos = glm::vec3(5.0f, 5.0f, 0.0f);
        bool m_cursorEnabled    = false;

        void initialize() override;

        bool loop() override;

        void update() override;

        void begin_draw() override;

        void draw() override;

        void end_draw() override;

        void draw_skybox();

        void draw_model(std::string modelName, std::string shaderName, glm::vec3 translateModel,
                        glm::vec3 scaleModel = glm::vec3(1.0f), float rotateModelAngle = 0.0f);

    public:
        bool isCursorEnabled() const {
            return m_cursorEnabled;
        }

        glm::vec3 getUfoLightPos() const {
            return m_ufoLightPos;
        }

        void setUfoLightPos(const glm::vec3 &pos) {
            m_ufoLightPos = pos;
        }

        std::string_view name() const override {
            return "MainController";
        }
    };
} // app

#endif //MATF_RG_PROJECT_MAINCONTROLLER_HPP
