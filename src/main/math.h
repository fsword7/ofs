// math.h - Math library package
//
// Author:  Tim Stark
// Date:    Apr 15, 20222

#pragma once

// Main library section
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

// 32-bit single precision floating
typedef glm::tvec2<float> vec2f_t;
typedef glm::tvec3<float> vec3f_t;
typedef glm::tvec4<float> vec4f_t;
typedef glm::tquat<float> quatf_t;

typedef glm::tmat3x3<float> mat3f_t;
typedef glm::tmat4x4<float> mat4f_t;

// 64-bit double precision floating
typedef glm::tvec2<double> vec2d_t;
typedef glm::tvec3<double> vec3d_t;
typedef glm::tvec4<double> vec4d_t;
typedef glm::tquat<double> quatd_t;

typedef glm::tmat3x3<double> mat3d_t;
typedef glm::tmat4x4<double> mat4d_t;

constexpr static const double pi = 3.14159265358979323846;

template <typename T> inline constexpr T square(T x) { return x * x; }
template <typename T> inline constexpr T cube(T x)   { return x * x * x; }

template <typename T>
inline glm::tquat<T> xRotate(T radians)
{
    T ang = radians * T(0.5); // half angle
    return glm::tquat<T>(cos(ang), sin(ang), 0, 0);
}

template <typename T>
inline glm::tquat<T> yRotate(T radians)
{
    T ang = radians * T(0.5); // half angle
    return glm::tquat<T>(cos(ang), 0, sin(ang), 0);
}

template <typename T>
inline glm::tquat<T> zRotate(T radians)
{
    T ang = radians * T(0.5); // half angle
    return glm::tquat<T>(cos(ang), 0, 0, sin(ang));
}

// Determine area of a circle formula
template <typename T> inline constexpr T circleArea(T r)
{
    return pi * r * r;
}

// Determine area of a sphere formula
template <typename T> inline constexpr T sphereArea(T r)
{
    return 4.0 * pi * r * r;
}