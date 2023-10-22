// fonts.h - Font package for FreeType interface
//
// Author:  Tim Stark
// Date:    Oct 21, 2023

#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

class Scene;

class TextureFont
{
private:
    struct Glyph
    {
        char32_t ch;
        GLuint glName;

        float ax, ay;   // Advance [X, Y]
        float sx, sy;   // Bitmap [width, height]
        float bx, by;   // Bearing [left, top]
        float tx, ty;   // Texture [X, Y]
    };

public:
    TextureFont(Scene &scene);
    ~TextureFont();

    static TextureFont *load(Scene &scene, const fs::path &fname,
        int size = 12, int dpi = 96);
    
    static void ginit();
    static void gexit();
    
    inline float getHeight() const      { return maxAscent + maxDescent; }
    inline float getMaxAscent() const   { return maxAscent; }
    inline float getMaxDescent() const  { return maxDescent; }

    float getWidth(cstr_t &text);
    void render(cstr_t &text, float x, float y, const color_t &color);

private:
    void initGlyphs();

private:
    Scene &scene;
    ShaderProgram *pgm = nullptr;
    GLuint vao, vbo;

    mat4Uniform uProj;
    vec4Uniform uColor;

    FT_Face face = nullptr;
    Glyph *glyph = nullptr;

    float maxAscent;
    float maxDescent;

    // Atlas texture
    int maxTextureSize = 0;
    int texWidth = 0;
    int texHeight = 0;

    static FT_Library font;
};