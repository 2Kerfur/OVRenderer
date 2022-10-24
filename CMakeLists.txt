#========================================================================
# OVRenderer (Open Vulkan Renderer) 
# Version: 0.1 
#------------------------------------------------------------------------
# Copyright (c) 2022-2022 Nagornov Vladimir <vladimirnagornov831@gmail.com>
#========================================================================

cmake_minimum_required(VERSION 3.14)
project(OVRenderer)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME}
        src/main.cpp "src/AppWindow.cpp" "src/AppWindow.h" 
        "src/App.h" "src/App.cpp" "src/ovr_pipeline.h" "src/ovr_pipeline.cpp"
        "src/ovr_device.h" "src/ovr_model.h" "src/ovr_device.cpp" "src/ovr_swap_chain.cpp" "src/ovr_model.h" 
        "src/ovr_model.cpp" "src/ovr_game_object.h" 
        "src/ovr_renderer.cpp" "src/simple_render_system.h" 
        "src/simple_render_system.cpp" "src/ovr_camera.h" 
        "src/ovr_camera.cpp" "src/keyboard_movement_controller.h"
        "src/keyboard_movement_controller.cpp" "src/ovr_utils.h" "src/utils/resource_loader.h" "src/utils/resource_loader.cpp" "src/engine_config.h" "src/ovr_game_object.cpp")


target_include_directories(${PROJECT_NAME}
        PUBLIC external/glfw/include
        PUBLIC external/glm
        PUBLIC external/stb
        PUBLIC external/tinyloader
        PUBLIC ${VULKAN_INCLUDE_DIRS} #linux specific
        PUBLIC C:/VulkanSDK/1.3.224.1/Include # win specific
        PUBLIC src
        )
target_link_directories(${PROJECT_NAME}
        PRIVATE external/glfw/lib
        PRIVATE C:/VulkanSDK/1.3.224.1/Lib # win specific
        PRIVATE ${VULKAN_LIB_LIST}
        PRIVATE ${CMAKE_DL_LIBS}
        )


if (WIN32)
    target_link_libraries(${PROJECT_NAME} glfw3) #glfw window
    target_link_libraries (${PROJECT_NAME} vulkan-1) # ${Vulkan_LIBRARIES}
endif (WIN32)

if (UNIX)
    target_link_libraries(${PROJECT_NAME} glfw3 X11 dl pthread) #glfw window
    target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARIES}) #vulkan libs
endif (UNIX)
