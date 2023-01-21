#include "./vkapp.hpp"
#include "./vkutils.hpp"

#include "utils/log.hpp"
#include "utils/list.hpp"
#include "utils/file.hpp"
#include "utils/assert.hpp"
#include "core/vulkan/vkstructs.hpp"

#include <vector>
#include <string>
#include <memory>
#include <unordered_set>

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

using namespace core;

vkapp::vkapp(GLFWwindow* window) {
    this->create_instance();
    this->query_physical_device();
    this->create_surface(window);
    family_indices = vkutils::get_queue_family_indices(physical_device, surface);
    this->create_logical_device();
    swapchain = vkswapchain(window, instance, physical_device, device, surface);
    //TODO fill in initialization order
    auto vertex_shader_source =
        utils::file::read_binary(ASSETS"shaders/basic.vert.spv");
    
    auto fragment_shader_source =
        utils::file::read_binary(ASSETS"shaders/basic.frag.spv");

    vertex_shader   = vkshader(device, vertex_shader_source, VK_SHADER_STAGE_VERTEX_BIT);
    fragment_shader = vkshader(device, fragment_shader_source, VK_SHADER_STAGE_FRAGMENT_BIT);

    std::vector<vkshader> shaders{vertex_shader, fragment_shader}; 

    pipeline = vkpipeline(device, swapchain, shaders);
}

vkapp::~vkapp() {
    // destroy every object
    pipeline.destroy(device);
    vertex_shader.destroy(device);
    fragment_shader.destroy(device);
    swapchain.destroy(device);

    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);
}

void vkapp::create_instance(){
    using std::vector;

    VkApplicationInfo app_create_info{};
    app_create_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    
    #ifdef VK_API_VERSION_1_3
        app_create_info.apiVersion = VK_API_VERSION_1_3;
        app_create_info.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
        app_create_info.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    #else 
        app_create_info.apiVersion = VK_MAKE_VERSION(1, 3, 0);
        app_create_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_create_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    #endif

    app_create_info.pApplicationName = "game";
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

    auto found_device = pick_physical_device(devices);

    ASSERT(found_device.has_value(), "Could not find Physical Device");

    physical_device = found_device.value();
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
            NULL,
            &surface
        )
    );
    ASSERT(surface != VK_NULL_HANDLE, "Surface not created succesfully")
}

void vkapp::create_image_view(){}

void vkapp::create_framebuffer(){}

void vkapp::create_render_pass(){}

void vkapp::create_pipeline(){}

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