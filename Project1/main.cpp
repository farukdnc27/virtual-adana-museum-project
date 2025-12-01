#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ImGuiManager.h"
#include "Robot.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include "Shader.h"
//#include "Crosshair.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "WindowManager.h"

// Global değişkenler
Camera camera(glm::vec3(17.0f, 5.0f, 0.0f));
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float fps = 0.0f;
float lastTime = 0.0f;
int frameCount = 0;

// Managersdan çağır
SceneManager sceneManager;
WindowManager& windowManager = WindowManager::GetInstance();
ImGuiManager& imguiManager = ImGuiManager::GetInstance(windowManager.GetWindow());
/*
// crosshair için değişkenler bunu ekranı test için bıraktım
unsigned int crosshairVAO = 0, crosshairVBO = 0;
Shader* crosshairShader = nullptr;
*/

//Robot için variable
std::unique_ptr<Robot> robot;

// konum göstergesi için değişkenler
std::string positionText;
std::string FpsText;

// Global değişkenler bölümüne ekle
static InputManager* g_inputManager = nullptr;

//crosshair için kod var burda
/*
void setupCrosshair() {
	float vertices[] = {
		-0.03f,  0.0f,  // sol çizgi
		 0.03f,  0.0f,
		 0.0f, -0.03f,  // alt çizgi
		 0.0f,  0.03f
	};

	 VAO ve VBO'yu temizle
	if (crosshairVAO != 0) {
		glDeleteVertexArrays(1, &crosshairVAO);
		crosshairVAO = 0;
	}
	if (crosshairVBO != 0) {
		glDeleteBuffers(1, &crosshairVBO);
		crosshairVBO = 0;
	}

	 Yeni VAO ve VBO oluştur
	glGenVertexArrays(1, &crosshairVAO);
	glGenBuffers(1, &crosshairVBO);

	glBindVertexArray(crosshairVAO);
	glBindBuffer(GL_ARRAY_BUFFER, crosshairVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	 Shader'ı yeniden oluştur
	if (crosshairShader != nullptr) {
		delete crosshairShader;
	}
	crosshairShader = new Shader("shaders/crosshairVertex.glsl", "shaders/crosshairFragment.glsl");
}

void drawCrosshair() {
	if (crosshairShader == nullptr || crosshairVAO == 0) {
		std::cerr << "Crosshair henüz başlatılmamış!" << std::endl;
		return;
	}

	glDisable(GL_DEPTH_TEST);
	glLineWidth(3.0f);

	crosshairShader->use();
	glBindVertexArray(crosshairVAO);
	glDrawArrays(GL_LINES, 0, 4);

	glLineWidth(1.0f);
	glEnable(GL_DEPTH_TEST);
}
*/
void updatePositionText(const glm::vec3& position) {
	positionText = "Konum: X: " + std::to_string(position.x) +
		" Y: " + std::to_string(position.y) +
		" Z: " + std::to_string(position.z);
}

void updateFPS(const float& fps) {
	FpsText = "FPS: " + std::to_string(fps);
}

// Callback fonksiyonları
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2((float)xpos, (float)ypos);

	if (!io.WantCaptureMouse && g_inputManager && g_inputManager->IsMouseLocked()) {
		InputManager::MouseCallback(window, xpos, ypos);
	}
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	ImGuiIO& io = ImGui::GetIO();
	if (button >= 0 && button < ImGuiMouseButton_COUNT) {
		io.MouseDown[button] = (action == GLFW_PRESS);
	}

	if (!io.WantCaptureMouse && g_inputManager && g_inputManager->IsMouseLocked()) {

	}
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	ImGuiIO& io = ImGui::GetIO();
	io.MouseWheelH += (float)xoffset;
	io.MouseWheel += (float)yoffset;

	if (!io.WantCaptureMouse && g_inputManager && g_inputManager->IsMouseLocked()) {
		InputManager::ScrollCallback(window, xoffset, yoffset);
	}
}

