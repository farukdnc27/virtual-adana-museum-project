#include "InputManager.h"
#include <GLFW/glfw3.h>

void InputManager::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void InputManager::MouseCallback(GLFWwindow* window, double xpos, double ypos) {
	auto& instance = GetInstance();

	if (instance.m_FirstMouse) {
		instance.m_LastX = static_cast<float>(xpos);
		instance.m_LastY = static_cast<float>(ypos);
		instance.m_FirstMouse = false;
	}

	float xoffset = static_cast<float>(xpos) - instance.m_LastX;
	float yoffset = instance.m_LastY - static_cast<float>(ypos);
	instance.m_LastX = static_cast<float>(xpos);
	instance.m_LastY = static_cast<float>(ypos);

	if (instance.m_Camera) {
		instance.m_Camera->ProcessMouseMovement(xoffset, yoffset);
	}
}

void InputManager::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	auto& instance = GetInstance();
	if (instance.m_Camera) {
		instance.m_Camera->ProcessMouseScroll(static_cast<float>(yoffset));
	}
}

// Input processing
void InputManager::ProcessInput(GLFWwindow* window, float deltaTime) {
	if (!m_Camera) return;

	// tab tuşu ile mouse kilitleme kontrolü bu en başta q idi sonra tab yaptım çünkü q robotu döndürüyor
	static bool qKeyPressed = false;
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && !qKeyPressed) {
		ToggleMouseLock(window);
		qKeyPressed = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE) {
		qKeyPressed = false;
	}

	// Mouse kilitli değilse kamera hareketlerini devre dışı bırak yoksa sahne saçmalıyor
	if (!m_MouseLocked) return;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		m_Camera->ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		m_Camera->ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		m_Camera->ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		m_Camera->ProcessKeyboard(RIGHT, deltaTime);
}

void InputManager::ProcessLightInput(GLFWwindow* window, Light& light) {
	glm::vec3 pos = light.GetPosition();
	// const float moveSpeed = 0.06f;

	// hareketli ışık iptal ama belki açarım diye böye bıraktım öfd
	/*
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)    pos.z -= moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)  pos.z += moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)  pos.x -= moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) pos.x += moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)     pos.y += moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) pos.y -= moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)     pos = glm::vec3(6.0f, 6.0f, 0.0f);
	*/

	light.SetPosition(pos);
}

void InputManager::ToggleMouseLock(GLFWwindow* window) {
	SetMouseLocked(window, !m_MouseLocked);
}

void InputManager::SetMouseLocked(GLFWwindow* window, bool locked) {
	m_MouseLocked = locked;
	glfwSetInputMode(window, GLFW_CURSOR,
		m_MouseLocked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

	if (m_MouseLocked) {
		// Mouse kilitlendiğinde ilk tıklama durumunu sıfırla
		m_FirstMouse = true;
	}
}

void InputManager::ProcessRobotInput(GLFWwindow* window, Robot& robot, float deltaTime) {
	// çü üni robotu hareketi için yön tuşları (eto)
	glm::vec3 moveDirection(0.0f);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		moveDirection.z -= 1.0f;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		moveDirection.z += 1.0f;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		moveDirection.x -= 1.0f;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		moveDirection.x += 1.0f;

	// Hareket yönünü normalize et 
	if (glm::length(moveDirection) > 0.0f) {
		moveDirection = glm::normalize(moveDirection);
		robot.Move(moveDirection, deltaTime);
	}

	// Q tuşu ile robot dönüşü   sağa doğru
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		robot.RotateRobot(45.0f * deltaTime); // 45 derece/saniye bunu uygun gördüm yoksa disko topu gibi dönüyor
	}

	// E tuşu ile robot dönüşü sola doğru
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		robot.RotateRobot(-45.0f * deltaTime); // -45 derece/saniye
	}

	// R tuşu ile hem kol hareketi hem de eser bilgisi pop up kontrolü en kolay yol bana göre bu 
	static bool rKeyPressed = false;
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !rKeyPressed) {
		robot.ToggleArmMovement();
		ImGuiManager::GetInstance().ToggleArtifactInfo();
		rKeyPressed = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE) {
		rKeyPressed = false;
	}

	// Kol hareketini güncelle
	robot.UpdateArmMovement(deltaTime);
}