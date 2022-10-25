//========================================================================
// OVRenderer (Open Vulkan Renderer) 
// Version: 0.1 
//------------------------------------------------------------------------
// Copyright (c) 2022-2022 Nagornov Vladimir <vladimirnagornov831@gmail.com>
//========================================================================

#pragma once

#include "ovr_model.h"
#include "ovr_image.h"


#include <glm/gtc/matrix_transform.hpp>

//std
#include <memory>


namespace ovr {

	struct TransformComponent {
		glm::vec3 translation{};
		glm::vec3 scale{ 1.f, 1.f , 1.f};

		glm::vec3 rotation{};

		glm::mat4 mat4();

		glm::mat3 normalMatrix();
	};

	class OvrGameObject {
	public:
		using id_t = unsigned int;

		OvrGameObject() {};
		static OvrGameObject createGameObject() {
			static id_t currentId = 0;
			return OvrGameObject{ currentId };
		}
		OvrGameObject(const OvrGameObject&) = delete;
		OvrGameObject& operator =(const OvrGameObject&) = delete;
		OvrGameObject(OvrGameObject&&) = default;
		OvrGameObject& operator = (OvrGameObject&&) = default;
		
		id_t getId() { return id; }

		std::shared_ptr<OvrModel> model{};
		glm::vec3 color{};
		TransformComponent transform{};


	private:
		OvrGameObject(id_t Obj_id) : id(Obj_id) {}
		id_t id;

	};
}