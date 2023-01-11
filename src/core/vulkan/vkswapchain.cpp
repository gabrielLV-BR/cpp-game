#include "./vkswapchain.hpp"

#include <vector>
#include <limits>
#include <cstdint>
#include <algorithm>

#include "utils/assert.hpp"
#include "core/vulkan/vkutils.hpp"
#include "core/vulkan/vkstructs.hpp"

using namespace core;

vkswapchain::vkswapchain(
    GLFWwindow* window,
    VkInstance instance, 
    VkPhysicalDevice physical_device, 
    VkDevice device,
    VkSurfaceKHR surface
) : instance(instance), device(device) {
    using std::vector;

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    VkSurfaceCapabilitiesKHR surface_capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &surface_capabilities);

    VkSwapchainCreateInfoKHR swapchain_create_info{};
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;

    extent       = pick_extent(window, surface_capabilities);
    format       = pick_format(physical_device, surface);
    present_mode = pick_present_mode(physical_device, surface);

    swapchain_create_info.imageExtent = extent;
    swapchain_create_info.presentMode = present_mode;
    swapchain_create_info.imageFormat = format.format;
    swapchain_create_info.imageColorSpace = format.colorSpace;
    swapchain_create_info.surface = surface;

    // good to always request at least one more, so less waiting has to be done
    swapchain_create_info.minImageCount = surface_capabilities.minImageCount + 1;
    if (surface_capabilities.maxImageCount > 0 && swapchain_create_info.minImageCount > surface_capabilities.maxImageCount) {
        swapchain_create_info.minImageCount = surface_capabilities.maxImageCount;
    }

    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    structs::queue_family_indices family_indices = vkutils::get_queue_family_indices(physical_device, surface);
    uint32_t family_indices_array[] = {family_indices.graphics_family.value(), family_indices.present_family.value()};

    

    // optimize if same family
    if(family_indices.graphics_family.value() != family_indices.present_family.value()) {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_create_info.queueFamilyIndexCount = 2;
        swapchain_create_info.pQueueFamilyIndices = family_indices_array;
    } else {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_create_info.queueFamilyIndexCount = 0;
        swapchain_create_info.pQueueFamilyIndices = nullptr;
    }
    swapchain_create_info.preTransform = surface_capabilities.currentTransform;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.clipped = true;

    swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;

    VK_ASSERT(
        vkCreateSwapchainKHR(
            device, 
            &swapchain_create_info, 
            nullptr, 
            &handle
        )
    );
}

vkswapchain::~vkswapchain() {
    this->destroy();
}

VkPresentModeKHR vkswapchain::pick_present_mode(
    VkPhysicalDevice physical_device,
    VkSurfaceKHR surface
) {
    using std::vector;

    uint32_t present_mode_count;
    vector<VkPresentModeKHR> present_modes;

    vkGetPhysicalDeviceSurfacePresentModesKHR(
        physical_device, 
        surface, 
        &present_mode_count, 
        nullptr
    );
    
    present_modes.resize(present_mode_count);
    
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        physical_device, 
        surface, 
        &present_mode_count, 
        present_modes.data()
    );

    for(auto& p : present_modes) {
        if(p == VK_PRESENT_MODE_MAILBOX_KHR)
            return p;        
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkSurfaceFormatKHR vkswapchain::pick_format(
    VkPhysicalDevice physical_device,
    VkSurfaceKHR surface
) {
    using std::vector;

    vector<VkSurfaceFormatKHR> formats;
    uint32_t format_count;

    vkGetPhysicalDeviceSurfaceFormatsKHR(
        physical_device, 
        surface, 
        &format_count, 
        nullptr
    );
 
    formats.resize(format_count);
 
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        physical_device, 
        surface, 
        &format_count, 
        formats.data()
    );

    for(auto& format : formats) {
        if(format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR &&
            format.format == VK_FORMAT_R8G8B8A8_SRGB ) {
                return format;
            }
    }

    return formats[0];
}

VkExtent2D vkswapchain::pick_extent(
    GLFWwindow* window,
    VkSurfaceCapabilitiesKHR& surface_capabilities
) {
    if(surface_capabilities.currentExtent.width 
        != std::numeric_limits<uint32_t>::max()) {
        return surface_capabilities.currentExtent;      
    }

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    VkExtent2D extent {
        std::clamp(
            static_cast<uint32_t>(width),
            surface_capabilities.minImageExtent.width,
            surface_capabilities.maxImageExtent.width
        ),
        std::clamp(
            static_cast<uint32_t>(height),
            surface_capabilities.minImageExtent.height,
            surface_capabilities.maxImageExtent.height
        )
    };

    return extent;   
}

//TODO error when destroying, check it out
void vkswapchain::destroy() {
    vkDestroySwapchainKHR(device, handle, nullptr);
}