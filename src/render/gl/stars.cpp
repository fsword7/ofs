// stars.cpp - Procedural star renderer package
//
// Author:  Tim Stark
// Date:    Apr 19, 2022

#include "main/core.h"
#include "osd/gl/context.h"
#include "osd/gl/buffers.h"
#include "osd/gl/shader.h"
#include "engine/player.h"
#include "universe/star.h"
#include "universe/handle.h"
#include "universe/starlib.h"
#include "universe/astro.h"
#include "render/gl/stars.h"
#include "render/scene.h"
#include "render/starcolors.h"

// ******** Star Vertex ********

StarVertex::StarVertex(Scene &scene)
: scene(scene), ctx(scene.getContext()),
  prm(scene.getRenderParameters()),
  type(useNotUsed), nStars(0),
  flagStarted(false)
{
}

void StarVertex::start()
{
    pgm->use();
    vbuf->bind();

    glBindBuffer(GL_ARRAY_BUFFER, vbuf->getVBO());
    glBufferData(GL_ARRAY_BUFFER, 120'000 * sizeof(starVertex), nullptr, GL_STREAM_DRAW);
    vertices = reinterpret_cast<starVertex *>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
    if (vertices == nullptr)
    {
        Logger::getLogger()->fatal("Can't render stars - aborted (error code: {})\n",
            glGetError());
        pgm->release();
        vbuf->release();
        return;
    }

    glEnable(GL_PROGRAM_POINT_SIZE);

    mvp = (prm.dmProj * prm.dmView).cast<float>();

    nStars = 0;
    type = useSprites;
    flagStarted = true;
}

void StarVertex::render()
{
    if (!glUnmapBuffer(GL_ARRAY_BUFFER))
    {
        Logger::getLogger()->fatal("Stars: buffer corrupted - aborted (error code: {})\n", glGetError());
        return;
    }
    vertices = nullptr;

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(starVertex), (void *)0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(starVertex), (void *)(3 * sizeof(float)));
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(starVertex), (void *)(7 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
 
    glDrawArrays(GL_POINTS, 0, nStars);
    nStars = 0;

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void StarVertex::finish()
{
    render();

    flagStarted = false;

    switch(type)
    {
    case useSprites:
        glDisable(GL_PROGRAM_POINT_SIZE);

        vbuf->release();
        pgm->release();
        break;

    case usePoints:
    default:
        break;
    }
}

void StarVertex::addStar(const vec3d_t &pos, const color_t &color, double radius)
{
    vertices[nStars].posStar = pos.cast<float>();
    vertices[nStars].color = color;
    vertices[nStars].size = radius;
    nStars++;
}

// ******** Star Renderer ********

void StarRenderer::process(const celStar &star, double dist, double appMag) const
{
    vec3d_t spos, rpos;
    double  srad;
    double  rdist;
    double  objSize;
    double  discSize;
    double  discScale;
    double  alpha, ptSize;
    color_t color;

    // Calculate relative position between star and
    // camera position in universal reference frame
    spos  = star.getStarPosition() * KM_PER_PC;
    rpos  = spos - cpos;
    rdist = rpos.norm();

    // Calculate apparent size of star in view field
    srad    = star.getRadius();
    objSize = ((srad / rdist) * 2.0) / pxSize;

    if (objSize > pxSize)
    {
        discSize = objSize;
        alpha = 1.0;
    }
    else
    {
        alpha = faintestMag - appMag;
        discSize = baseSize;
        if (alpha > 1.0)
        {
            discScale = std::min(pow(2.0, 0.3 * (saturationMag - appMag)), 100.0);
            discSize *= discScale;
            alpha = 1.0;
        }
        else if (alpha < 0.0)
            alpha = 0.0;
    }

    color = starColors->lookup(star.getTemperature());
    color.setAlpha(alpha);

    starBuffer->addStar(rpos, color, discSize);
}

// ******** Scene (star rendering section) ********

void Scene::initStarRenderer()
{
    starColors = new StarColors();
    starColors->load("stars/bbr_color_D58.txt");

    ShaderManager &smgr = *ctx.getShaderManager();

    pgmStar = smgr.createShader("point");
    vbufStar = new VertexBuffer(ctx, 1);
    vbufStar->createBuffer(VertexBuffer::VBO, 1);

    StarVertex *starBuffer = new StarVertex(*this);
    starBuffer->pgm = pgmStar;
    starBuffer->vbuf = vbufStar;
    starBuffer->mvp = mat4Uniform(pgmStar->getID(), "mvp");

    starRenderer = new StarRenderer();
    starRenderer->scene = this;
    starRenderer->ctx = &ctx;
    starRenderer->starBuffer = starBuffer;
    starRenderer->starColors = starColors;
}

void Scene::renderStars(const StarDatabase &starlib, const Player &player, double faintest)
{
    vec3d_t obs = player.getuPosition();
    quatd_t rot = player.getuOrientation();
    Camera *cam = player.getCamera();
    double  fov = cam->getFOV();
    double  aspect = cam->getAspect();

    starRenderer->cpos = obs;
    starRenderer->pxSize = pixelSize;
    starRenderer->baseSize = 5.0;
    starRenderer->faintestMag = faintestMag;
    starRenderer->faintestNightMag = faintest;
    starRenderer->saturationMag = saturationMag;
    starRenderer->starBuffer->start();

    ctx.enableBlend();
    starlib.findVisibleStars(*starRenderer, obs, rot, fov, aspect, faintest);
    starRenderer->starBuffer->finish();
    ctx.disableBlend();
}