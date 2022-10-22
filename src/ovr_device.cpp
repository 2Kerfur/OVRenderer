//========================================================================
// OVRenderer (Open Vulkan Renderer) 
// Version: 0.1 
//------------------------------------------------------------------------
// Copyright (c) 2022-2022 Nagornov Vladimir <vladimirnagornov831@gmail.com>
//========================================================================
#include "ovr_device.h"

// std headers
#include <cstring>
#include <iostream>
#include <set>
#include <unordered_set>

namespace ovr {

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback( // local debug callback creation 
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, // message importance
    VkDebugUtilsMessageTypeFlagsEXT messageType, // message type
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, // Structure specifying parameters returned to the callback
    void *pUserData) {
    
  std::cerr << "validation layer: " << "messageSeverity - " << messageSeverity << " callbackData" << pCallbackData->pMessage << std::endl;

  return VK_FALSE;
}


VkResult CreateDebugUtilsMessengerEXT( // Debug messeger creation
    VkInstance instance, //vulkan instance
    const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, //Structure specifying parameters of a newly created debug messenger
    const VkAllocationCallbacks *pAllocator, //Structure containing callback function pointers for memory allocation
    VkDebugUtilsMessengerEXT *pDebugMessenger) { // Messenger object which handles passing along debug messages to a provided debug callback.
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr( // load function
      instance, //vulkan instance
      "vkCreateDebugUtilsMessengerEXT"); //What to create
  if (func != nullptr) { // function successfully loaded
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger); // Create DebugUtilsMessengerEXT
  } else { // function couldn't be loaded
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void DestroyDebugUtilsMessengerEXT( // Destroy debug messeger
    VkInstance instance, // vulkan instance
    VkDebugUtilsMessengerEXT debugMessenger, // debug messenger
    const VkAllocationCallbacks *pAllocator) { // debug messenger allocator
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr( // load destroy function
      instance, //vk instance
      "vkDestroyDebugUtilsMessengerEXT"); //what to destroy
  if (func != nullptr) { // if succesfully loaded -> delete
    func(instance, debugMessenger, pAllocator); //delete debug messenger
  }
}

// class member functions
OVRDevice::OVRDevice(AppWindow &window) : window{window} {
  createInstance(); //init vulkan lib
  setupDebugMessenger(); //setup validation layers
  createSurface(); // create surface connection between window and vulkan
  pickPhysicalDevice(); // setup gpu
  createLogicalDevice(); // what features will be used
  createCommandPool(); // command buffer alocation settup
}

OVRDevice::~OVRDevice() {
  vkDestroyCommandPool(device_, commandPool, nullptr); //destroy vulkan command pool
  vkDestroyDevice(device_, nullptr); // destroy vulkan device

  if (enableValidationLayers) { // if validation layer (debug messenger) enabled -> destroy it
    DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
  }

  vkDestroySurfaceKHR(instance, surface_, nullptr); //destroy connection between Vulkan and the native surface
  vkDestroyInstance(instance, nullptr); // destroy vulkan instance
}

void OVRDevice::createInstance() {
  if (enableValidationLayers && !checkValidationLayerSupport()) { // if validation layers not supported print error
    throw std::runtime_error("validation layers requested, but not available!");
  }

  VkApplicationInfo appInfo = {}; // Application Info structure
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "OVRenerer";
  appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
  appInfo.pEngineName = "No Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;


  VkInstanceCreateInfo createInfo = {}; // Vulkan instance Info structure
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;
  
  auto extensions = getRequiredExtensions(); // get all requred extensions
  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size()); // put how many extensions used
  createInfo.ppEnabledExtensionNames = extensions.data(); // put all extensions names


  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo; // setup debug messenger
  if (enableValidationLayers) {
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size()); // put how many debug layers used
    createInfo.ppEnabledLayerNames = validationLayers.data(); //put all extensions names

    populateDebugMessengerCreateInfo(debugCreateInfo); //setup what types of messeges messager will show
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;  //pointer to a structure extending this structure.
  } else { 
    createInfo.enabledLayerCount = 0;
    createInfo.pNext = nullptr;
  }

  if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) { // create vulkan instance
    throw std::runtime_error("failed to create instance!");
  }

  hasGflwRequiredInstanceExtensions(); // check if all glfw extensions supported
}

