// shader.h - Shader Mamager package
//
// Author:  Tim Stark
// Date:    Apr 19, 2022

#pragma once

#include "lights.h"

class ShaderManager;

enum ShaderType
{
    shrUnknown = 0,
    shrVertexProcessor,
    shrGeometryProcessor,
    shrFragmentProcessor
};

enum ShaderStatus
{
    shrSuccessful,
    shrCompileError,
    shrLinkError,
    shrOutOfMemory,
    shrEmptyProgram
};

// struct ShaderPackage
// {
//     cstr_t glslName;
//     int nFiles;
//     struct {
//         cstr_t glslFilename;
//         ShaderType glslType;
//     } list[];
// };

struct ShaderPackage
{
    cstr_t glslFilename;
    bool glslFinal;
    ShaderType glslType;
};

class Uniform
{
public:
    Uniform() = default;
    Uniform(GLuint id, cstr_t &name)
    : uName(name)
    {
        slot = glGetUniformLocation(id, name.c_str());
        glLogger->debug("{}: {}\n", name,
            slot != -1 ? fmt::format("slot {}", slot) : 
            "not used/found");
        checkError();
    }

    bool isValid() const { return slot != -1; }

protected:
    void checkError()
    {
        int err;

        if ((err = glGetError()) != GL_NO_ERROR)
            glLogger->debug("{}: OpenGL Error: {}\n", uName, err);
    }

protected:
    str_t uName;
    GLuint slot = -1;
};

class boolUniform : public Uniform
{
public:
    boolUniform() : Uniform() { }
    boolUniform(GLuint id, cstr_t &name)
    : Uniform(id, name)
    { }

    boolUniform &operator = (bool val)
    {
        if (slot != -1)
            glUniform1i(slot, val), checkError();
        return *this;
    }
};

class intUniform : public Uniform
{
public:
    intUniform() : Uniform() { }
    intUniform(GLuint id, cstr_t &name)
    : Uniform(id, name)
    { }

    intUniform &operator = (int val)
    {
        if (slot != -1)
            glUniform1i(slot, val), checkError();
        return *this;
    }
};

class floatUniform : public Uniform
{
public:
    floatUniform() : Uniform() { }
    floatUniform(GLuint id, cstr_t &name)
    : Uniform(id, name)
    { }

    floatUniform &operator = (float val)
    {
        if (slot != -1)
            glUniform1f(slot, val), checkError();
        return *this;
    }
};

class vec2Uniform : public Uniform
{
public:
    vec2Uniform() : Uniform() { }
    vec2Uniform(GLuint id, cstr_t &name)
    : Uniform(id, name)
    { }

    vec2Uniform &operator = (const glm::vec2 &val)
    {
        if (slot != -1)
            glUniform2fv(slot, 1, glm::value_ptr(val)), checkError();
        return *this;
    }
};

class vec3Uniform : public Uniform
{
public:
    vec3Uniform() : Uniform() { }
    vec3Uniform(GLuint id, cstr_t &name)
    : Uniform(id, name)
    { }

    vec3Uniform &operator = (const glm::vec3 &val)
    {
        if (slot != -1)
            glUniform3fv(slot, 1, glm::value_ptr(val)), checkError();
        return *this;
    }
};

class vec4Uniform : public Uniform
{
public:
    vec4Uniform() : Uniform() { }
    vec4Uniform(GLuint id, cstr_t &name)
    : Uniform(id, name)
    { }

    vec4Uniform &operator = (const glm::vec4 &val)
    {
        if (slot != -1)
            glUniform4fv(slot, 1, glm::value_ptr(val)), checkError();
        return *this;
    }
};

class mat3Uniform : public Uniform
{
public:
    mat3Uniform() : Uniform() { }
    mat3Uniform(GLuint id, cchar_t *name)
    : Uniform(id, name)
    { }

    mat3Uniform &operator = (const glm::mat3 &val)
    {
        if (slot != -1)
            glUniformMatrix3fv(slot, 1, GL_FALSE, glm::value_ptr(val)), checkError();
        return *this;
    }
};

class mat4Uniform : public Uniform
{
public:
    mat4Uniform() : Uniform() { }
    mat4Uniform(GLuint id, cchar_t *name)
    : Uniform(id, name)
    { }

    mat4Uniform &operator = (const glm::mat4 &val)
    {
        if (slot != -1)
            glUniformMatrix4fv(slot, 1, GL_FALSE, glm::value_ptr(val)), checkError();
        return *this;
    }
};


class ShaderSource
{
public:
    ShaderSource(ShaderManager &shmgr, ShaderType type);
    ~ShaderSource();

    GLuint getID() const { return id; }

    bool readsFile(const fs::path &fname, str_t &data);
    void dump(cstr_t &source);

    cstr_t getLogInfo();
    ShaderStatus compile(const std::vector<str_t> &source);

    static ShaderStatus create(ShaderManager *shmgr, ShaderType type, const std::vector<str_t>& source, ShaderSource **shader);

private:
    bool isWhiteSpace(char c) { return (c == '\n' || c == '\t' || c == '\r' || c == ' '); }

    inline void skipWhiteSpace(cstr_t &str, size_t &idx)
    {
        while (isWhiteSpace(str[idx]) && idx < str.size())
            idx++;
    }

    bool isComment() { return isNormalComment || isBlockComment; }
    bool checkForComment(cstr_t &str, size_t idx);
    bool parseInclude(str_t &str, size_t idx);

private:
    ShaderManager &shmgr;
    ShaderType type = shrUnknown;
    GLuint id = 0;

    bool isNormalComment = false;
    bool isBlockComment = false;
};

struct glLight
{
    boolUniform enabled;

    vec3Uniform position;

    vec4Uniform diffuse;
    vec4Uniform specular;
};

class ShaderProgram
{
public:
    ShaderProgram();
    ShaderProgram(cstr_t &name);
    ~ShaderProgram();

    void attach(const ShaderSource &shader);

    cstr_t getLogInfo();
    ShaderStatus link();
    
    inline void setName(str_t name)   { pgmName = name; }
    inline cstr_t getName() const     { return pgmName; }
    inline GLuint getID() const       { return id; }

    inline void use() const           { glUseProgram(id); }
    inline void release() const       { glUseProgram(0); }

    void initLightParameters();
    void setLightParameters(const LightState &ls);
    // color_t materialDiffuse, color_t materialSpecular, color_t materialEmissive);

    void listUniforms();
    
protected:
    str_t strLightProperty(int idx, cstr_t &name)
    {
        return fmt::format("lights[{}].{}", idx, name);
    }

private:
    str_t pgmName;
    GLuint id = 0;

    struct
    {    
        vec3Uniform spos;
        vec3Uniform diffuse;
        vec3Uniform specular;
    } lights[MAX_LIGHTS];

    vec3Uniform ambient;
    intUniform  nLights;

    // std::vector<ShaderSource &> shaders;
};

class ShaderManager
{
    friend class ShaderSource;
    friend class ShaderProgram;

public:
    ShaderManager(cstr_t &folder);
    ~ShaderManager() = default;

    ShaderProgram *createShader(cstr_t &name, const ShaderPackage list[], int size);

private:
    cstr_t shaderFolder;

    std::vector<ShaderProgram *> programs;
};