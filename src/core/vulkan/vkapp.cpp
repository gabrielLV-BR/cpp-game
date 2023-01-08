#include "./vkapp.hpp"
#include "./vkutils.hpp"

#include "utils/assert.hpp"
#include "utils/log.hpp"

#include <vector>
#include <string>
#include <memory>

using namespace core;

const std::vector<const char*> validation_layers = {
    "VK_LAYER_KHRONOS_validation" // validation layer
};

#ifdef NDEBUG
    const bool use_validation_layers = false;
#else
    const bool use_validation_layers = true;
#endif

vkapp::vkapp(GLFWwindow* window) {
    this->create_instance();
    this->query_physical_device();
    this->create_logical_device();
    //TODO fill in initialization order
}

vkapp::~vkapp() {
    // destroy every object
}

void vkapp::create_instance(){
    using std::vector;

    VkApplicationInfo app_create_info{};
    app_create_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_create_info.apiVersion = VK_VERSION_1_3;
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
        vkGetPhysicalDeviceProperties(device, &physical_device_properties);
        if(physical_device_properties.deviceType ==
            VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            physical_device = device;
            found_device = true;
            break;
        }    
    }

    if(!found_device) physical_device = devices[0];
}

void vkapp::create_logical_device(){
    VkDeviceCreateInfo device_create_info{};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    //

    VK_ASSERT(
        vkCreateDevice(
            physical_device, 
            &device_create_info, 
            nullptr, 
            &device
        )
    );
}

void vkapp::create_swapchain(){}

void vkapp::create_image_view(){}

void vkapp::create_framebuffer(){}

void vkapp::create_render_pass(){}

void vkapp::create_pipeline(){}

components::vulkan_details vkapp::get_details() {
    //TODO return struct with details
    return {};
}