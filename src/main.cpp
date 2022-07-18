#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <vector>

#include <VulkanRenderer.h>

VulkanRenderer vulkanRenderer;

GLFWwindow * window;

void initWindow(std::string wName = "OVRenderer", const int width = 800, const int height = 600)
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); //disable opengl for GLFW
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(width, height, wName.c_str(), nullptr, nullptr);
}


int main() {
    initWindow("OVRenderer", 800, 600);

    //Create Vulkan renderer instance
    if(vulkanRenderer.init(window) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }


    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }
    vulkanRenderer.cleanup();

    glfwDestroyWindow(window);
    glfwTerminate();
    //
    return 0;
}
