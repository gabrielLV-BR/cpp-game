#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <vector>
#include <optional>

namespace core {
    class vkswapchain
    {
        VkInstance instance;

        VkSwapchainKHR handle;
        VkExtent2D extent;
        VkSurfaceFormatKHR format;
        VkPresentModeKHR present_mode;

        bool initialized;

    public:
        vkswapchain();
        vkswapchain(
            GLFWwindow*, 
            VkInstance, 
            VkPhysicalDevice,
            VkDevice, 
            VkSurfaceKHR
        );

        void destroy(VkDevice);
    
        std::vector<VkImage> images;
        std::vector<VkImageView> image_views;
        std::vector<VkFramebuffer> framebuffers;

    private:

        VkExtent2D pick_extent(GLFWwindow*, VkSurfaceCapabilitiesKHR&);
        VkPresentModeKHR pick_present_mode(VkPhysicalDevice, VkSurfaceKHR);
        VkSurfaceFormatKHR pick_format(VkPhysicalDevice, VkSurfaceKHR);

        void get_images(VkDevice);
        void get_image_views(VkDevice);
        void get_framebuffers(VkDevice);
    };
}