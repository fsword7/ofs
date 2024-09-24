// shader.cpp - Shader package for OpenGL interface
//
// Author:  Tim Stark
// Date:    Jan 21, 2022

#include "main/core.h"
#include "client.h"
#include "shader.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// ******** Shader Source ********

ShaderSource::ShaderSource(ShaderManager &shmgr, ShaderType type)
: shmgr(shmgr)
{
    GLenum idType;

    switch (type)
    {
        case shrVertexProcessor:
            idType = GL_VERTEX_SHADER;
            break;
        case shrFragmentProcessor:
            idType = GL_FRAGMENT_SHADER;
            break;
        default:
            return;
    }

    id = glCreateShader(idType);
}

ShaderSource::~ShaderSource()
{
    glDeleteShader(id);
}

bool ShaderSource::readsFile(const fs::path &fname, std::string &data)
{
    fs::path fullPath = shmgr.shaderFolder / fname;
    std::ifstream file(fullPath.string().c_str());

    if (!file.good())
        return false;

    auto size = fs::file_size(fullPath);
    data = str_t(size, '\0');
    file.read(&data[0], size);
    file.close();

    return true;
}

bool ShaderSource::checkForComment(cstr_t &str, size_t idx)
{
    // Check for any line comments
    if (str[idx] == '/' && str[idx+1] == '/')
    {
        isNormalComment = true;
        return true;
    }
    else if (str[idx] == '\n')
        isNormalComment = false;

    // Check for any block comments
    else if (str[idx] == '/' && str[idx+1] == '*')
    {
        isBlockComment = true;
        return true;
    }
    else if (str[idx] == '*' && str[idx+1] == '/')
        isBlockComment = false;

    return false;
}

bool ShaderSource::parseInclude(str_t &str, size_t idx)
{
    size_t  start = idx;
    cchar_t include[10] = "#include";

    for (int idx0 = 0; include[idx0] != '\0'; idx0++)
    {
        if (str[idx] == '\0')
            return false;
        if (str[idx++] != include[idx0])
            return false;
    }

    skipWhiteSpace(str, idx);
    if (str[idx] == '\0')
        return false;

    if (str[idx++] != '\"')
        return false;

    std::string fname = "";
    while (str[idx] != '\"' && str[idx] != '\n')
    {
        if (isWhiteSpace(str[idx]))
            return false;
        fname += str[idx++];
    }
    if (str[idx] != '\"')
        return false;

    str.erase(start, idx + 1 - start);
    str_t data;
    if (readsFile(fname, data))
    {
        // fmt::printf("Include file: %s\n", fname);
        // dump(data);
        while(data.back() == '\0')
            data.pop_back();

        if (data.length())
            str.insert(start, data.c_str());
    }

    return true;
}

void ShaderSource::dump(cstr_t &source)
{
    bool newLine = true;
    int lineNumber = 0;

    for (int idx = 0; idx < source.length(); idx++)
    {
        if (newLine == true)
        {
            lineNumber++;
            glLogger->verbose("{:04d}: ", lineNumber);
            newLine = false;
        }

        glLogger->verbose("{}", source[idx]);
        if (source[idx] == '\n')
            newLine = true;
    }
    glLogger->verbose("\n\n");
}

cstr_t ShaderSource::getLogInfo()
{
    GLint lsize = 0;
    GLsizei size = 0;

    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lsize);
    if (lsize < 0)
        return std::string();
    
    char *clog = new char [lsize];
    if (clog == nullptr)
        return std::string();

    glGetShaderInfoLog(id, lsize, &size, clog);
    std::string slog(clog, size);
    delete [] clog;

    return slog;
}

