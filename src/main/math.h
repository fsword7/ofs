// math.h - Math library package
//
// Author:  Tim Stark
// Date:    Apr 15, 20222

#pragma once

#include <cmath>
#include <numbers>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/type_ptr.hpp>


// constexpr static const double pi   = 3.14159265358979323846;
constexpr static const double pi   = std::numbers::pi_v<double>;
constexpr static const double pi2  = (pi * 2.0);
constexpr static const double pi05 = (pi / 2.0);

template <typename T> inline constexpr T square(T x) { return x * x; }
template <typename T> inline constexpr T cube(T x)   { return x * x * x; }

// Restrict x to [-pi, pi] range
template <typename T> inline constexpr T mod2pi(T x)
{
    constexpr T tpi = pi * 2.0;

    return x - tpi * floor((x + pi) / tpi);
}

template <typename T>
inline glm::dquat xqRotate(T radians)
{
    T ang = radians * T(0.5); // half angle
    return glm::dquat(cos(ang), sin(ang), 0, 0);
}

template <typename T>
inline glm::dquat yqRotate(T radians)
{
    T ang = radians * T(0.5); // half angle
    return glm::dquat(cos(ang), 0, sin(ang), 0);
}

template <typename T>
inline glm::dquat zqRotate(T radians)
{
    T ang = radians * T(0.5); // half angle
    return glm::dquat(cos(ang), 0, 0, sin(ang));
}


// return M * v
inline glm::dvec3 mul(const glm::dmat4 &m, const glm::dvec3 &val)
{
    return glm::dvec3(
        m[0][0]*val.x + m[0][1]*val.y + m[0][2]*val.z,
        m[1][0]*val.x + m[1][1]*val.y + m[1][2]*val.z,
        m[2][0]*val.x + m[2][1]*val.y + m[2][2]*val.z
    );
}

// return M^T * v
inline glm::dvec3 tmul(const glm::dmat4 &m, const glm::dvec3 &val)
{
    return glm::dvec3(
        m[0][0]*val.x + m[1][0]*val.y + m[2][0]*val.z,
        m[0][1]*val.x + m[1][1]*val.y + m[2][1]*val.z,
        m[0][2]*val.x + m[1][2]*val.y + m[2][2]*val.z
    );
}

