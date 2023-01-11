#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <optional>
#include <cstdint>

namespace structs {
    struct queue_family_indices
    {
        std::optional<uint32_t> graphics_family;
        std::optional<uint32_t> present_family;

        // must keep filling with important queue families
        bool is_complete()
        {
            return graphics_family.has_value() &&
                   present_family.has_value();
        }
    };

    struct swapchain_properties
    {
        std::optional<VkExtent2D> extent;
        std::optional<VkSurfaceFormatKHR> format;
        std::optional<VkPresentModeKHR> present_mode;

        bool is_complete()
        {
            return extent.has_value() &&
                   format.has_value() &&
                   present_mode.has_value();
        }
    };
}