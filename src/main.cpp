#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>

class HelloTriangleApplication {
    const uint32_t WIDTH = 800; // window width
    const uint32_t HEIGHT = 600; // window height

    const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    VkInstance instance; // vulkan instance
    GLFWwindow* window;
    void initWindow() { // create GLFW window
        glfwInit(); // initialize GLFW library
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // tell GLFW not to create OpenGL context
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // forbid resizing window
        // Create GLFW window itself
        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    }
    void initVulkan() {
        createInstance();
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) { // do this till window is not closed
            glfwPollEvents(); // Receive input from GLFW window
        }
    }

    void cleanup() {
        vkDestroyInstance(instance, nullptr); // destroy vulkan instance
        glfwDestroyWindow(window); // Destroy visible window
        glfwTerminate(); // Clear all window resources
    }
    void createInstance() {
        VkApplicationInfo appInfo{}; // Struct of app info
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO; // Set type of struct
        appInfo.pApplicationName = "Hello Triangle"; // application name
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0); // App version
        appInfo.pEngineName = "No Engine"; // Engine name
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0); // Engine version
        appInfo.apiVersion = VK_API_VERSION_1_0; // --- Api version ---

        // GLOBAL EXTENSIONS
        VkInstanceCreateInfo createInfo{}; // Struct of instance info
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO; // set type
        createInfo.pApplicationInfo = &appInfo; // pointer to app info

        uint32_t glfwExtensionCount = 0; // count of extensions
        const char** glfwExtensions; // array of extension names

        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount); // get all required extensions for GLFW

        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;
        createInfo.enabledLayerCount = 0; // Debugging extensions count

        VkResult result = vkCreateInstance(&createInfo, nullptr, &instance); // Finally create Vulkan INSTANCE
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create instance!");
        }

        uint32_t extensionCount = 0; //get all supported extensions
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
        std::cout << "available extensions:\n";

        for (const auto& extension : extensions) {
            std::cout << '\t' << extension.extensionName << '\n';
        }
        std::cout << "-------- end ----------" << std::endl;

        //VALIDATION creation
        if (enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }
        std::vector<const char*> Val_extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = Val_extensions.data();

    }
    bool checkValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }

    std::vector<const char*> getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }
};

int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}