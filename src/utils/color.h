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

private:
    // HDR color values
    float red   = 0.0f;
    float green = 0.0f;
    float blue  = 0.0f;
    float alpha = 1.0f;

public:
    static const color_t white;
};