ShaderStatus ShaderSource::compile(const std::vector<str_t> &source)
{
    GLint status;
    str_t ssrc[source.size()];
    str_t dsrc[source.size()];

    if (source.empty())
        return shrEmptyProgram;

    const char **src = new const char *[source.size()];
 
    for (int idx = 0; idx < source.size(); idx++)
    {
        ssrc[idx] = source[idx];
        dsrc[idx] = "";

        for (int idx0 = 0; idx0 < ssrc[idx].size(); idx0++)
        {
            char ch = ssrc[idx][idx0];
            checkForComment(ssrc[idx].c_str(), idx0);
            if (!isComment())
            {
                switch (ch)
                {
                    case '#':
                        if (parseInclude(ssrc[idx], idx0))
                        {
                            idx0--;
                            continue;
                        }
                        break;
                }
            }

            dsrc[idx] += ch;
        }
        // logger->verbose("After include parser: \n");
        // dump(dsrc[idx]);
        src[idx] = dsrc[idx].c_str();
    }
 
    glShaderSource(id, source.size(), src, nullptr);
    delete [] src;

    glCompileShader(id);

    glGetShaderiv(id, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
        return shrCompileError;
    return shrSuccessful;
}

ShaderStatus ShaderSource::create(ShaderManager *shmgr, ShaderType type, const std::vector<str_t>& source, ShaderSource **shader)
{
    ShaderSource *newShader = new ShaderSource(*shmgr, type);
    if (newShader == nullptr)
        return shrOutOfMemory;

    ShaderStatus status;
    str_t log;

    // for (unsigned int idx = 0; idx < source.size(); idx++)
    //     newShader->dump(source[idx]);

    status = newShader->compile(source);
    if (status != shrSuccessful)
    {
        log = newShader->getLogInfo();
        glLogger->verbose("Compiling shader source error:\n\n{}\n", log);
        delete newShader;
        return status;
    }

    *shader = newShader;
    return shrSuccessful;
}

// ******** Shader Program ********

ShaderProgram::ShaderProgram()
{
    id = glCreateProgram();
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(id);
}

void ShaderProgram::attach(const ShaderSource &shader)
{
    glAttachShader(id, shader.getID());
}

cstr_t ShaderProgram::getLogInfo()
{
    GLint lsize = 0;
    GLsizei size = 0;

    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &lsize);
    if (lsize < 0)
        return std::string();
    
    char *clog = new char [lsize];
    if (clog == nullptr)
        return std::string();

    glGetProgramInfoLog(id, lsize, &size, clog);
    std::string slog(clog, size);
    delete [] clog;

    return slog;
}

ShaderStatus ShaderProgram::link()
{
    GLint status;
    str_t log;

    glLinkProgram(id);

    glGetProgramiv(id, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        log = getLogInfo();
        glLogger->verbose("Linking shader program error:\n\n{}\n", log);
        return shrLinkError;
    }

    return shrSuccessful;
}

void ShaderProgram::initLightParameters()
{
    ambient = vec3Uniform(id, "uAmbient");
    nLights = intUniform(id, "unLights");

    for (int idx = 0; idx < MAX_LIGHTS; idx++)
    {
        lights[idx].spos = vec3Uniform(id, strLightProperty(idx, "spos"));
        lights[idx].diffuse = vec3Uniform(id, strLightProperty(idx, "diffuse"));
        lights[idx].specular = vec3Uniform(id, strLightProperty(idx, "specular"));

    }

    //     GLuint blkLights;
    //     GLuint bindingPoint = 1;

    //     blkLights = glGetUniformBlockIndex(id, "Lights");
    //     glUniformBlockBinding(id, blkLights, bindingPoint);
    //     fmt::printf("Block index = %d\n", blkLights);

}

void ShaderProgram::setLightParameters(const LightState &ls)
{
    assert (ls.nLights < MAX_LIGHTS);
    int idx;

    // glLogger->debug("Set {} lights\n", ls.nLights);
    nLights = ls.nLights;
    ambient = ls.ambientColor;
    for (idx = 0; idx < ls.nLights; idx++)
    {
        lights[idx].spos = ls.lights[idx].spos;
        lights[idx].diffuse = ls.lights[idx].color.vec3() * ls.lights[idx].irradiance;
        lights[idx].specular = { 0, 0, 0 };

        // glLogger->debug("{}: Color ({}, {}, {}) Irradiance {}\n", idx, ls.lights[idx].color.getRed(),
        //     ls.lights[idx].color.getGreen(), ls.lights[idx].color.getBlue(),
        //     ls.lights[idx].irradiance);
    }
}

