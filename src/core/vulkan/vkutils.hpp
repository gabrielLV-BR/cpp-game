#pragma once

// i know i could just include vulkan.h directly
// but i'd rather stay consistent
// (this phrase might be a double-edged knife)
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <vector>
#include <string>
#include <string.h>

namespace vkutils {
    bool check_validation_layers_support(
        std::vector<const char*> validation_layers
    ) {
        using std::vector;

        uint32_t layer_count = 0;
        vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
        
        vector<VkLayerProperties> available_layers(layer_count);
        vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

        int matches = 0;
        for(auto& validation_layer : validation_layers) {
            for(auto& available_layer : available_layers) {
                if(strcmp(validation_layer, available_layer.layerName) == 0) {
                    matches++;
                    break;
                }
            }
        }

        return matches == validation_layers.size();
    }
}