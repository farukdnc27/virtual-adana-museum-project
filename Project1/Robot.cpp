#include "Robot.h"
#include <iostream>

Robot::Robot(const std::string& bodyModelPath, const std::string& armModelPath, 
    const glm::vec3& initialPosition)
    : MuseumObject("Robot", "Müze Robotu", bodyModelPath, "", initialPosition, m_BodyScale, glm::vec3(0.0f, 90.0f, 0.0f)),
      m_Light(initialPosition + glm::vec3(0.0f, 1.0f, 0.0f), // Işık pozisyonu robotun üstünde
            glm::vec3(1.0f, 0.9f, 0.8f), // Sıcak beyaz ışık rengi
            0.3f, // ambient strength
            1.0f, // specular strength
            1.5f, // Intensity
            15.0f, // range
            Light::Type::SPOT, // Spot ışık
            glm::vec3(0.0f, -1.0f, 0.0f), // Asagi dogru yön
            15.0f, // spot cut off
            25.0f), // spot outer cut off
      m_ArmAngle(0.0f), m_ArmState(ArmState::IDLE), m_RobotRotation(90.0f) {
    
    // blendersiz olan robot kolu yükledim (eto)
    try {
        m_ArmModel = std::make_unique<MuseumObject>("Robot Kol", "Robot Kolu", 
            armModelPath, "", initialPosition, m_ArmScale, glm::vec3(0.0f, 90.0f, 0.0f));
    }
    catch (const std::exception& e) {
        std::cerr << "Robot kolu yüklenemedi: " << e.what() << std::endl;
    }
}

void Robot::Move(const glm::vec3& direction, float deltaTime) {
    // Robotun yönüne göre hareket vektörünü döndür (90 derece offset ile)
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(m_RobotRotation), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 rotatedDirection = glm::vec3(rotationMatrix * glm::vec4(direction, 0.0f));
    
    glm::vec3 newPosition = GetPosition() + rotatedDirection * m_MovementSpeed * deltaTime;
    SetPosition(newPosition);
    
    // Işık pozisyonunuda update et
    m_Light.SetPosition(newPosition + glm::vec3(0.0f, 1.0f, 0.0f));
    
    // Kol modelinin pozisyonunu da update et
    if (m_ArmModel) {
        m_ArmModel->SetPosition(newPosition);
    }
}

void Robot::RotateRobot(float angle) {
    m_RobotRotation += angle;
    if (m_RobotRotation >= 360.0f) {
        m_RobotRotation -= 360.0f;
    }
    else if (m_RobotRotation < 0.0f) {
        m_RobotRotation += 360.0f;
    }
    
    // Işık yönünü de udapte et
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 newDirection = glm::vec3(rotationMatrix * glm::vec4(m_Light.GetDirection(), 0.0f));
    m_Light.SetDirection(newDirection);
}

void Robot::UpdateArmMovement(float deltaTime) {
    switch (m_ArmState) {
        case ArmState::MOVING_UP:
            m_ArmAngle += m_ArmSpeed * deltaTime;
            if (m_ArmAngle >= 45.0f) {
                m_ArmAngle = 45.0f;
                m_ArmState = ArmState::MOVING_DOWN;
                m_TargetArmAngle = -45.0f;
            }
            break;

        case ArmState::MOVING_DOWN:
            m_ArmAngle -= m_ArmSpeed * deltaTime;
            if (m_ArmAngle <= 0.0f) {
                m_ArmAngle = 0.0f;
                m_ArmState = ArmState::IDLE;
            }
            break;

        case ArmState::IDLE:
            // Kol hareketsiz 
            break;
    }

    // Kol modelinin rotasyonunu update et (Z ekseni )
    if (m_ArmModel) {
        m_ArmModel->SetRotation(glm::vec3(0.0f, 0.0f, m_ArmAngle));
    }
}

void Robot::ToggleArmMovement() {
    if (m_ArmState == ArmState::IDLE) {
        m_ArmState = ArmState::MOVING_UP;
        m_TargetArmAngle = 0.0f;
    }
}

