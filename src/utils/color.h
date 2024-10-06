// color.h - HDR color package
//
// Author:  Tim Stark
// Date:    Apr 19, 2022

#pragma once

class color_t
{
public:
    color_t() = default;
    color_t(float red, float green, float blue, float alpha = 1.0f)
    : red(red), green(green), blue(blue), alpha(alpha)
    { }
    color_t(color_t rgb, float a)
    : red(rgb.red), green(rgb.green), blue(rgb.blue), alpha(a)
    { }
    ~color_t() = default;

    // Getter function calls
    inline float getRed() const    { return red;   }
    inline float getGreen() const  { return green; }
    inline float getBlue() const   { return blue;  }
    inline float getAlpha() const  { return alpha; }

    // Setter function calls
    inline void setRed(float r)    { red = r;   }
    inline void setGreen(float g)  { green = g; }
    inline void setBlue(float b)   { blue = b;  }
    inline void setAlpha(float a)  { alpha = a; }

    inline glm::fvec3  vec3() const   { return glm::fvec3(red, green, blue); }
    inline glm::fvec4  vec4() const   { return glm::fvec4(red, green, blue, alpha); }
    inline glm::dvec3  dvec3() const  { return glm::dvec3(red, green, blue); }
    inline glm::dvec4  dvec4() const  { return glm::dvec4(red, green, blue, alpha); }

    inline float &operator [] (int idx)
    {
        switch (idx)
        {
        case 0: return red;
        case 1: return green;
        case 2: return blue;
        case 3: return alpha;
        }
        throw std::out_of_range("Invalid color index");
    }

    // inline void operator = (const glm::fvec3 &vec) { red = vec.x, green = vec.y, blue = vec.z; }
    // inline void operator = (const glm::fvec4 &vec) { red = vec.x, green = vec.y, blue = vec.z, alpha = vec.w; }
    // inline void operator = (const glm::dvec3 &vec) { red = vec.x, green = vec.y, blue = vec.z; }
    // inline void operator = (const glm::dvec4 &vec) { red = vec.x, green = vec.y, blue = vec.z, alpha = vec.w; }

private:
    // HDR color values
    float red   = 0.0f;
    float green = 0.0f;
    float blue  = 0.0f;
    float alpha = 1.0f;

public:
    static const color_t white;
};