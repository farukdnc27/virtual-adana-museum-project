#include "WindowManager.h"
#include <iostream> 

// Singleton instance genelte tek kullndmiz icin bu yapiyi sectim
WindowManager* WindowManager::instance = nullptr;

WindowManager::WindowManager()
	: window(nullptr), windowedWidth(DEFAULT_WIDTH), windowedHeight(DEFAULT_HEIGHT),
	windowedPosX(100), windowedPosY(100), isFullscreen(false),
	windowTitle("Virtual Adana Museum") {
}

WindowManager::~WindowManager() {
	Shutdown();
}

WindowManager& WindowManager::GetInstance() {
	if (instance == nullptr) {
		instance = new WindowManager();
	}
	return *instance;
}

void WindowManager::DestroyInstance() {
	if (instance != nullptr) {
		delete instance;
		instance = nullptr;
	}
}

bool WindowManager::Initialize(const std::string& title) {
	windowTitle = title;

	// GLFWyi baslat
	if (!glfwInit()) {
		std::cerr << "GLFW baslatlamadi!" << std::endl;
		return false;
	}

	// GLFWayarlari
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	// pencereyi olusturma pencere modunda
	window = glfwCreateWindow(windowedWidth, windowedHeight, windowTitle.c_str(), nullptr, nullptr);
	if (window == nullptr) {
		std::cerr << "GLFW penceresi olusturulamadi!" << std::endl;
		glfwTerminate();
		return false;
	}

	// pencere ekranin ortasina yerlestirme kodu
	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
	if (videoMode) { 
		windowedPosX = (videoMode->width - windowedWidth) / 2;
		windowedPosY = (videoMode->height - windowedHeight) / 2;
		glfwSetWindowPos(window, windowedPosX, windowedPosY);
	}
	else {
		//hata durumunda kendi atad���m pozisyana yerle�tirsin diye
		glfwSetWindowPos(window, windowedPosX, windowedPosY);
		std::cerr << "Birincil monit�r video modu alnamadi, pencere varsayilan pozisyonda." << std::endl;
	}


	glfwMakeContextCurrent(window);

	
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
	glfwSetKeyCallback(window, KeyCallback); 

	// mausei sakla 
	glfwSetWindowUserPointer(window, this);

	// pencere cercevesini gizle 
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	std::cout << "WindowManager basarili bir sekilde baslatildi" << std::endl;
	std::cout << "Pencere boyutu: " << windowedWidth << "x" << windowedHeight << std::endl;

	return true;
}

bool WindowManager::InitializeOpenGL() {
	// GLAD'i baslat
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "GLAD baslatlamadi!" << std::endl;
		return false;
	}

	// OpenGL ayarlari
	glEnable(GL_DEPTH_TEST);
	
	int fbWidth, fbHeight;
	glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
	glViewport(0, 0, fbWidth, fbHeight);


	// VSync acildi ve monitorun destekledigi max hze g�re fps veriliyor ofd
	SetVSync(true);

	std::cout << "OpenGL basarili baslatildi" << std::endl;
	std::cout << "OpenGL Version: " << (const char*)glGetString(GL_VERSION) << std::endl;
	std::cout << "Graphics Card: " << (const char*)glGetString(GL_RENDERER) << std::endl;

	return true;
}

void WindowManager::Shutdown() {
	if (window != nullptr) {
		glfwDestroyWindow(window);
		window = nullptr;
	}
	glfwTerminate();
	std::cout << "WindowManager kapatildi" << std::endl;
}

void WindowManager::ToggleFullscreen() {
	SetFullscreen(!isFullscreen);
}

void WindowManager::SetFullscreen(bool fullscreen) {
	if (isFullscreen == fullscreen && window != nullptr) {
		
		return; 
	}

	if (window == nullptr) {
		std::cerr << "Hata: Pencere  null! SetFullscreen islemi yapilamaz." << std::endl;
		return;
	}

	if (fullscreen) {
		// Windowed state'i kaydet
		SaveWindowedState(); 

		// cift monitor varsa ana  monitoru alcak ben cift ekran kullnyporum sikinti yasadim
		GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
		if (!primaryMonitor) {
			std::cerr << "Birincil monitor alinamadi! Tam ekrana gecilemiyor." << std::endl;
			return;
		}
		const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
		if (!videoMode) {
			std::cerr << "Birincil monitor video modu alinamadi! Tam ekrana gecilemiyor." << std::endl;
			return;
		}

		// tam ekrana ge�
		glfwSetWindowMonitor(window, primaryMonitor, 0, 0,
			videoMode->width, videoMode->height, videoMode->refreshRate);

		std::cout << "Tam ekran modu: " << videoMode->width << "x" << videoMode->height << std::endl;
	}
	else {
		// pencere moda geri donme icin yazdim
		// pencere konumunu kayit ettik tekrar onu islem yuk olmamasi adina kullandim
		glfwSetWindowMonitor(window, nullptr, windowedPosX, windowedPosY,
			windowedWidth, windowedHeight, GLFW_DONT_CARE);

		std::cout << "Pencere modu: " << windowedWidth << "x" << windowedHeight << std::endl;
	}

	isFullscreen = fullscreen;

	
	// burda view portta kullanabilirdim glfwGetFramebufferSize kullandim forumlarda daha kullnyl oldugu soyleniyor
	int fbWidth, fbHeight;
	glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
	glViewport(0, 0, fbWidth, fbHeight);
}

