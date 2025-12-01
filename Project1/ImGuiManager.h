#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "SceneManager.h"
#include "InputManager.h"
#include <glm/glm.hpp>
#include "Robot.h"
#include <string>
#include <memory>
#include <vector>


class SceneManager;
class MuseumObject;

class ImGuiManager {
private:
	static ImGuiManager* instance;
	GLFWwindow* window;
	bool showDebugWindow = true;
	bool showLightControls = false;
	bool showRobotControls = false;
	bool showArtifactInfo = false;
	bool showFrustumControls = false;
	bool showLightWindow = false;
	// Singleton için private constructor
	ImGuiManager(GLFWwindow* window);

	// light kontrolleri
	float lightIntensity = 1.0f;
	float lightColor[3] = {1.0f, 1.0f, 1.0f}; // Işık rengi için RGB değerleri
	bool showCameraControls = true;
	float cameraSpeed = 2.5f;
	float mouseSensitivity = 0.1f;

	// ayarlar için aç kapa vs
	bool showSettings = true;
	bool showHelp = true;
	bool showDebugInfo = true;

	// çıktı bilgileri
	float fps = 0.0f;
	glm::vec3 cameraPosition = glm::vec3(0.0f);
	bool isMouseLocked = true;

	// rbt bilgileri
	glm::vec3 robotPosition = glm::vec3(0.0f);
	float robotArmAngle = 0.0f;

public:
	// Singleton instance al
	static ImGuiManager& GetInstance(GLFWwindow* window = nullptr);
	static void DestroyInstance();

	// ImGui başlatma ve temizleme
	void Initialize(GLFWwindow* window);
	void Shutdown();

	// frame işlemleri
	void BeginFrame();
	void EndFrame();
	void Render(const SceneManager& sceneManager);

	// ekrana çıktı bilgileri
	void UpdateDebugInfo(float fps, const glm::vec3& cameraPos, bool isMouseLocked);
	void UpdateRobotInfo(const glm::vec3& position, float armAngle);
	void UpdateArtifactInfo(const std::vector<std::shared_ptr<MuseumObject>>& objects, const glm::vec3& cameraPos);
	void ToggleArtifactInfo() { showArtifactInfo = !showArtifactInfo; }
	bool IsArtifactInfoVisible() const { return showArtifactInfo; }

	// Frustum culling kontrolü için yeni fonksiyonlar
	void ToggleFrustumControls() { showFrustumControls = !showFrustumControls; }
	bool IsFrustumControlsVisible() const { return showFrustumControls; }

private:
	void ShowArtifactPopup(const MuseumObject* object);
	void ShowDebugWindow(float fps, const glm::vec3& cameraPos, bool isMouseLocked);
	void ShowLightControls();
	void ShowRobotControls();
	void ShowFrustumControls(SceneManager& sceneManager);
};