#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>

class Frustum {
public:
    Frustum() = default;

    // bu kısmı claud ai tarafına prormasn artması için sordum 
	//oda örüş açısındaki render edilecek nesneleri sadece o an görünenleri render etmemizi sağlıyor 
	// performan artırıyor
	// 
    void Update(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
        glm::mat4 vp = projectionMatrix * viewMatrix;
        
        // Sol düzlem
        planes[0].x = vp[0][3] + vp[0][0];
        planes[0].y = vp[1][3] + vp[1][0];
        planes[0].z = vp[2][3] + vp[2][0];
        planes[0].w = vp[3][3] + vp[3][0];

        // Sağ düzlem
        planes[1].x = vp[0][3] - vp[0][0];
        planes[1].y = vp[1][3] - vp[1][0];
        planes[1].z = vp[2][3] - vp[2][0];
        planes[1].w = vp[3][3] - vp[3][0];

        // Alt düzlem
        planes[2].x = vp[0][3] + vp[0][1];
        planes[2].y = vp[1][3] + vp[1][1];
        planes[2].z = vp[2][3] + vp[2][1];
        planes[2].w = vp[3][3] + vp[3][1];

        // Üst düzlem
        planes[3].x = vp[0][3] - vp[0][1];
        planes[3].y = vp[1][3] - vp[1][1];
        planes[3].z = vp[2][3] - vp[2][1];
        planes[3].w = vp[3][3] - vp[3][1];

        // Yakın düzlem
        planes[4].x = vp[0][3] + vp[0][2];
        planes[4].y = vp[1][3] + vp[1][2];
        planes[4].z = vp[2][3] + vp[2][2];
        planes[4].w = vp[3][3] + vp[3][2];

        // Uzak düzlem
        planes[5].x = vp[0][3] - vp[0][2];
        planes[5].y = vp[1][3] - vp[1][2];
        planes[5].z = vp[2][3] - vp[2][2];
        planes[5].w = vp[3][3] - vp[3][2];

        // Düzlemleri normalize et
        for (auto& plane : planes) {
            float length = glm::length(glm::vec3(plane));
            plane /= length;
        }
    }

    // Bir noktanın görüş alanı içinde olup olmadığını kontrol et
    bool IsPointVisible(const glm::vec3& point) const {
        for (const auto& plane : planes) {
            if (glm::dot(glm::vec3(plane), point) + plane.w < 0) {
                return false;
            }
        }
        return true;
    }

    // Bir kürenin görüş alanı içinde olup olmadığını kontrol et
    bool IsSphereVisible(const glm::vec3& center, float radius) const {
        for (const auto& plane : planes) {
            if (glm::dot(glm::vec3(plane), center) + plane.w < -radius) {
                return false;
            }
        }
        return true;
    }

    // Bir (Axis-Aligned Bounding Box) görüş alanı içinde olup olmadığını kontrol et
    //bu ksıım daha maliyetli dikdörtgenler için verimli  
   /* bool IsAABBVisible(const glm::vec3& min, const glm::vec3& max) const {
        for (const auto& plane : planes) {
            glm::vec3 p(
                plane.x > 0 ? max.x : min.x,
                plane.y > 0 ? max.y : min.y,
                plane.z > 0 ? max.z : min.z
            );
            if (glm::dot(glm::vec3(plane), p) + plane.w < 0) {
                return false;
            }
        }
        return true;
    }*/

private:
    // Düzlem denklemleri (ax + by + cz + d = 0)
    std::array<glm::vec4, 6> planes;
}; 