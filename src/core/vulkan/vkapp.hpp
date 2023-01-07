#pragma once

#include "components/components.hpp"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

namespace core
{
    class vkapp
    {
    public:
        VkInstance instance_handle;
        VkPhysicalDevice physical_device;
        VkDevice device;
        VkSwapchainKHR swapchain;
        VkRenderPass render_pass;

        vkapp(GLFWwindow*);
        ~vkapp();

        void create_instance();
        void query_physical_device();
        void create_logical_device();
        void create_swapchain();
        void create_image_view();
        void create_framebuffer();
        void create_render_pass();
        void create_pipeline();

        components::vulkan_details get_details();
    };
}