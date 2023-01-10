#pragma once

// i know i could just include vulkan.h directly
// but i'd rather stay consistent
// (this phrase might be a double-edged knife)
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "utils/list.hpp"

#include <vector>
#include <string>
#include <unordered_set>
#include <string.h>

namespace vkutils {
    bool check_validation_layers_support(
        std::vector<const char*> validation_layers
    ) {
        using std::vector, std::string, std::unordered_set;

        uint32_t layer_count = 0;
        vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
        
        vector<VkLayerProperties> available_layers(layer_count);
        vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

        unordered_set<string> required_layer_names(
            validation_layers.begin(),
            validation_layers.end()
        );

        for(auto& available_layer : available_layers) {
            required_layer_names.erase(available_layer.layerName);
        }

        return required_layer_names.empty();
    }

    bool check_device_extension_support(
        VkPhysicalDevice device, std::vector<const char*> extension_names
    ) {
        using std::vector, std::unordered_set, std::string;

        uint32_t available_extension_count;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &available_extension_count, nullptr);

        vector<VkExtensionProperties> extension_properties(available_extension_count);
        vkEnumerateDeviceExtensionProperties(device, nullptr, nullptr, extension_properties.data());

        unordered_set<string> required_extensions(extension_names.begin(), extension_names.end());

        for(auto& property : extension_properties) {
            required_extensions.erase(property.extensionName);
        }

        return required_extensions.empty();
    }
}