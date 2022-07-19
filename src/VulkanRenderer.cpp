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
    vkDestroyDevice(mainDevice.logicalDevice, nullptr); //destroy logical device
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

    mainDevice.physicalDevice = deviceList[0]; //pick first device

    for (const auto &device : deviceList) //find device with graphics queue support
    {
        if (checkDeviceSuitable(device))
        {
            mainDevice.physicalDevice = device;
            break;
        }
    }

}

bool VulkanRenderer::checkDeviceSuitable(VkPhysicalDevice device) {

    //Info about the device itself (ID, name, type, vendor, etc)
/*    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    //Info about what device can do (geo shader, tess shader, wide lines, etc);

    VkPhysicalDeviceFeatures DeviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &DeviceFeatures);
    */

    QueueFamilyIndices indices = getQueueFamilies(device);

    return true;
}

QueueFamilyIndices VulkanRenderer::getQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;// variable to put all amount of queue family device supports
    //populate this variable
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    //variable to put supported queue families
    std::vector<VkQueueFamilyProperties> queueFamilyList(queueFamilyCount);
    //populate this vector with result
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyList.data());

    int i = 0; //variable to get index of graphics queue
    // Go through each queue family and check if it has 1 of the required types of queue(graphics)
    for (const auto &queueFamily : queueFamilyList)
    {
        //if > 0 and if has graphics in it
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = i; // if queue is valid put graphics queue index
        }
        if (indices.isValid()) //check if queue family indices are in a valid state, stop searching
        {
            break;
        }

        i ++;

    }
    return QueueFamilyIndices();
}

void VulkanRenderer::createLogicalDevice() {
    //Get the queue family indices for the chosen Physical device
    QueueFamilyIndices indices = getQueueFamilies(mainDevice.physicalDevice);

    //Queue the logical device needs to create and info to do so
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily; // This index of the family to create a queue
    queueCreateInfo.queueCount = 1; //Number of queues to create
    float priority = 1.0f;
    queueCreateInfo.pQueuePriorities = &priority; // Vulkan needs to know how to handle multiple queues


    //info to create logical device (device - means logical device)
    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1; //number of queue create infos
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;  //list of queue create infos so device can create required
    deviceCreateInfo.enabledLayerCount = 0; //Number of enabled logical device extensions
    deviceCreateInfo.ppEnabledExtensionNames = nullptr; //List of enabled logical device

    // Physical device features the logical devices will be using
    VkPhysicalDeviceFeatures deviceFeatures = {};

    deviceCreateInfo.pEnabledFeatures = &deviceFeatures; //Physical device features logical device will use

    //Create the logical device for given physical device
    VkResult result = vkCreateDevice(mainDevice.physicalDevice, &deviceCreateInfo, nullptr, &mainDevice.logicalDevice);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create a logical device!");
    }

    // Queues are created at the same time as the device
    // "graphicsQueue" variable will be used for accessing this queue
    vkGetDeviceQueue(mainDevice.logicalDevice, indices.graphicsFamily, 0, &graphicsQueue);

}
