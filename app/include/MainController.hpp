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
        glm::vec3 m_policeCarLightLeftPos   = glm::vec3(-14.418f, -2.647f, 3.478f);
        glm::vec3 m_policeCarLightRightPos  = glm::vec3(-13.023f, -2.790f, 1.149f);
        glm::vec3 m_policeCarLightDirection = glm::vec3(20.0f, 1.66f, 12.33f);
        glm::vec3 m_streetLight1Pos         = glm::vec3(18.0f, 6.0, 8.0f);
        glm::vec3 m_streetLight2Pos         = glm::vec3(-46.0f, 6.0, 8.0f);
        glm::vec3 m_streetLight3Pos         = glm::vec3(55.0f, 6.0, 8.0f);
        glm::vec3 m_streetLightDirection    = glm::vec3(0.0f, -1.0f, 0.0f);
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
            return m_policeCarLightLeftPos;
        }

        void setPoliceCarLightLeft(const glm::vec3 &pos) {
            m_policeCarLightLeftPos = pos;
        }

        glm::vec3 getPoliceCarLightRight() const {
            return m_policeCarLightRightPos;
        }

        void setPoliceCarLightRight(const glm::vec3 &pos) {
            m_policeCarLightRightPos = pos;
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

        void setStreetLight1Pos(const glm::vec3 &pos) {
            m_streetLight1Pos = pos;
        }

        glm::vec3 getStreetLight1Pos() const {
            return m_streetLight1Pos;
        }

        void setStreetLight2Pos(const glm::vec3 &pos) {
            m_streetLight2Pos = pos;
        }

        glm::vec3 getStreetLight2Pos() const {
            return m_streetLight2Pos;
        }

        void setStreetLight3Pos(const glm::vec3 &pos) {
            m_streetLight3Pos = pos;
        }

        glm::vec3 getStreetLight3Pos() const {
            return m_streetLight3Pos;
        }

        std::string_view name() const override {
            return "MainController";
        }
    };
} // app

#endif //MATF_RG_PROJECT_MAINCONTROLLER_HPP
