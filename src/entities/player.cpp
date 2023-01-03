#include "player.hpp"
#include "components/components.hpp"

#include "raymath.h"

#include <iostream>

void setup_player(world& w) {
    auto& registry = w.get_registry();
    auto entity = registry.create();

    registry.emplace<components::transform>(
        entity, 
        Vector3 { 10, 10, 10 }, // posget
        Vector3 { 1, 1, 1 }, // scl
        QuaternionIdentity() // rot
    );

    registry.emplace<components::rect>(
        entity,
        20, 20, RED
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