#pragma once

#include "nuck/math.hpp"
#include "nuck/object.hpp"
#include <algorithm>
#include <cstdint>
#include <vector>

namespace nuck {
class World {
  private:
    std::vector<Object> m_objects;
    std::vector<uint32_t> m_generations;
    std::vector<uint32_t> m_free_indexes;

    void simulate_gravity(Object* object, float delta_time);
    void apply_dynamics(Object* object, float delta_time);

  public:
    bool enable_gravity{ false };
    math::Vector2 world_gravity{ math::Vector2Zero() };

    struct Iterator {
      private:
        World* m_world;
        size_t m_index;
        void advance_to_next_valid() {
            while (m_index < m_world->m_objects.size()
                   && std::ranges::find(m_world->m_free_indexes, m_index)
                       != m_world->m_free_indexes.end()) {
                m_index = m_index + 1;
            }
        }

      public:
        Iterator(World* world, size_t start) : m_world(world), m_index(start) {
            advance_to_next_valid();
        }

        bool operator!=(const Iterator& other) const {
            return m_index != other.m_index;
        }
        Iterator& operator++() {
            ++m_index;
            advance_to_next_valid();
            return *this;
        }
        ObjectID operator*() {
            return ObjectID{ static_cast<uint32_t>(m_index),
                             m_world->m_generations[m_index] };
        }
    };

    Iterator begin() {
        return Iterator(this, 0);
    }

    Iterator end() {
        return Iterator(this, m_objects.size());
    }

    ObjectID create_object(math::Vector2 position, math::Vector2 size, float mass);
    Object* get_object(ObjectID object_id);
    void destroy_object(ObjectID object_id);
    void step(float delta_time);
};
} // namespace nuck
