#include "renderer.hpp"
#include "raylib.h"
#include "components/components.hpp"

void setup_renderer(world& w) {
    //
}

void update_renderer(world& w) {
    ClearBackground(WHITE);

    using components::rect, components::transform;

    auto view = w.get_registry().view<const rect, const transform>();

    BeginDrawing();

        view.each([](const auto& r, const auto& t) {
            DrawRectangle(
                t.position.x, t.position.y,
                r.dimensions.x, r.dimensions.y,
                r.color
            );
        });

    EndDrawing();

    SwapScreenBuffer();    
}

void dispose_renderer(world& w) {

}

void renderer::register_renderer(world& w) {
    WORLD_REGISTER_ALL(w, renderer);
}