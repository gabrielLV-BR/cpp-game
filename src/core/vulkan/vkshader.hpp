#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

namespace core {

    class vkshader {

        vkshader(
            VkDevice device
        ) {
            VkShaderModuleCreateInfo shader_create_info{};
            shader_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            shader_create_info.flags = VK_SHADER_STAGE_VERTEX_BIT;
            
            
            VkShaderModule shader_module;
            vkCreateShaderModule(device, &shader_create_info, nullptr, &shader_module);
        }        

    };

}