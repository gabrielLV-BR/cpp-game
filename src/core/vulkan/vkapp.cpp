#include "./vkapp.hpp"
#include "./vkutils.hpp"

#include "utils/assert.hpp"
#include "utils/log.hpp"
#include "utils/list.hpp"

#include <unordered_set>
#include <vector>
#include <string>
#include <memory>

using namespace core;

const std::vector<const char*> validation_layers = {
    "VK_LAYER_KHRONOS_validation" // validation layer
};

const std::vector<const char*> device_extensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
    const bool use_validation_layers = false;
#else
    const bool use_validation_layers = true;
#endif

vkapp::vkapp(GLFWwindow* window) {
    this->create_instance();
    this->query_physical_device();
    this->get_queue_family_indices();
    this->create_logical_device();
    this->create_surface(window);
    this->create_swapchain(window);
    //TODO fill in initialization order
}

vkapp::~vkapp() {
    // destroy every object
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);
}

void vkapp::create_instance(){
    using std::vector;

    VkApplicationInfo app_create_info{};
    app_create_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_create_info.apiVersion = VK_API_VERSION_1_3;
    app_create_info.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    app_create_info.pApplicationName = "game";
    app_create_info.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    app_create_info.pEngineName = "GECK0";

    VkInstanceCreateInfo instance_create_info{};
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.flags = 0;
    instance_create_info.pApplicationInfo = &app_create_info;

    uint32_t required_extension_count = 0;
    const char** required_extension_names = glfwGetRequiredInstanceExtensions(&required_extension_count);
    
    instance_create_info.ppEnabledExtensionNames = required_extension_names;
    instance_create_info.enabledExtensionCount   = required_extension_count;

    if(use_validation_layers) {
        ASSERT(
            vkutils::check_validation_layers_support(validation_layers),
            "Required validation layers were not provided."
        );
        instance_create_info.enabledLayerCount = validation_layers.size();
        instance_create_info.ppEnabledLayerNames = validation_layers.data();
    } else {
        instance_create_info.enabledLayerCount = 0;
        instance_create_info.ppEnabledLayerNames = nullptr;
    }

    VK_ASSERT(
        vkCreateInstance(&instance_create_info, nullptr, &instance)
    );

    LOG("Instance created");
}

void vkapp::query_physical_device(){
    uint32_t physical_device_count = 0;
    vkEnumeratePhysicalDevices(instance, &physical_device_count, nullptr);

    ASSERT(physical_device_count > 0, "No graphics device found");

    std::vector<VkPhysicalDevice> devices(physical_device_count);
    vkEnumeratePhysicalDevices(instance, &physical_device_count, devices.data());

    bool found_device = false;

    auto found_device = pick_physical_device(devices);
}

void vkapp::get_queue_family_indices() {
    using std::vector;

    uint32_t queue_family_count;
    vkGetPhysicalDeviceQueueFamilyProperties(
        physical_device, 
        &queue_family_count, 
        nullptr
    );

    ASSERT(queue_family_count > 0, "No queue family available");

    vector<VkQueueFamilyProperties> 
        queue_family_properties(queue_family_count); 

    vkGetPhysicalDeviceQueueFamilyProperties(
        physical_device, 
        &queue_family_count, 
        queue_family_properties.data()
    );

    int i = 0;
    for(auto& queue_family : queue_family_properties) {

        VkBool32 is_present_family = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(
            physical_device, 
            i, 
            surface, 
            &is_present_family
        );

        if(is_present_family == VK_TRUE)                    family_indices.present_family = i;
        if(queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) family_indices.graphics_family = i;
        
        i++;
    }

    ASSERT(
        family_indices.is_complete(), 
        "Required family indices not provided entirely"
    );
}

