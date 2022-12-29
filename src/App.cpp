//========================================================================
// OVRenderer (Open Vulkan Renderer) 
// Version: 0.1 
//------------------------------------------------------------------------
// Copyright (c) 2022-2022 Nagornov Vladimir <vladimirnagornov831@gmail.com>
//========================================================================
#include "App.h"
#include "simple_render_system.h"
#include "keyboard_movement_controller.h"
#include <iostream>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "ovr_camera.h"
#include <array>
#include <stdexcept>
#include <chrono>

namespace ovr {
 
	MainApp::MainApp() {
		loadGameObjects();
	}

	MainApp::~MainApp() {
	}

	void MainApp::run() {

		SimpleRenderSystem simpleRenderSystem{ ovrDevice, ovrRender.getSwapChainRenderPass() };
        OvrCamera camera{};
        //camera.setViewDirection(glm::vec3(0.f), glm::vec3(0.0f, 0.0f, 1.f));
        camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));
        

        auto viewerObject = OvrGameObject::createGameObject();
        KeyboardMovementController cameraController{};


        auto currentTime = std::chrono::high_resolution_clock::now();
        
        while (!appWindow.shouldClose()) {
			glfwPollEvents(); //get Window events
		
            auto newTime = std::chrono::high_resolution_clock::now();
            
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.moveInPlaneXZ(appWindow.getGLFWindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = ovrRender.getAspectRatio();
            //camera.setOrthographicProjection(-1, 1, -1, 1, -1, 1);
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 1000.f);

			if (auto commandBuffer = ovrRender.beginFrame()) {

				//

				ovrRender.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
				ovrRender.endSwapChainRenderPass(commandBuffer);
				ovrRender.endFrame();
			}
		}
	}
    
	void MainApp::loadGameObjects()
	{
        std::shared_ptr<OvrModel> ovrModel[1];

        //std::shared_ptr<OvrImage> ovrImage[1];
        //ovrImage[0] = OvrImage::createImageFromFile(ovrDevice,
        //    "D:\\DEV\\MY_GITHUB\\OVRenderer\\out\\build\\x64 - Release\\resources\\images\\textures\\text_texture.jpg");
        

        for (int i = 0; i < 1; i++) {
            ovrModel[i] = OvrModel::createModelFromFile(
                ovrDevice,
                "D:\\DEV\\MY_GITHUB\\OVRenderer\\out\\build\\x64-Release\\resources\\models\\lada_niva.obj");
            std::cout << i;
        }
        float trans = 0;
        ovr::OvrGameObject car[1];
        for (int i = 0; i < 1; i++) {
            car[i] = OvrGameObject::createGameObject();
            car[i].model = ovrModel[0];
            car[i].transform.translation = { trans, .0f, 2.5f };
            car[i].transform.scale = { .5f, .5f, .5f };
            trans = trans + 1;
        }

        //ovr::OvrGameObject image[1];
        //car[0] = OvrGameObject::createGameObject();
        //car[]
        

        for (int i = 0; i < 1; i++) {
            gameObjects.push_back(std::move(car[i]));
        }

	}


}