#pragma once

#include <cmath>
#include <numbers>

namespace math {

constexpr float deg_2_rad = (std::numbers::pi / 180.0F);
constexpr float rad_2_deg = (180.0F / std::numbers::pi);

struct Vector2 {
    float x;
    float y;
};

inline Vector2 Vector2Zero() {
    Vector2 result = { 0.0F, 0.0F };
    return result;
}

inline Vector2 Vector2One() {
    Vector2 result = { 1.0F, 1.0F };
    return result;
}

inline Vector2 Vector2Add(Vector2 v1, Vector2 v2) {
    Vector2 result = { v1.x + v2.x, v1.y + v2.y };
    return result;
}

inline Vector2 Vector2Subtract(Vector2 v1, Vector2 v2) {
    Vector2 result = { v1.x - v2.x, v1.y - v2.y };
    return result;
}

inline float Vector2Length(Vector2 v) {
    float result = sqrtf((v.x * v.x) + (v.y * v.y));
    return result;
}

inline float Vector2LengthSquared(Vector2 v) {
    float result = (v.x * v.x) + (v.y * v.y);
    return result;
}

inline float Vector2DotProduct(Vector2 v1, Vector2 v2) {
    float result = ((v1.x * v2.x) + (v1.y * v2.y));
    return result;
}

inline float Vector2Distance(Vector2 v1, Vector2 v2) {
    float result =
        sqrtf(((v1.x - v2.x) * (v1.x - v2.x)) + ((v1.y - v2.y) * (v1.y - v2.y)));
    return result;
}

inline float Vector2Angle(Vector2 v1, Vector2 v2) {
    double result = atan2f(v2.y - v1.y, v2.x - v1.x) * (180.0F / std::numbers::pi);
    if (result < 0) {
        result += 360.0F;
    }
    return (float)result;
}

inline Vector2 Vector2Scale(Vector2 v, float scale) {
    Vector2 result = { v.x * scale, v.y * scale };
    return result;
}

inline Vector2 Vector2MultiplyV(Vector2 v1, Vector2 v2) {
    Vector2 result = { v1.x * v2.x, v1.y * v2.y };
    return result;
}

inline Vector2 Vector2Negate(Vector2 v) {
    Vector2 result = { -v.x, -v.y };
    return result;
}

inline Vector2 Vector2Divide(Vector2 v, float div) {
    Vector2 result = { v.x / div, v.y / div };
    return result;
}

inline Vector2 Vector2DivideV(Vector2 v1, Vector2 v2) {
    Vector2 result = { v1.x / v2.x, v1.y / v2.y };
    return result;
}

inline Vector2 Vector2Normalize(Vector2 v) {
    Vector2 result = Vector2Divide(v, Vector2Length(v));
    return result;
}

inline Vector2 Vector2Lerp(Vector2 v1, Vector2 v2, float amount) {
    Vector2 result = { 0 };
    result.x = v1.x + amount * (v2.x - v1.x);
    result.y = v1.y + amount * (v2.y - v1.y);
    return result;
}


} // namespace math