void vkapp::create_logical_device(){    
    
    using std::vector, std::unordered_set;

    // info about how many queues we want from each queue family
    vector<VkDeviceQueueCreateInfo> device_queue_create_infos{};
    unordered_set<uint32_t> unique_queue_family_indices = {
        family_indices.graphics_family.value(),
        family_indices.present_family.value()
    };
    float queue_priority = 1.0f;
    for(auto unique_family_index : unique_queue_family_indices) {
        VkDeviceQueueCreateInfo device_queue_create_info{};
        device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        device_queue_create_info.queueFamilyIndex = unique_family_index;
        device_queue_create_info.queueCount = 1;
        device_queue_create_info.pQueuePriorities = &queue_priority;
    
        device_queue_create_infos.push_back(device_queue_create_info);
    }

    // we won't need this for now, but in the future we'll use
    // them to specify required features such as geometry shaders
    VkPhysicalDeviceFeatures physical_device_features{};

    // 
    VkDeviceCreateInfo device_create_info{};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.pQueueCreateInfos = device_queue_create_infos.data();
    device_create_info.queueCreateInfoCount = device_queue_create_infos.size();
    device_create_info.pEnabledFeatures = &physical_device_features;

    device_create_info.ppEnabledExtensionNames = device_extensions.data();
    device_create_info.enabledExtensionCount   = device_extensions.size();

    if(use_validation_layers) {
        device_create_info.enabledLayerCount   = validation_layers.size();
        device_create_info.ppEnabledLayerNames = validation_layers.data();
    } else {
        device_create_info.enabledLayerCount   = 0;
        device_create_info.ppEnabledLayerNames = nullptr;
    }

    VK_ASSERT(
        vkCreateDevice(
            physical_device, 
            &device_create_info, 
            nullptr, 
            &device
        )
    );

    // get actual queues from indices
    vkGetDeviceQueue(device, family_indices.graphics_family.value(), 0, &graphics_queue);
    vkGetDeviceQueue(device, family_indices.present_family.value(),  0, &present_queue);
}

void vkapp::create_surface(GLFWwindow* window) {
    VK_ASSERT(
        glfwCreateWindowSurface(
            instance,
            window,
            nullptr,
            &surface
        )
    );
}

void vkapp::create_swapchain(GLFWwindow* window) {
    using std::vector;

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    
    VkSwapchainCreateInfoKHR swapchain_create_info{};
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    
    // we must check for an adequate swapchain, as well
    // as the best format, extent and present_mode available

    VkSurfaceCapabilitiesKHR surface_capabilities{};
    vector<VkSurfaceFormatKHR> formats;
    vector<VkPresentModeKHR> present_modes;

    /* format */ {
        uint32_t format_count;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, nullptr);
        formats.resize(format_count);    
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, formats.data());
    }
    /* present_modes */ {
        uint32_t present_mode_count;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, nullptr);
        present_modes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, nullptr, present_modes.data());
    }
    /* surface capabilities */ {
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &surface_capabilities);
    }

    auto swapchain_properties = pick_swapchain_properties(window, surface_capabilities, formats, present_modes);

    ASSERT(swapchain_properties.is_complete(), "Swapchain properties was not complete");

    swapchain_create_info.imageExtent = swapchain_properties.extent.value();
    swapchain_create_info.presentMode = swapchain_properties.present_mode.value();
    swapchain_create_info.imageFormat = swapchain_properties.format.value().format;
    swapchain_create_info.imageColorSpace = swapchain_properties.format.value().colorSpace;

    VK_ASSERT(
        vkCreateSwapchainKHR(
            device, 
            &swapchain_create_info, 
            nullptr, 
            &swapchain.handle
        )
    );
}

void vkapp::create_image_view(){}

void vkapp::create_framebuffer(){}

void vkapp::create_render_pass(){}

void vkapp::create_pipeline(){}

components::vulkan_details vkapp::get_details() {
    //TODO return struct with details
    return {};
}

//

std::optional<VkPhysicalDevice> vkapp::pick_physical_device(std::vector<VkPhysicalDevice> available_devices) {
    for(auto physical_device : available_devices) {
        VkPhysicalDeviceProperties physical_device_properties{};
        VkPhysicalDeviceFeatures physical_device_features{};

        vkGetPhysicalDeviceProperties(physical_device, &physical_device_properties);
        vkGetPhysicalDeviceFeatures(physical_device, &physical_device_features);
        
        //TODO Less dumb way of picking physical device, eg. scoring
        if(
            physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && 
            physical_device_features.geometryShader &&
            vkutils::check_device_extension_support(physical_device, device_extensions)
        ) {
            return { physical_device };
        }    
    }

    return {};
}
swapchain_properties pick_swapchain_properties(
    GLFWwindow* window,
    VkSurfaceCapabilitiesKHR &surface_capabilities,
    std::vector<VkSurfaceFormatKHR> formats,
    std::vector<VkPresentModeKHR> present_modes
) {
    swapchain_properties properties{};
    return properties;
}