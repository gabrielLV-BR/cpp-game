#include "entt/entt.hpp"
#include "raylib.h"

#include "components/components.hpp"

#include "core/ecs/world.hpp"
#include "core/graphics/window.hpp"
#include "core/graphics/renderer.hpp"
#include "entities/player.hpp"

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

    InitWindow(width, height, title.c_str());
    world.insert_resource<components::window_details>(
        width, height, title
    );

    SetTargetFPS(60);

    // END WINDOWING

    world.tick_startup();

    while(!WindowShouldClose()) {
        world.tick_update(GetFrameTime());
        PollInputEvents();
    }
    world.tick_dispose();

    CloseWindow();
    return 0;
}