#include "renderer.hpp"
#include "components/components.hpp"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

using core::world;

void setup_renderer(world& w) {
}

void update_renderer(world& w) {
}

void dispose_renderer(world& w) {

}

void renderer::register_renderer(world& w) {
    WORLD_REGISTER_ALL(w, renderer);
}