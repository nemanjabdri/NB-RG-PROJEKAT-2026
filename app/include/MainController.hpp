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
        float m_totalTime                   = 0.0f;
        glm::vec3 m_policeRedLightPos       = glm::vec3(-17.67f, -0.4f, -1.6f);
        glm::vec3 m_policeBlueLightPos      = glm::vec3(-18.6f, -0.4f, -0.2f);
        glm::vec3 m_policeCarLightLeft      = glm::vec3(-14.418f, -2.647f, 3.478f);
        glm::vec3 m_policeCarLightRight     = glm::vec3(-13.023f, -2.790f, 1.149f);
        glm::vec3 m_policeCarLightDirection = glm::vec3(20.0f, 1.66f, 12.33f);
        bool m_cursorEnabled                = false;
        bool m_policeEmergencyLightsActive  = false;
        bool m_policeHeadLightsActive       = false;

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

        glm::vec3 getPoliceRedLightPos() const {
            return m_policeRedLightPos;
        }

        void setPoliceRedLightPos(const glm::vec3 &pos) {
            m_policeRedLightPos = pos;
        }

        glm::vec3 getPoliceBlueLightPos() const {
            return m_policeBlueLightPos;
        }

        void setPoliceBlueLightPos(const glm::vec3 &pos) {
            m_policeBlueLightPos = pos;
        }

        glm::vec3 getPoliceCarLightLeft() const {
            return m_policeCarLightLeft;
        }

        void setPoliceCarLightLeft(const glm::vec3 &pos) {
            m_policeCarLightLeft = pos;
        }

        glm::vec3 getPoliceCarLightRight() const {
            return m_policeCarLightRight;
        }

        void setPoliceCarLightRight(const glm::vec3 &pos) {
            m_policeCarLightRight = pos;
        }

        glm::vec3 getPoliceCarLightDirection() const {
            return m_policeCarLightDirection;
        }

        void setPoliceCarLightDirection(const glm::vec3 &pos) {
            m_policeCarLightDirection = pos;
        }

        bool isPoliceEmergencyLightsActive() {
            return m_policeEmergencyLightsActive;
        }

        void setPoliceEmergencyLightsActive(const bool active) {
            m_policeEmergencyLightsActive = active;
        }

        bool isPoliceHeadLightsActive() {
            return m_policeHeadLightsActive;
        }

        void setPoliceHeadLightsActive(const bool active) {
            m_policeHeadLightsActive = active;
        }

        std::string_view name() const override {
            return "MainController";
        }
    };
} // app

#endif //MATF_RG_PROJECT_MAINCONTROLLER_HPP