inline glm::dvec3 getMidpoints(const glm::dvec3 &v1, const glm::dvec3 &v2)
{
    return glm::normalize(glm::dvec3((v1.x + v2.x) / 2.0, (v1.y + v2.y) / 2.0, (v1.z + v2.z) / 2.0));
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

    template <typename T> inline constexpr T normangle(T angle)
    {
        T ang = fmod(angle, pi2);
        return (ang >= pi) ? ang - pi2 : (ang < -pi) ? ang + pi2 : ang;
    }

    template <typename T> inline constexpr T posangle(T angle)
    {
        T ang = fmod(angle, pi2);
        return (ang >= 0.0 ? ang : (ang + pi2));
    }

    // rotation matrices for right-handed rule
    template <typename T>
    inline glm::dmat3 xRotate(T radians)
    {
        double sang = sin(radians), cang = cos(radians);
        return glm::dmat3(
            { 1.0,   0.0,   0.0  },
            { 0.0,   cang, -sang },
            { 0.0,   sang,  cang }
        );
    }

    template <typename T>
    inline glm::dmat3 yRotate(T radians)
    {
        double sang = sin(radians), cang = cos(radians);
        return glm::dmat3(
            { cang,  0.0,   sang  },
            { 0.0,   1.0,   0.0   },
            {-sang,  0.0,   cang  }
        );
    }

    template <typename T>
    inline glm::dmat3 zRotate(T radians)
    {
        double sang = sin(radians), cang = cos(radians);
        return glm::dmat3(
            { cang, -sang,  0.0   },
            { sang,  cang,  0.0   },
            { 0.0 ,  0.0,   1.0   }
        );
    }

    // rotation matrices for left-handed rule
    // template <typename T>
    // inline glm::dmat3 xRotate(T radians)
    // {
    //     double sang = sin(radians), cang = cos(radians);
    //     return glm::dmat3(
    //         { 1.0,   0.0,   0.0  },
    //         { 0.0,   cang,  sang },
    //         { 0.0,  -sang,  cang }
    //     );
    // }

    // template <typename T>
    // inline glm::dmat3 yRotate(T radians)
    // {
    //     double sang = sin(radians), cang = cos(radians);
    //     return glm::dmat3(
    //         { cang,  0.0,  -sang  },
    //         { 0.0,   1.0,   0.0   },
    //         { sang,  0.0,   cang  }
    //     );
    // }

    // template <typename T>
    // inline glm::dmat3 zRotate(T radians)
    // {
    //     double sang = sin(radians), cang = cos(radians);
    //     return glm::dmat3(
    //         { cang,  sang,  0.0   },
    //         {-sang,  cang,  0.0   },
    //         { 0.0 ,  0.0,   1.0   }
    //     );
    // }

    template <typename T, typename U> inline constexpr T rotation(const glm::dvec3 &rot)
    {
        // U cosx = cos(rot.x), sinx = sin(rot.x);
        // U cosy = cos(rot.y), siny = sin(rot.y);
        // U cosz = cos(rot.z), sinz = sin(rot.z);

        // Rotation matrix using right-handed rule
        //
        //     |  1     0     0  || cosy   0  siny || cosz -sinz   0  |
        // R = |  0   cosx -sinx ||   0    1    0  || sinz  cosz   0  |
        //     |  0   sinx  cosx ||-siny   0  cosy ||   0     0    1  |

        return xRotate(rot.x) * yRotate(rot.y) * zRotate(rot.z);
    }


    // template <typename T> Eigen::Quaternion<T> lookAt(const Eigen::Matrix<T, 3, 1> &from, const Eigen::Matrix<T, 3, 1> &to,
    //     const Eigen::Matrix<T, 3, 1> &up)
    // {
    //     Eigen::Matrix<T, 3, 1> n = to - from;
    //     n.normalize();
    //     Eigen::Matrix<T, 3, 1> v = n.cross(up).normalized();
    //     Eigen::Matrix<T, 3, 1> u = v.cross(n);

    //     Eigen::Matrix<T, 3, 3> m;
    //     m.col(0) = v;
    //     m.col(1) = u;
    //     m.col(2) = -n;

    //     return Eigen::Quaternion<T>(m).conjugate();
    // }

    // template <typename T> glm::dmat4 perspective(T fov, T aspect, T zNear, T zFar)
    // {
    //     const T half = fov / static_cast<T>(2.0);
    //     const T ctg = cos(half) / sin(half); // cot(half)
    //     const T zDelta = zFar - zNear;

    //     glm::dmat4 m(1.0);
    //     m[0][0] = ctg / aspect;
    //     m[1][1] = ctg;
    //     // m[2][2] = zFar / (zNear - zFar);
    //     // m[2][3] = static_cast<T>(-1.0);
    //     // m[3][2] = zNear * zFar / (zNear - zFar);
    //     m[2][2] = -(zFar + zNear) / zDelta;
    //     m[2][3] = static_cast<T>(-2.0) * zNear * zFar / zDelta;
    //     m[3][2] = static_cast<T>(-1.0);
    //     m[3][3] = static_cast<T>(0);

    //     return m;
    // }

    // // Indefinite perspective for rendering stars
    // template <typename T> glm::dmat4 infinitePerspective(T fov, T aspect, T zNear)
    // {
    //     const T half = fov / static_cast<T>(2.0);
    //     const T ctg = cos(half) / sin(half); // cot(half)
    
    //     glm::dmat4 m(1.0);
    //     m[0][0] = ctg / aspect;
    //     m[1][1] = ctg;
    //     m[2][2] = static_cast<T>(0.0);
    //     m[2][3] = static_cast<T>(-1.0);
    //     m[3][2] = zNear;
    //     // m[2][2] = static_cast<T>(-1.0);
    //     // m[2][3] = static_cast<T>(-2.0) * zNear;
    //     // m[3][2] = static_cast<T>(-1.0);

    //     return m;
    // }

    // template <typename T> bool setProjectPerspective(const Eigen::Matrix<T, 4, 4> &mvp, const int viewport[4],
    //     const Eigen::Matrix<T, 3, 1> &from, Eigen::Matrix<T, 3, 1> &to)
    // {
    //     Eigen::Matrix<T, 4, 1> in(from.x(), from.y(), from.z(), static_cast<T>(1));
    //     Eigen::Matrix<T, 4, 1> out = mvp * in;
    //     if (out.w() == static_cast<T>(0))
    //         return false;

    //     out = out.array() / out.w();
    //     out = static_cast<T>(0.5) + out.array() * static_cast<T>(0.5);
    //     out.x() = viewport[0] + out.x() * viewport[2];
    //     out.y() = viewport[1] + out.y() * viewport[3];

    //     to = { out.x(), out.y(), out.z() };

    //     return true;
    // }

    // template <typename T> bool setProjectPerspective(const Eigen::Matrix<T, 4, 4> &dmProj, const Eigen::Matrix<T, 4, 4> &dmView,
    //     const int viewport[4], const Eigen::Matrix<T, 3, 1> &from, Eigen::Matrix<T, 3, 1> &to)
    // {
    //     Eigen::Matrix<T, 4, 1> in(from.x(), from.y(), from.z(), static_cast<T>(1));
    //     Eigen::Matrix<T, 4, 1> out = dmProj * dmView * in;

    //     // Outside of prospective area, do nothing and return with false condition.
    //     if (out.w() == static_cast<T>(0))
    //         return false;

    //     out = out.array() / out.w();
    //     out = static_cast<T>(0.5) + out.array() * static_cast<T>(0.5);

    //     out.x() = viewport[0] + out.x() * viewport[2];
    //     out.y() = viewport[1] + out.y() * viewport[3];

    //     double depth = from.x() * dmView(2, 0) +
    //                    from.y() * dmView(2, 1) +
    //                    from.z() * dmView(2, 2);
    //     out.z() = -depth;

    //     to = { out.x(), out.y(), out.z() };

    //     return true;
    // }

    // template <typename T> Eigen::Matrix<T, 4, 4> ortho(T left, T right, T top, T bottom, T zNear = -1, T zFar = 1)
    // {
    //     T rl = right - left;
    //     T tb = top - bottom;
    //     T fn = zFar - zNear;

    //     Eigen::Matrix<T, 4, 4> m;
    //     m << static_cast<T>(2)/rl, static_cast<T>(0),    static_cast<T>(0),     -(right + left) / rl,
    //          static_cast<T>(0),    static_cast<T>(2)/tb, static_cast<T>(0),     -(top + bottom)/tb,
    //          static_cast<T>(0),    static_cast<T>(0),    static_cast<T>(-2)/fn, -(zFar + zNear)/fn,
    //          static_cast<T>(0),    static_cast<T>(0),    static_cast<T>(0),     static_cast<T>(1);

    //     return m;
    // }

}
