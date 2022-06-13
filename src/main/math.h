// math.h - Math library package
//
// Author:  Tim Stark
// Date:    Apr 15, 20222

#pragma once

#include <cmath>
#include <numbers>

#include <Eigen/Core>
#include <Eigen/Geometry>

typedef Eigen::Vector2<float> vec2f_t;
typedef Eigen::Vector3<float> vec3f_t;
typedef Eigen::Vector4<float> vec4f_t;

typedef Eigen::Quaternion<float> quatf_t;

typedef Eigen::Matrix3<float> mat3f_t;
typedef Eigen::Matrix4<float> mat4f_t;


typedef Eigen::Vector2<double> vec2d_t;
typedef Eigen::Vector3<double> vec3d_t;
typedef Eigen::Vector4<double> vec4d_t;

typedef Eigen::Quaternion<double> quatd_t;

typedef Eigen::Matrix3<double> mat3d_t;
typedef Eigen::Matrix4<double> mat4d_t;


constexpr static const double pi = 3.14159265358979323846;

template <typename T> inline constexpr T square(T x) { return x * x; }
template <typename T> inline constexpr T cube(T x)   { return x * x * x; }

// Restrict x to [-pi, pi] range
template <typename T> inline constexpr T mod2pi(T x)
{
    constexpr T tpi = pi * 2.0;

    return x - tpi * floor((x + pi) / tpi);
}

template <typename T>
inline Eigen::Quaternion<T> xRotate(T radians)
{
    T ang = radians * T(0.5); // half angle
    return Eigen::Quaternion<T>(cos(ang), sin(ang), 0, 0);
}

template <typename T>
inline Eigen::Quaternion<T> yRotate(T radians)
{
    T ang = radians * T(0.5); // half angle
    return Eigen::Quaternion<T>(cos(ang), 0, sin(ang), 0);
}

template <typename T>
inline Eigen::Quaternion<T> zRotate(T radians)
{
    T ang = radians * T(0.5); // half angle
    return Eigen::Quaternion<T>(cos(ang), 0, 0, sin(ang));
}

namespace ofs
{
    template <typename T> inline constexpr T square(T x)
    {
        return x * x;
    }

    template <typename T> inline constexpr T cube(T x)
    {
        return x * x * x;
    }

    template <typename T> inline constexpr T radians(T degrees)
    {
        using std::numbers::pi_v;
        return degrees / static_cast<T>(180) * pi_v<T>;
    }

    template <typename T> inline constexpr T degrees(T radians)
    {
        using std::numbers::inv_pi_v;
        return radians * static_cast<T>(180) * inv_pi_v<T>;
    }

    // Determine area of a circle formula
    template <typename T> inline constexpr T circleArea(T r)
    {
        using std::numbers::pi_v;
        return pi_v<T> * r * r;
    }

    // Determine area of a sphere formula
    template <typename T> inline constexpr T sphereArea(T r)
    {
        using std::numbers::pi_v;
        return 4.0 * pi_v<T> * r * r;
    }

    template <typename T> Eigen::Quaternion<T> lookAt(const Eigen::Matrix<T, 3, 1> &from, const Eigen::Matrix<T, 3, 1> &to,
        const Eigen::Matrix<T, 3, 1> &up)
    {
        Eigen::Matrix<T, 3, 1> n = to - from;
        n.normalize();
        Eigen::Matrix<T, 3, 1> v = n.cross(up).normalized();
        Eigen::Matrix<T, 3, 1> u = v.cross(n);

        Eigen::Matrix<T, 3, 3> m;
        m.col(0) = v;
        m.col(1) = u;
        m.col(2) = -n;

        return Eigen::Quaternion<T>(m).conjugate();
    }

    template <typename T> Eigen::Matrix<T, 4, 4> perspective(T fovy, T aspect, T zNear, T zFar)
    {
        if (aspect == static_cast<T>(0))
            return Eigen::Matrix<T, 4, 4>::Identity();
        
        T zDelta = zFar - zNear;
        if (zDelta == static_cast<T>(0))
            return Eigen::Matrix<T, 4, 4>::Identity();

        T angle = fovy / static_cast<T>(2);
        T sine = sin(angle);
        if (sine == static_cast<T>(0))
            return Eigen::Matrix<T, 4, 4>::Identity();
        T ctg = cos(angle) / sine;

        Eigen::Matrix<T, 4, 4> m = Eigen::Matrix<T, 4, 4>::Identity();
        m(0, 0) = ctg / aspect;
        m(1, 1) = ctg;
        m(2, 2) = -(zFar + zNear) / zDelta;
        m(2, 3) = static_cast<T>(-2) * zNear * zFar / zDelta;
        m(3, 2) = static_cast<T>(-1);
        m(3, 3) = static_cast<T>(0);

        return m;
    }

    template <typename T> Eigen::Matrix<T, 4, 4> ortho(T left, T right, T top, T bottom, T zNear = -1, T zFar = 1)
    {
        T rl = right - left;
        T tb = top - bottom;
        T fn = zFar - zNear;

        Eigen::Matrix<T, 4, 4> m;
        m << static_cast<T>(2)/rl, static_cast<T>(0),    static_cast<T>(0),     -(right + left) / rl,
             static_cast<T>(0),    static_cast<T>(2)/tb, static_cast<T>(0),     -(top + bottom)/tb,
             static_cast<T>(0),    static_cast<T>(0),    static_cast<T>(-2)/fn, -(zFar + zNear)/fn,
             static_cast<T>(0),    static_cast<T>(0),    static_cast<T>(0),     static_cast<T>(1);

        return m;
    }
}
