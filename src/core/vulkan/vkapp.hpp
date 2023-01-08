#pragma once

#include "components/components.hpp"
#include <optional>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

namespace core
{
    struct queue_family_indices {
        std::optional<uint32_t> graphics_family;

        // must keep filling with important queue families
        bool is_complete() {
            return graphics_family.has_value();
        }
    };

    class vkapp
    {
    public:
        VkInstance instance;
        VkPhysicalDevice physical_device;
        VkDevice device;
        VkSwapchainKHR swapchain;
        VkRenderPass render_pass;

        VkQueue graphics_queue;
        queue_family_indices family_indices;

        vkapp(GLFWwindow*);
        ~vkapp();

        void create_instance();
        void query_physical_device();
        void get_queue_family_indices();
        void create_logical_device();
        void create_swapchain();
        void create_image_view();
        void create_framebuffer();
        void create_render_pass();
        void create_pipeline();

        components::vulkan_details get_details();
    };
}