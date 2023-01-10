#pragma once

#include "components/components.hpp"
#include <optional>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

namespace core
{
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

    struct swapchain_details
    {
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

        vkapp(GLFWwindow *);
        ~vkapp();

        void create_instance();
        void query_physical_device();
        void get_queue_family_indices();
        void create_logical_device();
        void create_surface(GLFWwindow *);
        void create_swapchain(GLFWwindow *);
        void create_image_view();
        void create_framebuffer();
        void create_render_pass();
        void create_pipeline();

        components::vulkan_details get_details();

    private:
        // HELPER FUNCTIONS

        std::optional<VkPhysicalDevice> pick_physical_device(std::vector<VkPhysicalDevice> available_devices);
        swapchain_properties pick_swapchain_properties(
            GLFWwindow* window,
            VkSurfaceCapabilitiesKHR& surface_capabilities,
            std::vector<VkSurfaceFormatKHR> formats,
            std::vector<VkPresentModeKHR> present_modes);
        };
}