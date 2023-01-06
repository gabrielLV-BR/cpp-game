#include "player.hpp"
#include "components/components.hpp"
#include "components/structs.hpp"

#include <iostream>

void setup_player(world& w) {
    using structs::vector3, structs::dimension2D, structs::color;

    auto& registry = w.get_registry();
    auto entity = registry.create();

    registry.emplace<components::transform>(
        entity, 
        vector3 { 10, 10, 10 }, // posget
        vector3 { 1, 1, 1 }, // scl
        vector3 { 0, 0, 0 }
    );

    registry.emplace<components::rect>(
        entity,
        dimension2D{ 20, 20 }, color{ 255, 255, 255, 255 }
    );
}

void update_player(world& w) {
    auto& registry = w.get_registry();
    auto view = registry.view<const components::transform>();

    for (auto [entity, transform] : view.each()) {
        std::cout << "player at X : " << transform.position.x << "\n";
    }
}

void dispose_player(world& w) {
    
}

void player::register_player(world& w) {
   w.add_startup_system(setup_player);
   w.add_update_system(update_player);
   w.add_dispose_system(dispose_player);
}