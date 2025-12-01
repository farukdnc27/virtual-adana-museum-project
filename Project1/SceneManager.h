#pragma once
#include <vector>
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "MuseumObject.h"
#include "Light.h"
//#include "LightCube.h"
#include "ShaderSetup.h"
#include "Frustum.h"

// Forward declaration
class ImGuiManager;

class SceneManager {
private:
    std::vector<std::shared_ptr<MuseumObject>> museumObjects;
    std::vector<Light> lights;
   // std::vector<std::unique_ptr<LightCube>> lightCubes;

    // Sahne ayarlari
    const float LIGHT_ACTIVATION_DISTANCE = 30.0f;
    const float LIGHT_FADE_DISTANCE = 6.0f;

    // Sahne bilgileri
    std::string sceneName;
    glm::vec3 sceneCenter;

    Frustum frustum;
    bool enableFrustumCulling = true; // Frustum culling'i açıp kapatmak için

    ImGuiManager* imguiManager; // Pointer olarak değiştirildi

public:
    SceneManager();
    ~SceneManager();

    // Muze objesi yonetimi
    std::shared_ptr<MuseumObject> AddMuseumObject(const std::string& name, const std::string& description,
        const std::string& modelPath, const std::string& texturePath,
        const glm::vec3& position, const glm::vec3& scale,
        const glm::vec3& rotation);

    void RemoveMuseumObject(const std::string& name);
    MuseumObject* GetMuseumObject(const std::string& name);
    std::vector<MuseumObject*> GetAllMuseumObjects();

    // Isik yonetimi
    void AddLight(const glm::vec3& position, const glm::vec3& color,
        float ambientStrength, float specularStrength,
        float intensity, float range);

    void RemoveLight(int index);
    Light* GetLight(int index) const;
    std::vector<Light>& GetAllLights();

    // Sahne olusturma metodlari
    void LoadAdanaMuseumScene(); // Ana muze sahnesini yukler
   // void LoadTestScene();        // Test sahnesi gerekirse açarız
    void ClearScene();           // Sahneyi temizler

    // Guncelleme ve cizim
    void Update(const glm::vec3& cameraPosition, float deltaTime);
    void Draw(const glm::mat4& view, const glm::mat4& projection,
        const glm::vec3& cameraPosition);

    // Isik yonetimi
    void UpdateLightIntensities(const glm::vec3& cameraPosition);
    void SetupLightsForShaders();

    // Frustum  ayarları
    void EnableFrustumCulling(bool enable) { enableFrustumCulling = enable; }
    bool IsFrustumCullingEnabled() const { return enableFrustumCulling; }

    void SetSceneName(const std::string& name) { sceneName = name; }
    const std::string& GetSceneName() const { return sceneName; }

    void SetSceneCenter(const glm::vec3& center) { sceneCenter = center; }
    const glm::vec3& GetSceneCenter() const { return sceneCenter; }

    // Debug bilgileri
    void PrintSceneInfo();
    int GetObjectCount() const { return museumObjects.size(); }
    int GetLightCount() const { return lights.size(); }
};