int main() {
	// wm başlat
	if (!windowManager.Initialize("Virtual Adana Museum")) {
		std::cerr << "WindowManager başlatılamadı!" << std::endl;
		return -1;
	}

	// OpenGL başlat
	if (!windowManager.InitializeOpenGL()) {
		std::cerr << "OpenGL başlatılamadı!" << std::endl;
		return -1;
	}

	// ImGui başlat - pencere oluşturulduktan sonra
	GLFWwindow* window = windowManager.GetWindow();
	imguiManager.Initialize(window);

	// girdileri kur
	InputManager& inputManager = InputManager::GetInstance();
	inputManager.SetCamera(&camera);
	g_inputManager = &inputManager;  // Global pointer'ı ayarla

	// Mouse callback'leri ayarla
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetScrollCallback(window, scrollCallback);

	// Mouse görünürlüğünü ayarla
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// sahne için gerekli classı yükle
	sceneManager.LoadAdanaMuseumScene();
	sceneManager.PrintSceneInfo();

	// Robot'u başlat
	try {
		robot = std::make_unique<Robot>("models/robot/sonrobot.obj", "models/robot/sonkol.obj",
			glm::vec3(-5.0f, 0.0f, 0.0f));
		std::cout << "Robot başarıyla yüklendi!" << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << "Robot yüklenirken hata oluştu: " << e.what() << std::endl;
	}

	// Window bilgilerini yazdır
	windowManager.PrintWindowInfo();

	// crossair başlat
   /* setupCrosshair();
	std::cout << "Crosshair baslatildi" << std::endl;
   */
	std::cout << "Kontroller:" << std::endl;
	std::cout << "F11 - Tam ekran gecisi" << std::endl;
	std::cout << "ESC - Cıkıs" << std::endl;
	std::cout << "WASD - Hareket" << std::endl;
	std::cout << "Mouse - Bakıs acısı" << std::endl;
	std::cout << "Arrow Keys - Isık hareketi" << std::endl;
	std::cout << "Robot Kontrolleri:" << std::endl;
	std::cout << "Ok Tuşları: Hareket" << std::endl;
	std::cout << "Q: Robotu saat yönünde döndür" << std::endl;
	std::cout << "E: Robotu saat yönünün tersine döndür" << std::endl;
	std::cout << "R: Kol hareketini başlat/durdur" << std::endl;

	// asıl döngü
	while (!windowManager.ShouldClose()) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// fps hesaplama
		frameCount++;
		if (currentFrame - lastTime >= 1.0) {
			fps = frameCount / (currentFrame - lastTime);
			frameCount = 0;
			lastTime = currentFrame;
		}

		// input islemleri
		windowManager.ProcessWindowInput();  // F11, ESC
		inputManager.ProcessInput(windowManager.GetWindow(), deltaTime);

		// Robot kontrolü
		if (robot) {
			inputManager.ProcessRobotInput(windowManager.GetWindow(), *robot, deltaTime);
		}

		// light kontrolü 
		Light* mainLight = sceneManager.GetLight(0);
		if (mainLight != nullptr) {
			inputManager.ProcessLightInput(windowManager.GetWindow(), *mainLight);
		}

		// Mouse kilidini kontrol et
		if (inputManager.IsMouseLocked()) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

		// ImGui frame başlat
		imguiManager.BeginFrame();

		// Mouse pozisyonunu güncelle
		ImGuiIO& io = ImGui::GetIO();
		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);
		io.MousePos = ImVec2((float)mouseX, (float)mouseY);

		// Sahneyi guncelle
		sceneManager.Update(camera.Position, deltaTime);

		// dereinlik testi renk ayarı vs 
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// monitore uygun pencere boyutu ile ilgili ayarlar 
		int screenWidth, screenHeight;
		windowManager.GetWindowSize(screenWidth, screenHeight);

		// window boyutlarını kontrol et
		if (screenWidth <= 0 || screenHeight <= 0) {
			std::cerr << "Geçersiz pencere boyutları: " << screenWidth << "x" << screenHeight << std::endl;
			continue; // Bu framei atla
		}

		// aspect ratio kontrol et en boy oranı yani
		float aspect = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
		if (std::abs(aspect) < std::numeric_limits<float>::epsilon()) {
			std::cerr << "Geçersiz aspect ratio: " << aspect << std::endl;
			continue; // Bu framei atla
		}

		// Perspektif matrisini güvenli bir şekilde oluştur
		glm::mat4 projection;
		try {
			projection = glm::perspective(
				glm::radians(45.0f),
				aspect,
				0.1f,
				100.0f
			);
		}
		catch (const std::exception& e) {
			std::cerr << "Perspektif matrisi oluşturulurken hata: " << e.what() << std::endl;
			continue; // Bu frame atla
		}

		// sahhney çizme 
		sceneManager.Draw(camera.GetViewMatrix(), projection, camera.Position);

		// Robot'u çiz
		if (robot) {
			robot->Draw(camera.GetViewMatrix(), projection, sceneManager.GetLight(0)->GetPosition(), camera.Position);
		}

		// imgui render
		imguiManager.UpdateDebugInfo(fps, camera.Position, inputManager.IsMouseLocked());
		imguiManager.Render(sceneManager);

		// imgui robot bilgileri ayarı
		if (robot) {
			imguiManager.UpdateRobotInfo(robot->GetPosition(), robot->GetArmAngle());
		}

		// crossairi çiz
		//int currentWidth, currentHeight;
		//windowManager.GetWindowSize(currentWidth, currentHeight);
		//glViewport(0, 0, currentWidth, currentHeight);
		////drawCrosshair();

		// hata mesajı güncelle
		updatePositionText(camera.Position);
		updateFPS(fps);

		// hatamesaj ekrana yazdır
		std::cout << "\r" << positionText << " | " << FpsText;
		if (windowManager.IsFullscreen()) {
			std::cout << " | [FULLSCREEN]";
		}
		std::cout << std::flush;

		windowManager.SwapBuffers();
		windowManager.PollEvents();
	}

	// Temizlik işlemleri
	try {
		// Önce robot'u temizle
		robot.reset();

		// Crosshair kaynaklarını temizle
		/*
		if (crosshairShader != nullptr) {
			delete crosshairShader;
			crosshairShader = nullptr;
		}
		if (crosshairVAO != 0) {
			glDeleteVertexArrays(1, &crosshairVAO);
			crosshairVAO = 0;
		}
		if (crosshairVBO != 0) {
			glDeleteBuffers(1, &crosshairVBO);
			crosshairVBO = 0;
		}
		*/
		// Son bir frame çiz
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		windowManager.SwapBuffers();

		// ImGuiyi temizle 
		imguiManager.Shutdown();
		ImGuiManager::DestroyInstance();

		// En son window managerı temizle
		WindowManager::DestroyInstance();

		std::cout << "Tüm kaynaklar başarıyla temizlendi" << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << "Temizleme sırasında hata oluştu: " << e.what() << std::endl;
	}

	return 0;
}