#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>

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
    } mainDeivece;

    //Vulkan Functions
    void createInstance();

    // - Get functions
    void getPhysicalDevice();


    // -- Support
    bool checkInstanceExtensionSupport(std::vector<const char*> * checkExtension);
    bool checkDeviceSuitable(VkPhysicalDevice device);
};


