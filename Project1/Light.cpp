#include "Light.h"

Light::Light(const glm::vec3& position, const glm::vec3& color, float ambientStrength, float specularStrength, float intensity, float range, Type type, const glm::vec3& direction, float spotCutOff, float spotOuterCutOff)
    : m_Position(position)
    , m_Color(color)
    , m_AmbientStrength(ambientStrength)
    , m_SpecularStrength(specularStrength)
    , m_Intensity(intensity)
    , m_Range(range)
    , m_Type(type)
    , m_Direction(direction)
    , m_SpotCutOff(spotCutOff)
    , m_SpotOuterCutOff(spotOuterCutOff)
{
}

void Light::SetPosition(const glm::vec3& position) {
    m_Position = position;
}

void Light::SetColor(const glm::vec3& color) {
    m_Color = color;
}

void Light::SetAmbientStrength(float strength) {
    m_AmbientStrength = strength;
}

void Light::SetSpecularStrength(float strength) {
    m_SpecularStrength = strength;
}

void Light::SetIntensity(float intensity) {
    m_Intensity = intensity;
}

void Light::SetRange(float range) {
    m_Range = range;
}

void Light::SetType(Type type) {
    m_Type = type;
}

void Light::SetDirection(const glm::vec3& direction) {
    m_Direction = direction;
}

void Light::SetSpotCutOff(float cutOff) {
    m_SpotCutOff = cutOff;
}

void Light::SetSpotOuterCutOff(float outerCutOff) {
    m_SpotOuterCutOff = outerCutOff;
}

void Light::ApplyToShader(const Shader& shader) const {
    shader.setVec3("light.position", m_Position);
    shader.setVec3("light.color", m_Color * m_Intensity);
    shader.setFloat("light.ambientStrength", m_AmbientStrength);
    shader.setFloat("light.specularStrength", m_SpecularStrength);
    shader.setFloat("light.range", m_Range);
    
    // Işık türüne göre ek parametreleri ayarla
    shader.setInt("light.type", static_cast<int>(m_Type));
    
    if (m_Type == Type::SPOT || m_Type == Type::DIRECTIONAL) {
        shader.setVec3("light.direction", m_Direction);
    }
    
    if (m_Type == Type::SPOT) {
        shader.setFloat("light.cutOff", glm::cos(glm::radians(m_SpotCutOff)));
        shader.setFloat("light.outerCutOff", glm::cos(glm::radians(m_SpotOuterCutOff)));
    }
}