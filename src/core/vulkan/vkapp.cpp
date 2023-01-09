#include "./vkapp.hpp"
#include "./vkutils.hpp"

#include "utils/assert.hpp"
#include "utils/log.hpp"

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
    for(auto device : devices) {
        VkPhysicalDeviceProperties physical_device_properties{};
        VkPhysicalDeviceFeatures physical_device_features{};

        vkGetPhysicalDeviceProperties(device, &physical_device_properties);
        vkGetPhysicalDeviceFeatures(device, &physical_device_features);
        
        //TODO Less dumb way of picking physical device, eg. scoring
        if(
            physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && 
            physical_device_features.geometryShader
        ) {
            physical_device = device;
            found_device = true;
            break;
        }    
    }

    if(!found_device) physical_device = devices[0];
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
    // for now, we only care about graphics
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
    // them to query for availability of features such as 
    // geometry shaders
    VkPhysicalDeviceFeatures physical_device_features{};

    // 
    VkDeviceCreateInfo device_create_info{};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.pQueueCreateInfos = device_queue_create_infos.data();
    device_create_info.queueCreateInfoCount = device_queue_create_infos.size();
    device_create_info.pEnabledFeatures = &physical_device_features;
    device_create_info.enabledExtensionCount = 0;

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

void vkapp::create_swapchain() {
    
}

void vkapp::create_image_view(){}

void vkapp::create_framebuffer(){}

void vkapp::create_render_pass(){}

void vkapp::create_pipeline(){}

components::vulkan_details vkapp::get_details() {
    //TODO return struct with details
    return {};
}