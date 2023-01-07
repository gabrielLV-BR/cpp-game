#pragma once

#include "entt/entt.hpp"
#include <vector>
#include <functional>

#define WORLD_REGISTER_ALL(w, s)     \
    w.add_startup_system(setup_##s); \
    w.add_update_system(update_##s); \
    w.add_dispose_system(dispose_##s);

namespace core
{
    class world
    {
    public:
        using system = std::function<void(world &)>;
        using delta_time = float;

    private:
        entt::entity _world_entity;
        entt::registry _registry;
        std::vector<system> _startup_systems, _update_systems, _dispose_systems;

    public:
        world();
        ~world();

        entt::registry &get_registry();

        void add_startup_system(system);
        void add_update_system(system);
        void add_dispose_system(system);

        // gets resource from registry
        template <typename T>
        inline T get_resource()
        {
            // view<T>().get() returns a tuple
            // std::get<>(tuple) is a static type-safe way to get
            // elements from tuples
            return std::get<0>(_registry.view<T>().get(_world_entity));
        }

        template <typename T, typename... Args>
        void insert_resource(Args... args)
        {
            _registry.emplace_or_replace<T>(_world_entity, args...);
        }

        void tick_startup();
        void tick_update(float);
        void tick_dispose();
    };
}