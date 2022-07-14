cmake_minimum_required(VERSION 3.14)
project(OVRenderer)

set(CMAKE_CXX_STANDARD 14)

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
        )

if (WIN32)
    target_link_libraries(${PROJECT_NAME} glfw3)
endif (WIN32)

if (UNIX)
    target_link_libraries(${PROJECT_NAME} glfw3 X11 dl pthread)
    target_link_libraries (${PROJECT_NAME} ${VULKAN_LIB_LIST})
endif (UNIX)