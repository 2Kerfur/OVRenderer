cmake_minimum_required(VERSION 3.14)
project(OVRenderer)

set(CMAKE_CXX_STANDARD 14)


find_package(Vulkan REQUIRED FATAL_ERROR)


add_executable(${PROJECT_NAME}
        src/main.cpp)

target_include_directories(${PROJECT_NAME}
        PUBLIC external/glfw/include
        PUBLIC external/glm/glm
        PUBLIC src
        PUBLIC ${VULKAN_INCLUDE_DIRS}
        )
target_link_directories(${PROJECT_NAME}
        PRIVATE external/glfw/lib
        PRIVATE ${VULKAN_LIB_LIST}
        PRIVATE ${CMAKE_DL_LIBS}
        )


if (WIN32)
    #TODO: add support for windows
endif (WIN32)

if (UNIX)
    target_link_libraries(${PROJECT_NAME} glfw3 X11 dl pthread) #glfw window
    target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARIES}) #vulkan libs
endif (UNIX)

