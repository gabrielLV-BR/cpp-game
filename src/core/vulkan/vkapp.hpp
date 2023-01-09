#pragma once

#include "components/components.hpp"
#include <optional>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

namespace core
{
    struct queue_family_indices {
        std::optional<uint32_t> graphics_family;
        std::optional<uint32_t> present_family;

        // must keep filling with important queue families
        bool is_complete() {
            return graphics_family.has_value() &&
                present_family.has_value();
        }
    };

    struct swapchain_details {
        VkSwapchainKHR handle;
    };

    class vkapp
    {
    public:
        VkInstance instance;
        VkPhysicalDevice physical_device;
        VkDevice device;
        VkSurfaceKHR surface;
        VkRenderPass render_pass;

        VkQueue graphics_queue;
        VkQueue present_queue;

        queue_family_indices family_indices;
        swapchain_details swapchain;

        vkapp(GLFWwindow*);
        ~vkapp();

        void create_instance();
        void query_physical_device();
        void get_queue_family_indices();
        void create_logical_device();
        void create_surface(GLFWwindow*);
        void create_swapchain();
        void create_image_view();
        void create_framebuffer();
        void create_render_pass();
        void create_pipeline();

        components::vulkan_details get_details();
    };
}