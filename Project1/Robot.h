#pragma once

#include "MuseumObject.h"
#include "Light.h"
#include <glm/glm.hpp>
#include <string>
#include <memory>
#include "Texture.h"

class Robot : public MuseumObject {
public:
    Robot(const std::string& bodyModelPath, const std::string& armModelPath, 
        const glm::vec3& initialPosition = glm::vec3(0.0f));
    ~Robot() = default;

    // Robot hareketi 
    void Move(const glm::vec3& direction, float deltaTime);
    void UpdateArmMovement(float deltaTime);
    void ToggleArmMovement();
    void RotateRobot(float angle); // Robotun kendi ekseni etrafında dönmesi için

    // Kol hareketi stateleri
    enum class ArmState {
        IDLE,
        MOVING_UP,
        MOVING_DOWN
    };

    // Getter'lar
    float GetArmAngle() const { return m_ArmAngle; }
    ArmState GetArmState() const { return m_ArmState; }
    float GetRobotRotation() const { return m_RobotRotation; }
    Light& GetLight() { return m_Light; }
    const Light& GetLight() const { return m_Light; }

    // Draw fonksiyonu override 
    void Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix,
        const glm::vec3& lightPos, const glm::vec3& viewPos) override;

private:
    // Kol modlei için pointer atadsim
    std::unique_ptr<MuseumObject> m_ArmModel;
    
    // Robot ışığı
    Light m_Light;

    // Robotve kol olcekleri
    glm::vec3 m_BodyScale = glm::vec3(0.3f);  // Robot gövdesi için ölçek
    glm::vec3 m_ArmScale = glm::vec3(0.3f);   // Kol için ölçek

    // Robot donusu için variables
    float m_RobotRotation = 0.0f;  // Robotun Y ekseni etrafındaki açısı
    float m_RotationSpeed = 45.0f; // derece/saniye

    // Kol hareketi için variables
    float m_ArmAngle = 0.0f;
    float m_ArmSpeed = 45.0f; // derece/saniye
    ArmState m_ArmState = ArmState::IDLE;
    float m_TargetArmAngle = 0.0f;
    float m_MovementSpeed = 2.5f;
}; 