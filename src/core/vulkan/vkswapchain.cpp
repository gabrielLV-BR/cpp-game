#include "./vkswapchain.hpp"

#include <vector>
#include <limits>
#include <cstdint>
#include <algorithm>

#include "utils/assert.hpp"
#include "core/vulkan/vkutils.hpp"
#include "core/vulkan/vkstructs.hpp"

using namespace core;

vkswapchain::vkswapchain() {
    instance = VK_NULL_HANDLE;
    handle = VK_NULL_HANDLE;
    initialized = false;
}

vkswapchain::vkswapchain(
    GLFWwindow* window,
    VkInstance instance, 
    VkPhysicalDevice physical_device, 
    VkDevice device,
    VkSurfaceKHR surface
) : instance(instance) {
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

    get_images(device);
    get_image_views(device);
    get_framebuffers(device);

    initialized = true;
}

void vkswapchain::get_images(
    VkDevice device
) {
    uint32_t image_count = 0;
    vkGetSwapchainImagesKHR(device, handle, &image_count, nullptr);
    
    images.resize(image_count);
    vkGetSwapchainImagesKHR(
        device, 
        handle, 
        &image_count, 
        images.data()
    );
}

void vkswapchain::get_image_views(VkDevice device) {
    image_views.resize(images.size());
    
    for(int i = 0; i < image_views.size(); i++) {
        VkImageViewCreateInfo image_view_create_info{};
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.image = images[i];
        image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_create_info.format = format.format;

        image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_create_info.subresourceRange.baseArrayLayer = 0;
        image_view_create_info.subresourceRange.layerCount = 1;
        image_view_create_info.subresourceRange.baseMipLevel = 0;
        image_view_create_info.subresourceRange.levelCount = 1;

        VK_ASSERT(
            vkCreateImageView(
                device,
                &image_view_create_info,
                nullptr,
                &image_views[i]
            )
        );
        
    }
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

void vkswapchain::get_framebuffers(VkDevice device) {
    VkFramebufferCreateInfo framebuffer_info{};

    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    
    vkCreateFramebuffer(
        device, &framebuffer_info, nullptr, nullptr 
    );
}

//TODO error when destroying, check it out
void vkswapchain::destroy(VkDevice device) {
    if(!initialized) return;
    
    if(image_views.size() > 0 ) {
        for (auto &image_view : image_views)
        {
            vkDestroyImageView(device, image_view, nullptr);
        }
    }
    if(handle != VK_NULL_HANDLE) 
        vkDestroySwapchainKHR(device, handle, nullptr);

    initialized = false;
}