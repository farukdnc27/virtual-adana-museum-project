#pragma once
#include <glm/glm.hpp>
#include "Shader.h"
#include <string>
#include <iomanip>

class Light {
public:
    // Işık türleri için enum
    enum class Type {
        POINT,
        SPOT,
        DIRECTIONAL
    };

    Light(const glm::vec3& position = glm::vec3(0.0f),
        const glm::vec3& color = glm::vec3(1.0f),
        float ambientStrength = 0.5f,
        float specularStrength = 1.0f,
        float intensity = 1.0f,
        float range = 10.0f,
        Type type = Type::POINT,
        const glm::vec3& direction = glm::vec3(0.0f, -1.0f, 0.0f),
        float spotCutOff = 12.5f,
        float spotOuterCutOff = 17.5f);

    void SetPosition(const glm::vec3& position);
    void SetColor(const glm::vec3& color);
    void SetAmbientStrength(float strength);
    void SetSpecularStrength(float strength);
    void SetIntensity(float intensity);
    void SetRange(float range);
    void SetType(Type type);
    void SetDirection(const glm::vec3& direction);
    void SetSpotCutOff(float cutOff);
    void SetSpotOuterCutOff(float outerCutOff);


    const glm::vec3& GetPosition() const { return m_Position; }
    const glm::vec3& GetColor() const { return m_Color; }
    float GetAmbientStrength() const { return m_AmbientStrength; }
    float GetSpecularStrength() const { return m_SpecularStrength; }
    float GetIntensity() const { return m_Intensity; }
    float GetRange() const { return m_Range; }
    Type GetType() const { return m_Type; }
    const glm::vec3& GetDirection() const { return m_Direction; }
    float GetSpotCutOff() const { return m_SpotCutOff; }
    float GetSpotOuterCutOff() const { return m_SpotOuterCutOff; }

    void ApplyToShader(const Shader& shader) const;
  //  void PrintInfo(const std::string& label = "Light Information") const;

private:
    glm::vec3 m_Position;
    glm::vec3 m_Color;
    float m_AmbientStrength;
    float m_SpecularStrength;
    float m_Intensity;
    float m_Range;
    Type m_Type;
    glm::vec3 m_Direction;
    float m_SpotCutOff;
    float m_SpotOuterCutOff;
};