// fonts.cpp - Font package for FreeType interface
//
// Author:  Tim Stark
// Date:    Oct 21, 2023

#include "main/core.h"
#include "client.h"
#include "scene.h"
#include "shader.h"
#include "fonts.h"

FT_Library TextureFont::font = nullptr;

TextureFont::TextureFont(Scene &scene)
: scene(scene)
{
}

TextureFont::~TextureFont()
{
    if (glyph != nullptr)
    {
        for (int gidx = 0; gidx < face->num_glyphs; gidx++)
            if (glyph[gidx].glName != 0)
                glDeleteTextures(1, &glyph[gidx].glName);
        delete [] glyph;
    }
    if (face != nullptr)
        FT_Done_Face(face);
}

void TextureFont::ginit()
{
    if (font != nullptr)
        return;
    if (FT_Init_FreeType(&font))
    {
        glLogger->fatal("TTF: Can't initialize freetype library - aborted\n");
    }
}

void TextureFont::gexit()
{
    if (font != nullptr)
        FT_Done_FreeType(font);
}

static ShaderPackage glslText[] = {
    { "text.vs.glsl", true, shrVertexProcessor },
    { "text.fs.glsl", true, shrFragmentProcessor }
};

void TextureFont::initGlyphs()
{
    FT_GlyphSlot slot = face->glyph;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glyph = new Glyph[face->num_glyphs];
    for (int gidx = 0; gidx < face->num_glyphs; gidx++)
    {
        if (FT_Load_Glyph(face, gidx, FT_LOAD_RENDER))
            continue;

        GLuint name;
        glGenTextures(1, &name);
        glBindTexture(GL_TEXTURE_2D, name);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
            slot->bitmap.width, slot->bitmap.rows,
            0, GL_RED, GL_UNSIGNED_BYTE,
            slot->bitmap.buffer);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glyph[gidx].glName = name;
        glyph[gidx].ax = slot->advance.x >> 6;
        glyph[gidx].ay = slot->advance.y >> 6;
        glyph[gidx].sx = slot->bitmap.width;
        glyph[gidx].sy = slot->bitmap.rows;
        glyph[gidx].bx = slot->bitmap_left;
        glyph[gidx].by = slot->bitmap_top;
    }

    // Assign UNICODE code to glyph table
    uint32_t gidx;
    char32_t ch = FT_Get_First_Char(face, &gidx);
    while (gidx != 0)
    {
        glyph[gidx].ch = ch;
        ch = FT_Get_Next_Char(face, ch, &gidx);
    }

    maxAscent = face->size->metrics.ascender >> 6;
    maxDescent = face->size->metrics.descender >> 6;

    ShaderManager &shmgr = scene.getShaderManager();

    // pgm = shmgr.createShader("text");
    pgm = shmgr.createShader("text", glslText, ARRAY_SIZE(glslText));

    pgm->use();

    uColor = vec4Uniform(pgm->getID(), "textColor");
    uProj = mat4Uniform(pgm->getID(), "proj");

    // Initialize OpenGL buffers
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    pgm->release();
}

TextureFont *TextureFont::load(Scene &scene, const fs::path &path, int size, int dpi)
{
    FT_Face face;

    if (FT_New_Face(font, path.string().c_str(), 0, &face) != 0)
    {
        glLogger->error("TFF: Can't open font %{}: {}\n",
            path.string(), strerror(errno));
        return nullptr;
    }

    if (!FT_IS_SCALABLE(face))
    {
        glLogger->error("TTF: font {} is not scalable.\n", path.string());
        FT_Done_Face(face);
        return nullptr;
    }

    if (FT_Set_Char_Size(face, 0, size << 6, dpi, dpi) != 0)
    {
        glLogger->error("TTF: Can't set {} on font {}\n",
            size, path.string());
        FT_Done_Face(face);
        return nullptr;
    }

    if (FT_Select_Charmap(face, FT_ENCODING_UNICODE) != 0)
    {
        glLogger->error("TTF: Unicode not supported on font {}\n", path.string());
        FT_Done_Face(face);
        return nullptr;
    }

    glLogger->info("TTF: Loading {} ({}) {} glyphs...\n",
        face->family_name, face->style_name, face->num_glyphs);
    
    TextureFont *font = new TextureFont(scene);

    // Initializing glyph database from font files
    font->face = face;
    font->initGlyphs();

    return font;
}

float TextureFont::getWidth(cstr_t &text)
{
    float ax = 0.0f;

    std::string::const_iterator ch;
    for (ch = text.begin(); ch != text.end(); ch++)
    {
        int gidx = FT_Get_Char_Index(face, *ch);

        ax += glyph[gidx].ax;
    }

    return ax;
}

void TextureFont::render(cstr_t &text, float x, float y, const color_t &color)
{
    if (pgm == nullptr)
        return;
    pgm->use();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vao);

    // mat4f_t proj = ofs::ortho(0.0f, float(gl.getWidth()), 0.0f, float(gl.getHeight()));
    glm::mat4 proj = glm::ortho(0.0f, float(scene.getWidth()), float(scene.getHeight()), 0.0f);

    uColor = {color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha() };
    uProj = proj;

    std::string::const_iterator ch;
    for (ch = text.begin(); ch != text.end(); ch++)
    {
        int gidx = FT_Get_Char_Index(face, *ch);

        float xpos = x + glyph[gidx].bx;
        float ypos = y - (glyph[gidx].sy - glyph[gidx].by);
        float w = glyph[gidx].sx;
        float h = glyph[gidx].sy;
        float vtx[6][4] = {
            { xpos,   ypos+h, 0.0, 0.0 },
            { xpos,   ypos,   0.0, 1.0 },
            { xpos+w, ypos,   1.0, 1.0 },
            
            { xpos,   ypos+h, 0.0, 0.0 },
            { xpos+w, ypos,   1.0, 1.0 },
            { xpos+w, ypos+h, 1.0, 0.0 },
        };

        // fmt::printf("Render '%lc' => Glyph index %d (%f,%f) at (%f,%f) Advance (%f,%f)\n",
        //     *ch, gidx, w, h, xpos, ypos, glyph[gidx].ax, glyph[gidx].ay);

        glBindTexture(GL_TEXTURE_2D, glyph[gidx].glName);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vtx), vtx);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        x += glyph[gidx].ax;
    }

    glDisable(GL_BLEND);
    pgm->release();
}