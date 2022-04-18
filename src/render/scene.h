// scene.h - Scene main rendering routines
//
// Author:  Tim Stark
// Date:    Apr 16, 2022

#pragma once

class Context;
class Universe;
class Player;
class vObject;

template <typename T>
struct TextureCoordRange
{
    T tumin, tumax;
    T tvmin, tvmax;
};

typedef TextureCoordRange<double> tcrd_t;

class Scene
{
public:
    Scene(Context &gl) : ctx(gl)
    { }
    ~Scene() = default;

    inline Context &getContext() { return ctx; }

    void init();
    void render(Player &player);

private:
    Context &ctx;

    std::vector<vObject *> vObjects;
};