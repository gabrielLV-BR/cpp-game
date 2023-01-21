#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "utils/assert.hpp"
#include <vector>

namespace core
{
    class vkshader
    {
    public:

        VkShaderModule handle;
        VkShaderStageFlagBits stage_flags;

        vkshader();

        vkshader(
            VkDevice device,
            std::vector<char> source,
            VkShaderStageFlagBits);

        void destroy(VkDevice);
    };
}