void OVRDevice::pickPhysicalDevice() { // pick Video Card from list of avaliable 
    uint32_t deviceCount = 0; //avaliable devices count
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
    throw std::runtime_error("Failed to find GPUs with Vulkan support!"); 
    }
  std::cout << "Device count: " << deviceCount << std::endl;
  std::vector<VkPhysicalDevice> devices(deviceCount); //vector of avaliable physical devices
  //get list of avaliable devices
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

  for (const auto &device : devices) { // go through avaliable devices
      if (isDeviceSuitable(device)) { // check if suits requirements
         physicalDevice = device;
      
         break;
    }
  }

  if (physicalDevice == VK_NULL_HANDLE) {
    throw std::runtime_error("failed to find a suitable GPU!");
  }

  vkGetPhysicalDeviceProperties(physicalDevice, &properties);
  std::cout << "physical device: " << properties.deviceName << std::endl;
  std::cout << "device api version: "     << properties.apiVersion << std::endl;
  std::cout << "device ID: " << properties.deviceID << std::endl;
  switch (properties.deviceType) {
  case 0:
      std::cout << "device type: " << "VK_PHYSICAL_DEVICE_TYPE_OTHER" << std::endl;
      break;
  case 1:
      std::cout << "device type: " << "VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU " << std::endl;
      break;
  case 2:
      std::cout << "device type: " << "VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU" << std::endl;
      break;
  case 3:
      std::cout << "device type: " << "VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU " << std::endl;
      break;
  case 4:
      std::cout << "device type: " << "VK_PHYSICAL_DEVICE_TYPE_CPU " << std::endl;
      break;
  }
  std::cout << "device vendorID: " << properties.vendorID << std::endl;
  std::cout << "device driverVersion: " << properties.driverVersion << std::endl;
  //VkPhysicalDeviceLimits device_limits = properties.limits;
  //VkPhysicalDeviceSparseProperties sparce_properties = properties.sparseProperties;

}

void OVRDevice::createLogicalDevice() { // create vulkan logical device
  QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily};

  float queuePriority = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies) { // init all queue families
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  }

  VkPhysicalDeviceFeatures deviceFeatures = {}; // vk device features
  deviceFeatures.samplerAnisotropy = VK_TRUE; //enable anisotropic filtering

  VkDeviceCreateInfo createInfo = {}; //vk virtual device info
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.pQueueCreateInfos = queueCreateInfos.data();

  createInfo.pEnabledFeatures = &deviceFeatures;
  createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
  createInfo.ppEnabledExtensionNames = deviceExtensions.data();

  // might not really be necessary anymore because device specific validation layers
  // have been deprecated
  if (enableValidationLayers) {
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }

  if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device_) != VK_SUCCESS) { //Try to create physical device
    throw std::runtime_error("failed to create logical device!");
  }

  vkGetDeviceQueue(device_, indices.graphicsFamily, 0, &graphicsQueue_); //Get a queue handle from a device
  vkGetDeviceQueue(device_, indices.presentFamily, 0, &presentQueue_); //Get a queue handle from a device
}

void OVRDevice::createCommandPool() {
  QueueFamilyIndices queueFamilyIndices = findPhysicalQueueFamilies(); // if device supports queue families

  VkCommandPoolCreateInfo poolInfo = {};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
  poolInfo.flags =
      VK_COMMAND_POOL_CREATE_TRANSIENT_BIT //specifies that command buffers allocated from the pool will be short-lived,
      //meaning that they will be reset or freed in a relatively short timeframe.

      | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; //allows any command buffer allocated from a pool to be individually reset to the initial state

  if (vkCreateCommandPool(device_, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) { // try to create command pool
    throw std::runtime_error("failed to create command pool!");
  }
}

void OVRDevice::createSurface() { window.createWindowSurface(instance, &surface_); } // create window surface for Vulkan

bool OVRDevice::isDeviceSuitable(VkPhysicalDevice device) { 
  QueueFamilyIndices indices = findQueueFamilies(device); // check if device supports queue families

  bool extensionsSupported = checkDeviceExtensionSupport(device); // check graphics extensions support

  bool swapChainAdequate = false;
  if (extensionsSupported) {
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device); // check swap chain support
    swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
  }

  VkPhysicalDeviceFeatures supportedFeatures;
  vkGetPhysicalDeviceFeatures(device, &supportedFeatures); //get physical supported features

  return indices.isComplete() && extensionsSupported && swapChainAdequate &&
         supportedFeatures.samplerAnisotropy; //if all things supported return succes
}

void OVRDevice::populateDebugMessengerCreateInfo( //setup what messeges messeger will show
    VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
  createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | // show warning messeges
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT; // show error messeges
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = debugCallback;
  createInfo.pUserData = nullptr;  // Optional
}

void OVRDevice::setupDebugMessenger() {
  if (!enableValidationLayers) return;
  VkDebugUtilsMessengerCreateInfoEXT createInfo;
  populateDebugMessengerCreateInfo(createInfo);
  if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) { // try to create debug messager
      std::cout << "failed to setup debug messeger";
    throw std::runtime_error("failed to set up debug messenger!");
  }

}

