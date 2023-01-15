#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "utils/assert.hpp"
#include <string>

namespace core
{
    class vkshader
    {
    public:
        enum shader_type
        {
            VERTEX = VK_SHADER_STAGE_VERTEX_BIT,
            FRAGMENT = VK_SHADER_STAGE_FRAGMENT_BIT
        };

        VkShaderModule handle;
        shader_type type;

        vkshader();

        vkshader(
            VkDevice device,
            std::string source,
            shader_type type);

        void destroy(VkDevice);
    };
}