#include "entt/entt.hpp"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "components/components.hpp"

#include "core/ecs/world.hpp"
#include "core/graphics/renderer.hpp"
#include "core/vulkan/vkapp.hpp"
#include "entities/player.hpp"
#include "utils/assert.hpp"

#include <iostream>
#include <exception>

int main() {
    
    core::world world;
    
    // WINDOWING
    // GLFW windowing

    ASSERT(glfwInit() == GLFW_TRUE, "Error initializing GLFW");

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    int width = 500;
    int height = 500;
    std::string title = "Hello, World!";
    GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), 0, 0);

    world.insert_resource<components::window_details>(width, height, title);

    // VULKAN context

    core::vkapp vulkan_app(window);
    world.insert_resource<components::vulkan_details>(vulkan_app.get_details());
    
    // ENTITIES & COMPONENTS

    player::register_player(world);
    renderer::register_renderer(world);

    // Main loop

    const float TARGET_FPS = 60;
    world.tick_startup();
    while(glfwWindowShouldClose(window) != GLFW_TRUE) {
        world.tick_update(1/TARGET_FPS);
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    // Cleanup

    world.tick_dispose();
    glfwDestroyWindow(window);
    
    return 0;
}