bool OVRDevice::checkValidationLayerSupport() { // check if video card support validation layers
  uint32_t layerCount = 0;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const char *layerName : validationLayers) {
    bool layerFound = false;
    

    for (const auto &layerProperties : availableLayers) {
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

std::vector<const char *> OVRDevice::getRequiredExtensions() {
  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

  if (enableValidationLayers) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  return extensions;
}

void OVRDevice::hasGflwRequiredInstanceExtensions() { //get requrend GLFW extensions
  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

  std::cout << "available extensions:" << std::endl;
  std::unordered_set<std::string> available;
  for (const auto &extension : extensions) {
    std::cout << "\t" << extension.extensionName << std::endl;
    available.insert(extension.extensionName);
  }

  std::cout << "required extensions:" << std::endl;
  auto requiredExtensions = getRequiredExtensions();
  for (const auto &required : requiredExtensions) {
    std::cout << "\t" << required << std::endl;
    if (available.find(required) == available.end()) {
      throw std::runtime_error("Missing required glfw extension");
    }
  }
}

bool OVRDevice::checkDeviceExtensionSupport(VkPhysicalDevice device) {
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(
      device,
      nullptr,
      &extensionCount,
      availableExtensions.data());

  std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

  for (const auto &extension : availableExtensions) {
    requiredExtensions.erase(extension.extensionName);
  }

  return requiredExtensions.empty();
}

// check queue families support
QueueFamilyIndices OVRDevice::findQueueFamilies(VkPhysicalDevice device) { // check support for different queues like (grapics queue)
  QueueFamilyIndices indices;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

  int i = 0;
  for (const auto &queueFamily : queueFamilies) {
      //check if device supports graphics queue
    if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
      indices.graphicsFamilyHasValue = true;
    }
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &presentSupport);
    //check if device supports presentation queue
    if (queueFamily.queueCount > 0 && presentSupport) {
      indices.presentFamily = i;
      indices.presentFamilyHasValue = true;
    }
    if (indices.isComplete()) { // if all queues supported -> retun indices
      break;
    }

    i++;
  }

  return indices;
}

SwapChainSupportDetails OVRDevice::querySwapChainSupport(VkPhysicalDevice device) {
  SwapChainSupportDetails details;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_, &details.capabilities);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, nullptr);

  if (formatCount != 0) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, details.formats.data());
  }

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &presentModeCount, nullptr);

  if (presentModeCount != 0) {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device,
        surface_,
        &presentModeCount,
        details.presentModes.data());
  }
  return details;
}

VkFormat OVRDevice::findSupportedFormat( // check supported image format
    const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
  for (VkFormat format : candidates) {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

    if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
      return format;
    } else if (
        tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
      return format;
    }
  }
  throw std::runtime_error("failed to find supported format!");
}

uint32_t OVRDevice::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) &&
        (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
      return i;
    }
  }

  throw std::runtime_error("failed to find suitable memory type!");
}

void OVRDevice::createBuffer(
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer &buffer,
    VkDeviceMemory &bufferMemory) {
  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(device_, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to create vertex buffer!");
  }

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(device_, buffer, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

  if (vkAllocateMemory(device_, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate vertex buffer memory!");
  }

  vkBindBufferMemory(device_, buffer, bufferMemory, 0);
}

VkCommandBuffer OVRDevice::beginSingleTimeCommands() {
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = commandPool;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(device_, &allocInfo, &commandBuffer);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(commandBuffer, &beginInfo);
  return commandBuffer;
}

void OVRDevice::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  vkQueueSubmit(graphicsQueue_, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(graphicsQueue_);

  vkFreeCommandBuffers(device_, commandPool, 1, &commandBuffer);
}

void OVRDevice::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
  VkCommandBuffer commandBuffer = beginSingleTimeCommands();

  VkBufferCopy copyRegion{};
  copyRegion.srcOffset = 0;  // Optional
  copyRegion.dstOffset = 0;  // Optional
  copyRegion.size = size;
  vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

  endSingleTimeCommands(commandBuffer);
}

void OVRDevice::copyBufferToImage(
    VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount) {
  VkCommandBuffer commandBuffer = beginSingleTimeCommands();

  VkBufferImageCopy region{};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;

  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = layerCount;

  region.imageOffset = {0, 0, 0};
  region.imageExtent = {width, height, 1};

  vkCmdCopyBufferToImage(
      commandBuffer,
      buffer,
      image,
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      1,
      &region);
  endSingleTimeCommands(commandBuffer);
}

void OVRDevice::createImageWithInfo(
    const VkImageCreateInfo &imageInfo,
    VkMemoryPropertyFlags properties,
    VkImage &image,
    VkDeviceMemory &imageMemory) {
  if (vkCreateImage(device_, &imageInfo, nullptr, &image) != VK_SUCCESS) {
    throw std::runtime_error("failed to create image!");
  }

  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(device_, image, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

  if (vkAllocateMemory(device_, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate image memory!");
  }

  if (vkBindImageMemory(device_, image, imageMemory, 0) != VK_SUCCESS) {
    throw std::runtime_error("failed to bind image memory!");
  }
}

}  // namespace lve