void WindowManager::ProcessWindowInput() {
	

	
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	// alt+F4 ile cikis 
	if ((glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS ||
		glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS) &&
		glfwGetKey(window, GLFW_KEY_F4) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void WindowManager::GetWindowSize(int& width, int& height) const {
	if (window) {
		glfwGetWindowSize(window, &width, &height);
	}
	else {
		width = isFullscreen ? 0 : windowedWidth; // veya bir hata durumu
		height = isFullscreen ? 0 : windowedHeight;
	}
}

void WindowManager::SetWindowTitle(const std::string& title) {
	windowTitle = title;
	if (window != nullptr) {
		glfwSetWindowTitle(window, title.c_str());
	}
}

bool WindowManager::ShouldClose() const {
	return window != nullptr && glfwWindowShouldClose(window);
}

void WindowManager::SwapBuffers() {
	if (window != nullptr) {
		glfwSwapBuffers(window);
	}
}

void WindowManager::PollEvents() {
	glfwPollEvents();
}

void WindowManager::SetVSync(bool enabled) {
	glfwSwapInterval(enabled ? 1 : 0);
	std::cout << "VSync: " << (enabled ? "Acik" : "Kapali") << std::endl;
}

void WindowManager::PrintWindowInfo() {
	std::cout << "\n=== PENCERE BILGILERI ===" << std::endl;
	std::cout << "Ba�l�k: " << windowTitle << std::endl;
	std::cout << "Mod: " << (isFullscreen ? "Tam Ekran" : "Pencere") << std::endl;

	int width, height;
	GetWindowSize(width, height); // pencere boyutunu al
	std::cout << "Pencere Boyutu (Window Size): " << width << "x" << height << std::endl;

	if (window) { 
		int fbWidth, fbHeight;
		glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
		std::cout << "Framebuffer Boyutu: " << fbWidth << "x" << fbHeight << std::endl;
	}


	if (!isFullscreen && window) {
		int x, y;
		glfwGetWindowPos(window, &x, &y);
		std::cout << "Pozisyon: " << x << ", " << y << std::endl;
		std::cout << "Kaydedilmis Pencere Modu Boyutu: " << windowedWidth << "x" << windowedHeight << std::endl;
		std::cout << "Kaydedilmis Pencere Modu Pozisyonu: " << windowedPosX << ", " << windowedPosY << std::endl;
	}
	//araya motif gibi ekledim
	std::cout << "%%%%%<<<<<<%%%%%%%>>>>>>>%%%%" << std::endl;
}

void WindowManager::SaveWindowedState() {

	if (!isFullscreen && window) { // Tam ekranda degilsek ve pencere varsa, o anki degerleri kaydet
		glfwGetWindowSize(window, &windowedWidth, &windowedHeight);
		glfwGetWindowPos(window, &windowedPosX, &windowedPosY);
	}
}

void WindowManager::RestoreWindowedState() {
	// gelistirme asamasinda pencere durumunu kaydetmek icin kullandim ancak sonrasinda ihtiyacim kalmadi
	// ihtiya olma ihtimaline karsin silmeidm

}

void WindowManager::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height); // Her zaman framebuffer boyutuna gore viewport ayarla

	WindowManager* windowManager = static_cast<WindowManager*>(glfwGetWindowUserPointer(window));
	if (windowManager) { // windowManager null degilse

		if (!windowManager->isFullscreen) {

		}
	}
}

void WindowManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	WindowManager* windowManager = static_cast<WindowManager*>(glfwGetWindowUserPointer(window));
	if (windowManager == nullptr) return;

	// F11 tusu ile tam ekran gecis
	if ((key == GLFW_KEY_F11 && action == GLFW_PRESS) || (key == GLFW_KEY_F && action == GLFW_PRESS)  ) {
		windowManager->ToggleFullscreen(); 
		
	}

	// ESC ile cikis
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}