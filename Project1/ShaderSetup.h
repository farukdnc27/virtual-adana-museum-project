#pragma once
#include "Shader.h"
#include "Light.h"

class ShaderSetup {
public:
    static void SetupLight(const Shader& shader, const Light& light) {
        shader.use();
        SetUniform(shader, "light.position", light.GetPosition());
        SetUniform(shader, "light.color", light.GetColor());
        SetUniform(shader, "light.ambientStrength", light.GetAmbientStrength());
        SetUniform(shader, "light.specularStrength", light.GetSpecularStrength());
        SetUniform(shader, "light.intensity", light.GetIntensity());
        SetUniform(shader, "light.range", light.GetRange());
    }

private:
    // Obu kısım şuan işime yaramıyor
    static void SetUniform(const Shader& shader, const std::string& name, const glm::vec3& value) {
        shader.setVec3(name, value);
    }

    static void SetUniform(const Shader& shader, const std::string& name, float value) {
        shader.setFloat(name, value);
    }

    
};