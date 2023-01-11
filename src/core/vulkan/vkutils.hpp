#pragma once

#include "core/vulkan/vkstructs.hpp"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <vector>

namespace vkutils
{
    bool check_validation_layers_support(std::vector<const char *> validation_layers);
    bool check_device_extension_support(VkPhysicalDevice device, std::vector<const char *> extension_names);
    structs::queue_family_indices get_queue_family_indices(VkPhysicalDevice physical_device, VkSurfaceKHR surface);
}