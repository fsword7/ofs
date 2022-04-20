// shader.h - Shader Mamager package
//
// Author:  Tim Stark
// Date:    Apr 19, 2022

#pragma once

class Context;

enum ShaderType
{
    shrUnknown = 0,
    shrVertexProcessor,
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

class intUniform
{
public:
    intUniform() = default;
    intUniform(GLuint id, cstr_t &name)
    {
        slot = glGetUniformLocation(id, name.c_str());
    }

    intUniform &operator = (int val)
    {
        if (slot != -1)
            glUniform1i(slot, val);
        return *this;
    }

private:
    GLuint slot = -1;
};

class floatUniform
{
public:
    floatUniform() = default;
    floatUniform(GLuint id, cstr_t &name)
    {
        slot = glGetUniformLocation(id, name.c_str());
    }

    floatUniform &operator = (float val)
    {
        if (slot != -1)
            glUniform1f(slot, val);
        return *this;
    }

private:
    GLuint slot = -1;
};

class vec2Uniform
{
public:
    vec2Uniform() = default;
    vec2Uniform(GLuint id, cstr_t &name)
    {
        slot = glGetUniformLocation(id, name.c_str());
    }

    vec2Uniform &operator = (const vec2f_t &val)
    {
        if (slot != -1)
            glUniform2f(slot, val.x, val.y);
        return *this;
    }

private:
    GLuint slot = -1;
};

class vec3Uniform
{
public:
    vec3Uniform() = default;
    vec3Uniform(GLuint id, cstr_t &name)
    {
        slot = glGetUniformLocation(id, name.c_str());
        fmt::printf("Uniform slot = %d (program ID %d): %s\n", slot, id, name);
    }

    vec3Uniform &operator = (const vec3f_t &val)
    {
        if (slot != -1)
            glUniform3f(slot, val.x, val.y, val.z);
        return *this;
    }

private:
    GLuint slot = -1;
};

class vec4Uniform
{
public:
    vec4Uniform() = default;
    vec4Uniform(GLuint id, cstr_t &name)
    {
        slot = glGetUniformLocation(id, name.c_str());
    }

    vec4Uniform &operator = (const vec4f_t &val)
    {
        if (slot != -1)
            glUniform4f(slot, val.x, val.y, val.z, val.w);
        return *this;
    }

private:
    GLuint slot = -1;
};

class mat3Uniform
{
public:
    mat3Uniform() = default;
    mat3Uniform(GLuint id, cstr_t &name)
    {
        slot = glGetUniformLocation(id, name.c_str());
    }

    mat3Uniform &operator = (const mat3f_t &val)
    {
        if (slot != -1)
            glUniformMatrix3fv(slot, 1, GL_FALSE, glm::value_ptr(val));
        return *this;
    }

private:
    GLuint slot = -1;
};

class mat4Uniform
{
public:
    mat4Uniform() = default;
    mat4Uniform(GLuint id, cstr_t &name)
    {
        slot = glGetUniformLocation(id, name.c_str());
        // fmt::printf("Assigned ID %d to %s\n", slot, name);
    }

    mat4Uniform &operator = (const mat4f_t &val)
    {
        if (slot != -1)
            glUniformMatrix4fv(slot, 1, GL_FALSE, glm::value_ptr(val));
        // fmt::printf("Set mat4 data on slot %d\n", slot);
        return *this;
    }

private:
    GLuint slot = -1;
};

class ShaderSource
{
public:
    ShaderSource(ShaderType type);
    ~ShaderSource();

    GLuint getID() const { return id; }

    bool readsFile(const fs::path &fname, str_t &data);
    void dump(cstr_t &source);

    cstr_t getLogInfo();
    ShaderStatus compile(const std::vector<str_t> &source);

    static ShaderStatus create(ShaderType type, const std::vector<str_t>& source, ShaderSource **shader);

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
    ShaderType type = shrUnknown;
    GLuint id = 0;

    bool isNormalComment = false;
    bool isBlockComment = false;
};

class ShaderProgram
{
public:
    ShaderProgram();
    ~ShaderProgram();

    void attach(const ShaderSource &shader);

    cstr_t getLogInfo();
    ShaderStatus link();
    
    inline void setName(str_t name)   { pgmName = name; }
    inline cstr_t getName() const     { return pgmName; }
    inline GLuint getID() const       { return id; }

    inline void use() const           { glUseProgram(id); }
    inline void release() const       { glUseProgram(0); }

    // void initLightParameters();
    // void setLightParameters(const LightState &ls,
    //     color_t materialDiffuse, color_t materialSpecular, color_t materialEmissive);

    void listUniforms();
    
protected:
    std::string strLightProperty(int idx, cstr_t &name)
    {
        return fmt::sprintf("lights[%d].%s", idx, name);
    }

private:
    str_t pgmName;
    GLuint id = 0;

    // struct
    // {
    //     vec3Uniform direction;
    //     vec3Uniform diffuse;
    //     vec3Uniform specular;
    // } lights[MAX_LIGHTS];

    // std::vector<ShaderSource &> shaders;
};

class ShaderManager
{
public:
    ShaderManager(Context &gl);
    ~ShaderManager() = default;

    ShaderStatus createProgram(cstr_t &vsSource, cstr_t &fsSource, ShaderProgram **program);
    ShaderProgram *buildPrograms(cstr_t &name, cstr_t &vsSource, cstr_t &fsSource);
    ShaderProgram *createShader(cstr_t &name);

private:
    Context &gl;

    std::vector<ShaderProgram *> programs;
};