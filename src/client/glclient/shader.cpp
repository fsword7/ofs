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

// void ShaderProgram::initLightParameters()
// {
//     // for (int idx = 0; idx < MAX_LIGHTS; idx++)
//     // {
//     //     lights[idx].direction = vec3Uniform(id, strLightProperty(idx, "direction"));
//     //     lights[idx].diffuse = vec3Uniform(id, strLightProperty(idx, "diffuse"));
//     //     lights[idx].specular = vec3Uniform(id, strLightProperty(idx, "specular"));
//     // }

//     GLuint blkLights;
//     GLuint bindingPoint = 1;

//     blkLights = glGetUniformBlockIndex(id, "Lights");
//     glUniformBlockBinding(id, blkLights, bindingPoint);
//     fmt::printf("Block index = %d\n", blkLights);

// }

// void ShaderProgram::setLightParameters(const LightState &ls,
//             color_t materialDiffuse, color_t materialSpecular, color_t materialEmissive)
// {

// }

// struct glsl_type_set {
//   GLenum      type;
//   const char* name;
// }
// type_set [] = {
//   GL_INVALID_ENUM,                              "invalid",
//   GL_FLOAT,                                     "float",
//   GL_FLOAT_VEC2,                                "vec2",
//   GL_FLOAT_VEC3,                                "vec3",
//   GL_FLOAT_VEC4,                                "vec4",
//   GL_DOUBLE,                                    "double",
//   GL_DOUBLE_VEC2,                               "dvec2",
//   GL_DOUBLE_VEC3,                               "dvec3",
//   GL_DOUBLE_VEC4,                               "dvec4",
//   GL_INT,                                       "int",
//   GL_INT_VEC2,                                  "ivec2",
//   GL_INT_VEC3,                                  "ivec3",
//   GL_INT_VEC4,                                  "ivec4",
//   GL_UNSIGNED_INT,                              "unsigned int",
//   GL_UNSIGNED_INT_VEC2,                         "uvec2",
//   GL_UNSIGNED_INT_VEC3,                         "uvec3",
//   GL_UNSIGNED_INT_VEC4,                         "uvec4",
//   GL_BOOL,                                      "bool",
//   GL_BOOL_VEC2,                                 "bvec2",
//   GL_BOOL_VEC3,                                 "bvec3",
//   GL_BOOL_VEC4,                                 "bvec4",
//   GL_FLOAT_MAT2,                                "mat2",
//   GL_FLOAT_MAT3,                                "mat3",
//   GL_FLOAT_MAT4,                                "mat4",
//   GL_FLOAT_MAT2x3,                              "mat2x3",
//   GL_FLOAT_MAT2x4,                              "mat2x4",
//   GL_FLOAT_MAT3x2,                              "mat3x2",
//   GL_FLOAT_MAT3x4,                              "mat3x4",
//   GL_FLOAT_MAT4x2,                              "mat4x2",
//   GL_FLOAT_MAT4x3,                              "mat4x3",
//   GL_DOUBLE_MAT2,                               "dmat2",
//   GL_DOUBLE_MAT3,                               "dmat3",
//   GL_DOUBLE_MAT4,                               "dmat4",
//   GL_DOUBLE_MAT2x3,                             "dmat2x3",
//   GL_DOUBLE_MAT2x4,                             "dmat2x4",
//   GL_DOUBLE_MAT3x2,                             "dmat3x2",
//   GL_DOUBLE_MAT3x4,                             "dmat3x4",
//   GL_DOUBLE_MAT4x2,                             "dmat4x2",
//   GL_DOUBLE_MAT4x3,                             "dmat4x3",
//   GL_SAMPLER_1D,                                "sampler1D",
//   GL_SAMPLER_2D,                                "sampler2D",
//   GL_SAMPLER_3D,                                "sampler3D",
//   GL_SAMPLER_CUBE,                              "samplerCube",
//   GL_SAMPLER_1D_SHADOW,                         "sampler1DShadow",
//   GL_SAMPLER_2D_SHADOW,                         "sampler2DShadow",
//   GL_SAMPLER_1D_ARRAY,                          "sampler1DArray",
//   GL_SAMPLER_2D_ARRAY,                          "sampler2DArray",
//   GL_SAMPLER_1D_ARRAY_SHADOW,                   "sampler1DArrayShadow",
//   GL_SAMPLER_2D_ARRAY_SHADOW,                   "sampler2DArrayShadow",
//   GL_SAMPLER_2D_MULTISAMPLE,                    "sampler2DMS",
//   GL_SAMPLER_2D_MULTISAMPLE_ARRAY,              "sampler2DMSArray",
//   GL_SAMPLER_CUBE_SHADOW,                       "samplerCubeShadow",
//   GL_SAMPLER_BUFFER,                            "samplerBuffer",
//   GL_SAMPLER_2D_RECT,                           "sampler2DRect",
//   GL_SAMPLER_2D_RECT_SHADOW,                    "sampler2DRectShadow",
//   GL_INT_SAMPLER_1D,                            "isampler1D",
//   GL_INT_SAMPLER_2D,                            "isampler2D",
//   GL_INT_SAMPLER_3D,                            "isampler3D",
//   GL_INT_SAMPLER_CUBE,                          "isamplerCube",
//   GL_INT_SAMPLER_1D_ARRAY,                      "isampler1DArray",
//   GL_INT_SAMPLER_2D_ARRAY,                      "isampler2DArray",
//   GL_INT_SAMPLER_2D_MULTISAMPLE,                "isampler2DMS",
//   GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY,          "isampler2DMSArray",
//   GL_INT_SAMPLER_BUFFER,                        "isamplerBuffer",
//   GL_INT_SAMPLER_2D_RECT,                       "isampler2DRect",
//   GL_UNSIGNED_INT_SAMPLER_1D,                   "usampler1D",
//   GL_UNSIGNED_INT_SAMPLER_2D,                   "usampler2D",
//   GL_UNSIGNED_INT_SAMPLER_3D,                   "usampler3D",
//   GL_UNSIGNED_INT_SAMPLER_CUBE,                 "usamplerCube",
//   GL_UNSIGNED_INT_SAMPLER_1D_ARRAY,             "usampler2DArray",
//   GL_UNSIGNED_INT_SAMPLER_2D_ARRAY,             "usampler2DArray",
//   GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE,       "usampler2DMS",
//   GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY, "usampler2DMSArray",
//   GL_UNSIGNED_INT_SAMPLER_BUFFER,               "usamplerBuffer",
//   GL_UNSIGNED_INT_SAMPLER_2D_RECT,              "usampler2DRect",
//   GL_IMAGE_1D,                                  "image1D",
//   GL_IMAGE_2D,                                  "image2D",
//   GL_IMAGE_3D,                                  "image3D",
//   GL_IMAGE_2D_RECT,                             "image2DRect",
//   GL_IMAGE_CUBE,                                "imageCube",
//   GL_IMAGE_BUFFER,                              "imageBuffer",
//   GL_IMAGE_1D_ARRAY,                            "image1DArray",
//   GL_IMAGE_2D_ARRAY,                            "image2DArray",
//   GL_IMAGE_2D_MULTISAMPLE,                      "image2DMS",
//   GL_IMAGE_2D_MULTISAMPLE_ARRAY,                "image2DMSArray",
//   GL_INT_IMAGE_1D,                              "iimage1D",
//   GL_INT_IMAGE_2D,                              "iimage2D",
//   GL_INT_IMAGE_3D,                              "iimage3D",
//   GL_INT_IMAGE_2D_RECT,                         "iimage2DRect",
//   GL_INT_IMAGE_CUBE,                            "iimageCube",
//   GL_INT_IMAGE_BUFFER,                          "iimageBuffer",
//   GL_INT_IMAGE_1D_ARRAY,                        "iimage1DArray",
//   GL_INT_IMAGE_2D_ARRAY,                        "iimage2DArray",
//   GL_INT_IMAGE_2D_MULTISAMPLE,                  "iimage2DMS",
//   GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY,            "iimage2DMSArray",
//   GL_UNSIGNED_INT_IMAGE_1D,                     "uimage1D",
//   GL_UNSIGNED_INT_IMAGE_2D,                     "uimage2D",
//   GL_UNSIGNED_INT_IMAGE_3D,                     "uimage3D",
//   GL_UNSIGNED_INT_IMAGE_2D_RECT,                "uimage2DRect",
//   GL_UNSIGNED_INT_IMAGE_CUBE,                   "uimageCube",
//   GL_UNSIGNED_INT_IMAGE_BUFFER,                 "uimageBuffer",
//   GL_UNSIGNED_INT_IMAGE_1D_ARRAY,               "uimage1DArray",
//   GL_UNSIGNED_INT_IMAGE_2D_ARRAY,               "uimage2DArray",
//   GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE,         "uimage2DMS",
//   GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY,   "uimage2DMSArray",
//   GL_UNSIGNED_INT_ATOMIC_COUNTER,               "atomic_uint"
// };

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