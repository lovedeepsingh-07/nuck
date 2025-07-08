#pragma once

#include "nuck/math.hpp"
#include <cstdint>

namespace nuck {

struct Object {
    math::Vector2 position;
    math::Vector2 size;
    math::Vector2 velocity;
    math::Vector2 force;
    float mass;
    bool is_sleeping = false;
    float sleep_timer = 0.0F;
};
struct ObjectID {
    uint32_t index;
    uint32_t generation;
};
inline bool operator==(ObjectID one, ObjectID other) {
    return one.index == other.index && one.generation == other.generation;
}
inline bool operator!=(ObjectID one, ObjectID other) {
    return one.index != other.index || one.generation != other.generation;
}

} // namespace nuck
