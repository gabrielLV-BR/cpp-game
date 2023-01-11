#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <optional>

namespace core {
    class vkswapchain
    {
        VkInstance instance;
        VkDevice device;

        VkSwapchainKHR handle;
        VkExtent2D extent;
        VkSurfaceFormatKHR format;
        VkPresentModeKHR present_mode;

    public:
        vkswapchain() = default;
        vkswapchain(
            GLFWwindow*, 
            VkInstance, 
            VkPhysicalDevice,
            VkDevice, 
            VkSurfaceKHR
        );

        ~vkswapchain();

        void destroy();
    
        VkExtent2D pick_extent(GLFWwindow*, VkSurfaceCapabilitiesKHR&);
        VkPresentModeKHR pick_present_mode(VkPhysicalDevice, VkSurfaceKHR);
        VkSurfaceFormatKHR pick_format(VkPhysicalDevice, VkSurfaceKHR);
    };
}