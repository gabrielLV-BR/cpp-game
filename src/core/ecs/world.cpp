#include "world.hpp"

using namespace core;

world::world()
    : _registry(), _world_entity() 
{
    _world_entity = _registry.create();
}

world::~world() {
    tick_dispose();
}

// impls

entt::registry& world::get_registry() {
    return _registry;
}

void world::add_startup_system(system s) {
    _startup_systems.push_back(s);
}

void world::add_update_system(system s) {
    _update_systems.push_back(s);
}

void world::add_dispose_system(system s) {
    _dispose_systems.push_back(s);
}

void world::tick_startup() {
    for(auto& f : _startup_systems) f(*this);
}

void world::tick_update(float delta) {
    for(auto& f : _update_systems) f(*this);
}

void world::tick_dispose() {
    for(auto& f : _dispose_systems) f(*this);
}