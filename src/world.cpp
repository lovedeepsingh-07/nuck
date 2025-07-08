#include "nuck/world.hpp"
#include "nuck/object.hpp"

namespace nuck {

// PRIVATE API ...

void World::simulate_gravity(Object* object, float delta_time) {
    if (this->enable_gravity) {
        object->force =
            Vector2Add(object->force, Vector2Scale(this->world_gravity, object->mass));
    }
}

void World::apply_dynamics(Object* object, float delta_time) {
    math::Vector2 acceleration = Vector2Scale(object->force, 1.0F / object->mass);
    object->velocity =
        Vector2Add(object->velocity, Vector2Scale(acceleration, delta_time));
    object->position =
        Vector2Add(object->position, Vector2Scale(object->velocity, delta_time));
    // reset the force on the all the objects for the next frame
    object->force = math::Vector2(0, 0);
};

// PUBLIC API ...

// TODO: lets say you create an object then get that object's reference, then when you create another object, that reference seem to be lost somehow
ObjectID World::create_object(math::Vector2 position, math::Vector2 size, float mass) {
    uint32_t curr_index = 0;
    uint32_t curr_gen = 0;


    if (m_free_indexes.empty()) {
        curr_index = m_objects.size();
        m_objects.push_back(Object{ .position = position, .size = size, .mass = mass });
        curr_gen = 0;
        m_generations.push_back(curr_gen);
    } else {
        curr_index = m_free_indexes.back();
        m_free_indexes.pop_back();
        m_objects[curr_index] = Object{ .position = position, .size = size, .mass = mass };
        curr_gen = m_generations[curr_index];
    }
    return ObjectID{ .index = curr_index, .generation = curr_gen };
}

Object* World::get_object(ObjectID object_id) {
    if (object_id.index < m_objects.size()
        && m_generations[object_id.index] == object_id.generation) {
        return &m_objects[object_id.index];
    }
    return nullptr;
}

void World::destroy_object(ObjectID object_id) {
    if (object_id.index < m_objects.size()
        && m_generations[object_id.index] == object_id.generation) {
        m_generations[object_id.index] += 1;
        m_free_indexes.push_back(object_id.index);
    }
};

void World::step(float delta_time) {
    for (ObjectID object_id : *this) {
        Object* object = this->get_object(object_id);
        if (object == nullptr || object->is_sleeping) {
            continue;
        }

        this->simulate_gravity(object, delta_time);
        this->apply_dynamics(object, delta_time);
    }
};

} // namespace nuck
