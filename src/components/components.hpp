#pragma once

#include <string>
#include "./structs.hpp"

namespace components {
    struct transform {
        structs::vector3 position;
        structs::vector3 scale;
        structs::vector3 rotation;
    };

    struct label {
        const char* label;
    };

    struct window_details {
        int width, height;
        std::string title;
    };

    struct vulkan_details {};
}