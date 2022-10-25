//========================================================================
// OVRenderer (Open Vulkan Renderer) 
// Version: 0.1 
//------------------------------------------------------------------------
// Copyright (c) 2022-2022 Nagornov Vladimir <vladimirnagornov831@gmail.com>
//========================================================================
#include "ovr_image.h"
#include "engine_config.h"
#include "utils/resource_loader.h"

#include <iostream>

#include "ovr_utils.h"

#define GLM_ENABLE_EXPERIMENTAL

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <cassert>
#include <cstring>
#include <glm/gtx/hash.hpp>
#include <unordered_map>

namespace std {
	
}

namespace ovr {
	void OvrImage::Builder::loadImage(const std::string& filepath)
	{
         int texWidth, texHeight, texChannels;
         stbi_uc* pixels = stbi_load("D:\\DEV\\MY_GITHUB\\OVRenderer\\out\\build\\x64-Debug\\resources\\images\\textures\\text_texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
         VkDeviceSize imageSize = texWidth * texHeight * 4;

         
         if (!pixels) {
             std::cout << "failed to load texture";
             //throw std::runtime_error("failed to load texture image!");
         }
	}
    OvrImage::OvrImage(OVRDevice& device) : ovrDevice { device }
    {
        
    }
    OvrImage::~OvrImage()
    {
    }
    std::unique_ptr<OvrImage> OvrImage::createImageFromFile(OVRDevice& device, const std::string& filepath)
    {
        Builder builder{};
        builder.loadImage(filepath);
        std::cout << "image created\n";
        return std::unique_ptr<OvrImage>();
    }
}