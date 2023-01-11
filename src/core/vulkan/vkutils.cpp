
#include "./vkutils.hpp"

#include "utils/list.hpp"
#include "utils/assert.hpp"
#include "core/vulkan/vkstructs.hpp"

#include <vector>
#include <string>
#include <unordered_set>
#include <string.h>

bool vkutils::check_validation_layers_support(
    std::vector<const char *> validation_layers)
{
    using std::vector, std::string, std::unordered_set;

    uint32_t layer_count = 0;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    vector<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

    unordered_set<string> required_layer_names(
        validation_layers.begin(),
        validation_layers.end());

    for (auto &available_layer : available_layers)
    {
        required_layer_names.erase(available_layer.layerName);
    }

    return required_layer_names.empty();
}

bool vkutils::check_device_extension_support(
    VkPhysicalDevice device, std::vector<const char *> extension_names)
{
    using std::vector, std::string, std::unordered_set;

    uint32_t available_extension_count;
    std::vector<VkExtensionProperties> extension_properties;

    vkEnumerateDeviceExtensionProperties(
        device,
        nullptr,
        &available_extension_count,
        nullptr);

    extension_properties.resize(available_extension_count);

    vkEnumerateDeviceExtensionProperties(
        device,
        nullptr,
        &available_extension_count,
        extension_properties.data());

    unordered_set<string> required_extensions(extension_names.begin(), extension_names.end());

    for (auto &property : extension_properties)
    {
        required_extensions.erase(property.extensionName);
    }

    return required_extensions.empty();
}

structs::queue_family_indices vkutils::get_queue_family_indices(VkPhysicalDevice physical_device, VkSurfaceKHR surface)
{
    using std::vector;

    static structs::queue_family_indices indices{};

    if (indices.is_complete())
        return indices;

    uint32_t queue_family_count;
    vkGetPhysicalDeviceQueueFamilyProperties(
        physical_device,
        &queue_family_count,
        nullptr);

    ASSERT(queue_family_count > 0, "No queue family available");

    vector<VkQueueFamilyProperties>
        queue_family_properties(queue_family_count);

    vkGetPhysicalDeviceQueueFamilyProperties(
        physical_device,
        &queue_family_count,
        queue_family_properties.data());

    int i = 0;
    for (auto &queue_family : queue_family_properties)
    {

        VkBool32 is_present_family = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(
            physical_device,
            i,
            surface,
            &is_present_family);

        if (is_present_family == VK_TRUE)
            indices.present_family = i;
        if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indices.graphics_family = i;

        i++;
    }

    ASSERT(
        indices.is_complete(),
        "Required family indices not provided entirely");

    return indices;
}