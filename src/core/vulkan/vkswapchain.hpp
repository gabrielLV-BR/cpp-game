#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <vector>
#include <optional>

namespace core {
    class vkswapchain
    {
    private:
        std::vector<VkFramebuffer> framebuffers;

    public:
        VkInstance instance;

        VkSwapchainKHR handle;
        VkExtent2D extent;
        VkSurfaceFormatKHR format;
        VkPresentModeKHR present_mode;

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
        std::vector<VkFramebuffer> get_framebuffers();

        void create_framebuffers(VkRenderPass, VkDevice);

    private:
        VkExtent2D pick_extent(GLFWwindow*, VkSurfaceCapabilitiesKHR&);
        VkPresentModeKHR pick_present_mode(VkPhysicalDevice, VkSurfaceKHR);
        VkSurfaceFormatKHR pick_format(VkPhysicalDevice, VkSurfaceKHR);

        void get_images(VkDevice);
        void get_image_views(VkDevice);
    };
}