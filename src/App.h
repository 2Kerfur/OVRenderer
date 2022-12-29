//========================================================================
// OVRenderer (Open Vulkan Renderer) 
// Version: 0.1 
//------------------------------------------------------------------------
// Copyright (c) 2022-2022 Nagornov Vladimir <vladimirnagornov831@gmail.com>
//========================================================================

#pragma once

#include "AppWindow.h"
#include "ovr_device.h"
#include "ovr_model.h"
#include "ovr_image.h"
#include "ovr_game_object.h"
#include "ovr_renderer.h"

#include <memory>
#include <vector>

namespace ovr {

	class MainApp {

	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		MainApp();
		~MainApp();

		// c++11 Disallow copying (compiler will not generate those constructors)
		MainApp(const MainApp&) = delete;
		MainApp& operator=(const MainApp&) = delete;
		void run();

	private:
		void loadGameObjects();

		AppWindow appWindow{WIDTH, HEIGHT, "OVRenderer"};
		OVRDevice ovrDevice{appWindow};
		OvrRenderer ovrRender{ appWindow, ovrDevice };

		std::vector<OvrGameObject> gameObjects;
	};
}