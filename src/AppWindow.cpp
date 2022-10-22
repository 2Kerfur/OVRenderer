//========================================================================
// OVRenderer (Open Vulkan Renderer) 
// Version: 0.1 
//------------------------------------------------------------------------
// Copyright (c) 2022-2022 Nagornov Vladimir <vladimirnagornov831@gmail.com>
//========================================================================
#include "AppWindow.h"
#include <stdexcept>

namespace ovr {

	
	AppWindow::AppWindow(int w, int h, std::string name) 
		: width{ w }, height{ h }, windowName{ name } { //puts values in these variables right after initalization
		
		initWindow();
	}

	AppWindow::~AppWindow()
	{
		//destroy glfw resources
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void AppWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto appWindow = reinterpret_cast<AppWindow*>(glfwGetWindowUserPointer(window));
		appWindow->framebufferResized = true;
		appWindow->width = width;
		appWindow->height = height;
	}

	void AppWindow::initWindow()
	{
		glfwInit(); //init GLFW library
		
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); //don't create OpenGL context 
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE); // no resizing

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}
	

	void AppWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface");
		}
	}

}
