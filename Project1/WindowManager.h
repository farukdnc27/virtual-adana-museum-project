#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

class WindowManager {
private:
	GLFWwindow* window;


	int windowedWidth, windowedHeight;
	int windowedPosX, windowedPosY;
	bool isFullscreen;
	bool f11KeyPressed;


	static const int DEFAULT_WIDTH = 1280;
	static const int DEFAULT_HEIGHT = 720;
	std::string windowTitle;

	// Singleton pattern
	static WindowManager* instance;
	WindowManager();

public:
	~WindowManager();


	static WindowManager& GetInstance();
	static void DestroyInstance();


	bool Initialize(const std::string& title = "Virtual Adana Museum");
	bool InitializeOpenGL();
	void Shutdown();

	void ToggleFullscreen();
	void SetFullscreen(bool fullscreen);
	bool IsFullscreen() const { return isFullscreen; }

	void ProcessWindowInput();

	GLFWwindow* GetWindow() const { return window; }
	void GetWindowSize(int& width, int& height) const;
	void SetWindowTitle(const std::string& title);


	static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	bool ShouldClose() const;
	void SwapBuffers();
	void PollEvents();
	void SetVSync(bool enabled);


	void PrintWindowInfo();

private:
	void SaveWindowedState();
	void RestoreWindowedState();
};