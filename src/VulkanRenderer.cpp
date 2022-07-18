#include "VulkanRenderer.h"
#include <string>
#include <cstring>

int VulkanRenderer::init(GLFWwindow *newWindow) {
    window = newWindow;

    try {
        createInstance();
        getPhysicalDevice();
    } catch(const std::runtime_error &e)
    {
        printf("ERROR: %s\n", e.what());
        return EXIT_FAILURE;
    }

    return 0;
}

void VulkanRenderer::createInstance() {
    //Information about application itself
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan App"; //custom name of application
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0); //Custom version of app
    appInfo.pEngineName = "No Engine"; //custom engine name
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0); //Custom engine version
    appInfo.apiVersion = VK_API_VERSION_1_0; //Vulkan version



    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo; //p - means pointer

    // Create list to hold instance extentions
    std::vector<const char*> instanceExtensions = std::vector<const char*>();

    uint32_t glfwExtentionCount = 0; //GLFW may require multiple extensions
    const char** glfwExtentions; //Extentions passed as array of cstring, so need pointer (the array)

    // Get GLFW extensions
    glfwExtentions = glfwGetRequiredInstanceExtensions(&glfwExtentionCount);

    // Add glfw extensions to list of extensions

    for (size_t i = 0; i < glfwExtentionCount; i++)
    {
        instanceExtensions.push_back(glfwExtentions[i]);
    }

    if (!checkInstanceExtensionSupport(&instanceExtensions))
    {
        throw std::runtime_error("VkInstance does not support required extensions!");
    }

    createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
    createInfo.ppEnabledExtensionNames = instanceExtensions.data();


    createInfo.enabledLayerCount = 0; //No validation
    createInfo.ppEnabledLayerNames = nullptr;
    //Create instance

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create a Vulkan Instance!");
    }


}

bool VulkanRenderer::checkInstanceExtensionSupport(std::vector<const char *> *checkExtensions) {
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    //Create a list of VkExtensionProperties
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    for (const auto &checkExtension : *checkExtensions)
    {
        bool hasExtension = false;
        for (const auto &extension : extensions)
        {
            if (strcmp(checkExtension, extension.extensionName))
            {
                hasExtension = true;
                break;
            }

        }
        if (!hasExtension)
        {
            return false;
        }
    }
    return true;
}

void VulkanRenderer::cleanup() {
    vkDestroyInstance(instance, nullptr);

}

void VulkanRenderer::getPhysicalDevice() {
    // Enumerate Physical devices the vkInstance and access
    uint32_t  deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0)
    {
        throw std::runtime_error("Can't find GPUs that support Vulkan Instance!");
    }
    // Get list of Physical Devices
    std::vector<VkPhysicalDevice> deviceList(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, deviceList.data());

    mainDeivece.physicalDevice = deviceList[0];
}

bool VulkanRenderer::checkDeviceSuitable(VkPhysicalDevice device) {

    //Info about the device itself (ID, name, type, vendor, etc)
/*    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    //Info about what device can do (geo shader, tess shader, wide lines, etc);

    VkPhysicalDeviceFeatures DeviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &DeviceFeatures);*/


    return true;
}
