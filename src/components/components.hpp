#pragma once

#include "raylib.h"
#include "raymath.h"

#include <string>

namespace components {
    struct transform {
        Vector3 position;
        Vector3 scale;
        Quaternion rotation;
    };

    struct label {
        const char* label;
    };

    struct window_details {
        int width, height;
        std::string title;
    };

    struct rect {
        Vector2 dimensions;
        Color color;
    };
}