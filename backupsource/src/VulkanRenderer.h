#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>

#include <Utilities.h>

class VulkanRenderer {
public:
    int init(GLFWwindow * newWindow);
    void cleanup();

private:
    GLFWwindow * window;

    //Vulkan components
    VkInstance instance;

    struct {
        VkPhysicalDevice  physicalDevice;
        VkDevice logicalDevice;
    } mainDevice;

    VkQueue  graphicsQueue;

    VkSurfaceKHR surface;

    //Vulkan Functions
    void createInstance();
    

    // - Get functions
    void getPhysicalDevice();

    // - Create functions
    void createLogicalDevice();
    void createSurface();



    // -- Support
    bool checkInstanceExtensionSupport(std::vector<const char*> * checkExtension);

    bool checkDeviceSuitable(VkPhysicalDevice device);

    QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);
};


