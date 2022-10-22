//========================================================================
// OVRenderer (Open Vulkan Renderer) 
// Version: 0.1 
//------------------------------------------------------------------------
// Copyright (c) 2022-2022 Nagornov Vladimir <vladimirnagornov831@gmail.com>
//========================================================================

#pragma once

#include "ovr_game_object.h"
#include "AppWindow.h"

namespace ovr {
	class KeyboardMovementController {
	public:
        struct KeyMappings {
             int moveLeft = GLFW_KEY_A;
             int moveRight = GLFW_KEY_D;
             int moveForward = GLFW_KEY_W;
             int moveBackward = GLFW_KEY_S;
             int moveUp = GLFW_KEY_E;
             int moveDown = GLFW_KEY_Q;
             int lookLeft = GLFW_KEY_LEFT;
             int lookRight = GLFW_KEY_RIGHT;
             int lookUp = GLFW_KEY_UP;
             int lookDown = GLFW_KEY_DOWN;
        };
        void moveInPlaneXZ(GLFWwindow* window, float dt, OvrGameObject& gameObject);


        KeyMappings keys{};
        float moveSpeed{ 3.f };
        float lookSpeed{ 1.5f };
	};
}