// fonts.h - Font package using FreeType library for OpenGL interface
//
// Author:  Tim Stark
// Date:    Apr 24, 2022

#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

class Context;

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
    TextureFont(Context &gl) : gl(gl)
    { }
    ~TextureFont();

    static TextureFont *load(Context &gl, const fs::path &fname,
        int size = 12, int dpi = 96);
    
    static void ginit();
    static void gexit();
    
    void initGlyphs();

    float getWidth(cstr_t &text);
    void render(cstr_t &text, float x, float y, const color_t &color);
    
private:
    Context &gl;

    ShaderProgram *pgm = nullptr;
    GLuint vao, vbo;

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