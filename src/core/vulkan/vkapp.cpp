#include "./vkapp.hpp"

#include "utils/assert.hpp"

#include <vector>
#include <string>

using namespace core;

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
    using std::vector, std::string;

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
    const char** required_extension_names;
    required_extension_names = glfwGetRequiredInstanceExtensions(&required_extension_count);

    instance_create_info.enabledExtensionCount = required_extension_count;
    instance_create_info.ppEnabledExtensionNames = required_extension_names;

    vector<const char*> enabledLayers{};
    instance_create_info.enabledLayerCount = enabledLayers.size();
    instance_create_info.ppEnabledLayerNames = enabledLayers.data();

    VK_ASSERT(
        vkCreateInstance(&instance_create_info, nullptr, &instance_handle)
    );
}

void vkapp::query_physical_device(){
    
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