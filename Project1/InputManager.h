#pragma once

#include "Camera.h"
#include "Light.h"
#include "Robot.h"
#include "ImGuiManager.h"

// camera sınıfı ile şeleştirdim burdaki değişekneleri ortak olması için
enum CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class InputManager {
public:
	// Singleton pattern tek birdefa oluşturulması için 
    // birde bu yapıyı biraz daha iyi öğrendiğim için  sık kullandım
    static InputManager& GetInstance() {
        static InputManager instance;
        return instance;
    }

	// callbaek  fonksiyonları
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    // inputlariçin proses
    void ProcessInput(GLFWwindow* window, float deltaTime);
    void ProcessLightInput(GLFWwindow* window, Light& light);
    void ProcessRobotInput(GLFWwindow* window, Robot& robot, float deltaTime);

    // kamra control
    void SetCamera(Camera* camera) { m_Camera = camera; }
    Camera* GetCamera() const { return m_Camera; }

    // Mouse control
    void ResetMouseState() { m_FirstMouse = true; }

    // Mouse kilitleme kontrolü bunda sahne kilitlemek zor oldu
    bool IsMouseLocked() const { return m_MouseLocked; }
    void ToggleMouseLock(GLFWwindow* window);
    void SetMouseLocked(GLFWwindow* window, bool locked);

private:
    InputManager() : m_FirstMouse(true), m_LastX(0), m_LastY(0), m_Camera(nullptr), m_MouseLocked(true) {}
    InputManager(const InputManager&) = delete;
    void operator=(const InputManager&) = delete;

    // mouse anlık durum için değişkenler
    bool m_FirstMouse;
    float m_LastX;
    float m_LastY;

    // kamera ref 
    Camera* m_Camera;

    bool m_MouseLocked = true; // Başlangıçta mouse kilitli gelmesi için yazdım
};