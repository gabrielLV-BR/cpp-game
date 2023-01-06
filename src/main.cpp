#include "entt/entt.hpp"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "components/components.hpp"

#include "core/ecs/world.hpp"
#include "core/graphics/renderer.hpp"
#include "entities/player.hpp"
#include "utils/assert.hpp"

#include <iostream>
#include <exception>

int main() {
    world world;

    // REGISTER ENTITIES & COMPONENTS

    player::register_player(world);
    renderer::register_renderer(world);

    // WINDOWING
    // doing it outside of a system as it manages a global instance

    int width = 500;
    int height = 500;
    std::string title = "Hello, World!";

    ASSERT(glfwInit() == GLFW_TRUE, "Error initializing GLFW");

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), 0, 0);
    world.insert_resource<components::window_details>(width, height, title);

    const float TARGET_FPS = 60;

    // END WINDOWING
    //
    //

    world.tick_startup();

    while(glfwWindowShouldClose(window) != GLFW_TRUE) {
        world.tick_update(1/60);
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    world.tick_dispose();

    glfwDestroyWindow(window);
    return 0;
}