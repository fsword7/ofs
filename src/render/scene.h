// scene.h - Scene main rendering routines
//
// Author:  Tim Stark
// Date:    Apr 16, 2022

#pragma once

class Context;
class VertexBuffer;
class ShaderManager;
class ShaderProgram;
class Universe;
class Player;
class celStar;
class StarRenderer;
class StarColors;
class StarDatabase;
class vObject;

template <typename T>
struct TextureCoordRange
{
    T tumin, tumax;
    T tvmin, tvmax;
};

typedef TextureCoordRange<double> tcrd_t;

struct renderParam
{
    double jnow; // current Julian date/time

    // Camera paramters
    vec3d_t cpos;
    quatd_t crot;

    mat4d_t dmProj;  // projection matrix
    mat4d_t dmView;  // view matrix
    mat4d_t dmModel; // model matrix
    mat4d_t dmWorld; // Model/view matrix
    mat4d_t dmOrhto; // orthrogaphic matrix for annonations

    mat4f_t mProj;  // projection matrix
    mat4f_t mView;  // view matrix
    mat4f_t mModel; // model matrix
    mat4f_t mWorld; // model/view matrix
    
    mat4f_t mvp;    // total matrix

};

class Scene
{
public:
    Scene(Context &gl) : ctx(gl)
    { }
    ~Scene() = default;

    inline Context &getContext() { return ctx; }
    inline renderParam &getRenderParameters() { return prm; }

    void init();
    void render(Universe &universe, Player &player);

protected:
    void initStarRenderer();
    void renderStars(const StarDatabase &starlib, const Player &player, double faintest);

private:
    Context &ctx;
    ShaderManager *smgr = nullptr;

    std::vector<vObject *> vObjects;

    std::vector<const celStar *> closeStars;

    StarRenderer *starRenderer = nullptr;
    StarColors *starColors = nullptr;

    double pixelSize = 1.0;

    float faintestMag = 6.0f;
    float faintestNightMag = faintestMag;
    float saturationMag = 1.0f;

    renderParam prm;

    ShaderProgram *pgmStar = nullptr;
    VertexBuffer *vbufStar = nullptr;
};