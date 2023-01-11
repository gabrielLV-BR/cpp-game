#pragma once

#include "core/vulkan/vkswapchain.hpp"
#include "core/vulkan/vkstructs.hpp"
#include "components/components.hpp"

#include <optional>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

namespace core
{
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

        structs::queue_family_indices family_indices;
        core::vkswapchain swapchain;

        vkapp(GLFWwindow *);
        ~vkapp();

        void create_instance();
        void query_physical_device();
        void create_logical_device();
        void create_surface(GLFWwindow *);
        void create_image_view();
        void create_framebuffer();
        void create_render_pass();
        void create_pipeline();

        components::vulkan_details get_details();

    private:
        // HELPER FUNCTIONS

        std::optional<VkPhysicalDevice> pick_physical_device(
            std::vector<VkPhysicalDevice> available_devices
        );
    };
}