void Robot::Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix,
    const glm::vec3& lightPos, const glm::vec3& viewPos) {
    
    // Robot gövdesi için model matrisini oluştur (90 derece offset ile)
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, GetPosition());
    modelMatrix = glm::rotate(modelMatrix, glm::radians(m_RobotRotation), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, m_BodyScale);
    
    // shaderı kullan ve uniformları set et
    const Shader& shader = GetShader();
    shader.use();
    shader.setMat4("model", modelMatrix);
    shader.setMat4("view", viewMatrix);
    shader.setMat4("projection", projectionMatrix);
    
    // Robotun kendi ışığını shader'a uygula
    m_Light.ApplyToShader(shader);
    
    shader.setVec3("viewPos", viewPos);

    // Robot gövdesini çiz
    for (const auto& mesh : GetMeshes()) {
        // Materyal ayarlarını uygula
        const Material& material = mesh.material;
        shader.setVec3("material.ambient", material.ambient);
        shader.setVec3("material.diffuse", material.diffuse);
        shader.setVec3("material.specular", material.specular);
        shader.setFloat("material.shininess", material.shininess);
        shader.setFloat("material.brightness", 1.0f); // Parlaklığı artır

        // Texture bağlama
        if (!mesh.material.diffuseMap.empty()) {
            const Texture* texture = ResourceManager::GetInstance().GetTexture(mesh.material.diffuseMap);
            if (texture) {
                glActiveTexture(GL_TEXTURE0);
                texture->Bind(0);
                shader.setInt("material.diffuseMap", 0);
                shader.setBool("material.hasDiffuseMap", true);
            } else {
                std::cerr << "Texture yüklenemedi: " << mesh.material.diffuseMap << std::endl;
                shader.setBool("material.hasDiffuseMap", false);
            }
        } else {
            shader.setBool("material.hasDiffuseMap", false);
        }

        // Mesh'i çiz
        glBindVertexArray(mesh.VAO);
        glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Texture'ı cleanle
        if (!mesh.material.diffuseMap.empty()) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    // Kol modeli için model matrisini oluştur
    if (m_ArmModel) {
        glm::mat4 armModelMatrix = glm::mat4(1.0f);
        armModelMatrix = glm::translate(armModelMatrix, GetPosition());
        armModelMatrix = glm::rotate(armModelMatrix, glm::radians(m_RobotRotation), glm::vec3(0.0f, 1.0f, 0.0f));
        armModelMatrix = glm::rotate(armModelMatrix, glm::radians(m_ArmAngle), glm::vec3(0.0f, 0.0f, 1.0f));
        armModelMatrix = glm::scale(armModelMatrix, m_ArmScale);

        // Kol modelini çiz
        const Shader& armShader = m_ArmModel->GetShader();
        armShader.use();
        armShader.setMat4("model", armModelMatrix);
        armShader.setMat4("view", viewMatrix);
        armShader.setMat4("projection", projectionMatrix);
        
        // Robotun ışığını kol shaderınada uygula
        m_Light.ApplyToShader(armShader);
        
        armShader.setVec3("viewPos", viewPos);

        for (const auto& mesh : m_ArmModel->GetMeshes()) {
            const Material& material = mesh.material;
            armShader.setVec3("material.ambient", material.ambient);
            armShader.setVec3("material.diffuse", material.diffuse);
            armShader.setVec3("material.specular", material.specular);
            armShader.setFloat("material.shininess", material.shininess);
            armShader.setFloat("material.brightness", 1.0f); // Parlaklığı artır

            // Texture bağlama
            if (!mesh.material.diffuseMap.empty()) {
                const Texture* texture = ResourceManager::GetInstance().GetTexture(mesh.material.diffuseMap);
                if (texture) {
                    glActiveTexture(GL_TEXTURE0);
                    texture->Bind(0);
                    armShader.setInt("material.diffuseMap", 0);
                    armShader.setBool("material.hasDiffuseMap", true);
                } else {
                    std::cerr << "Texture yüklenemedi: " << mesh.material.diffuseMap << std::endl;
                    armShader.setBool("material.hasDiffuseMap", false);
                }
            } else {
                armShader.setBool("material.hasDiffuseMap", false);
            }

            glBindVertexArray(mesh.VAO);
            glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            // Texture'ı cleanla
            if (!mesh.material.diffuseMap.empty()) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
        }
    }
} 