void ShaderProgram::listUniforms()
{
    GLint uCount;

    glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &uCount);

    GLchar name[256];

    for (int idx = 0; idx < uCount; idx++)
    {
        memset(name, '\0', 256);
        GLint size;
        GLenum type;

        glGetActiveUniform(id, idx, 255, nullptr, &size, &type, name);

        GLint location = glGetUniformLocation(id, name);

        if (size > 1)
            glLogger->verbose("Uniform %d (loc=%d):\t%d %20s %-20s <Size: %d>\n",
                idx, location, type, "-", name, size);
        else
            glLogger->verbose("Uniform %d (loc=%d):\t%d %20s %-20s\n",
                idx, location, type, "-", name);   
    }
}

// ******** Shader Manager ********

ShaderManager::ShaderManager(cstr_t &folder)
: shaderFolder(folder)
{
    programs.clear();
}

ShaderStatus ShaderManager::createProgram(cstr_t &vsSource, cstr_t &fsSource, ShaderProgram **program)
{
    std::vector<str_t> vsSourcev;
    std::vector<str_t> fsSourcev;
    ShaderSource *vsShader = nullptr;
    ShaderSource *fsShader = nullptr;
    ShaderStatus st;

    vsSourcev.push_back(vsSource);
    fsSourcev.push_back(fsSource);

    st = ShaderSource::create(this, shrVertexProcessor, vsSourcev, &vsShader);
    st = ShaderSource::create(this, shrFragmentProcessor, fsSourcev, &fsShader);
   
    ShaderProgram *npgm = new ShaderProgram();
    if (vsShader != nullptr)
        npgm->attach(*vsShader);
    if (fsShader != nullptr)
        npgm->attach(*fsShader);
    
    st = npgm->link();
    if (st == shrSuccessful)
        *program = npgm;
    return shrSuccessful;
}

ShaderProgram *ShaderManager::buildPrograms(cstr_t &name, cstr_t &vsSource, cstr_t &fsSource)
{
    ShaderProgram *program;
    ShaderStatus st;

    st = createProgram(vsSource, fsSource, &program);
    if (st != shrSuccessful || program == nullptr)
        return nullptr;
    
    program->setName(name);
    programs.push_back(program);

    return program;
}

ShaderProgram *ShaderManager::createShader(cstr_t &name)
{
    for (int idx = 0; idx < programs.size(); idx++)
        if (programs[idx]->getName() == name)
            return programs[idx];

    auto vsName = fmt::format("{}/{}.vs", shaderFolder, name);
    auto fsName = fmt::format("{}/{}.fs", shaderFolder, name);

    // fs::path p = fs::current_path();
    // std::cout << "Current path: " << p << std::endl;

    str_t vsSource, fsSource;
    struct stat st;

    if (!stat(vsName.c_str(), &st))
    {
        auto vsSize = st.st_size;
        std::ifstream vsFile(vsName);
        if (!vsFile.good())
        {
            glLogger->verbose("Failed to open '%s' file: '%s'\n",
                vsName, strerror(errno));
            return nullptr;
        }

        vsSource = std::string(vsSize, '\0');
        vsFile.read(&vsSource[0], vsSize);
        vsFile.close();
    }

    if (!stat(fsName.c_str(), &st))
    {
        auto fsSize = st.st_size;
        std::ifstream fsFile(fsName);
        if (!fsFile.good())
        {
            glLogger->verbose("Failed to open '%s' file: '%s'\n",
                fsName, strerror(errno));
            return nullptr;
        }

        fsSource = std::string(fsSize, '\0');
        fsFile.read(&fsSource[0], fsSize);
        fsFile.close();
    }

    return buildPrograms(name, vsSource, fsSource);
}