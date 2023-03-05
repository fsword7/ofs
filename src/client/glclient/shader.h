// shader.h - Shader Mamager package
//
// Author:  Tim Stark
// Date:    Apr 19, 2022

#pragma once

class ShaderManager;

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

    vec2Uniform &operator = (const glm::vec2 &val)
    {
        if (slot != -1)
            glUniform2fv(slot, 1, glm::value_ptr(val));
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
    }

    vec3Uniform &operator = (const glm::vec3 &val)
    {
        if (slot != -1)
            glUniform3fv(slot, 1, glm::value_ptr(val));
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

    vec4Uniform &operator = (const glm::vec4 &val)
    {
        if (slot != -1)
            glUniform4fv(slot, 1, glm::value_ptr(val));
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

    mat3Uniform &operator = (const glm::mat3 &val)
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
    }

    mat4Uniform &operator = (const glm::mat4 &val)
    {
        if (slot != -1)
            glUniformMatrix4fv(slot, 1, GL_FALSE, glm::value_ptr(val));
        return *this;
    }

private:
    GLuint slot = -1;
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
        return fmt::format("lights[{}].{}", idx, name);
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
    friend class ShaderSource;
    friend class ShaderProgram;

public:
    ShaderManager(cstr_t &folder);
    ~ShaderManager() = default;

    ShaderStatus createProgram(cstr_t &vsSource, cstr_t &fsSource, ShaderProgram **program);
    ShaderProgram *buildPrograms(cstr_t &name, cstr_t &vsSource, cstr_t &fsSource);
    ShaderProgram *createShader(cstr_t &name);

private:
    cstr_t shaderFolder;

    std::vector<ShaderProgram *> programs;
};