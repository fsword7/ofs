// stars.cpp - Procedural Star Renderer package
//
// Author:  Tim Stark
// Date:    Oct 10, 2022

#include "main/core.h"
#include "universe/astro.h"
#include "client.h"
#include "scene.h"
#include "buffer.h"
#include "starcolors.h"
#include "stars.h"

// ******** Star Vertex ********

StarVertex::StarVertex(Scene &scene)
: scene(scene),
  type(useNotUsed), nStars(0),
  flagStarted(false)
{
    vbuf = new VertexBuffer(nullptr, 120'000 * sizeof(StarVertex));
}

void StarVertex::start()
{
    pgm->use();
    vbuf->bind();

    vertices = reinterpret_cast<starVertex *>(vbuf->map());
    if (vertices == nullptr)
    {
        logger->fatal("Can't render stars - aborted (error code: {})\n",
            glGetError());
        pgm->release();
        vbuf->unbind();
        return;
    }

    glEnable(GL_PROGRAM_POINT_SIZE);

    // mvp = (prm.dmProj * prm.dmView).cast<float>();

    nStars = 0;
    type = useSprites;
    flagStarted = true;
}

void StarVertex::render()
{
    // if (!glUnmapBuffer(GL_ARRAY_BUFFER))
    // {
    //     Logger::getLogger()->fatal("Stars: buffer corrupted - aborted (error code: {})\n", glGetError());
    //     return;
    // }
    vbuf->unmap();
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

        vbuf->unbind();
        pgm->release();
        break;

    case usePoints:
    default:
        break;
    }
}

void StarVertex::addStar(const glm::dvec3 &pos, const color_t &color, double radius)
{
    vertices[nStars].posStar = pos;
    vertices[nStars].color = color;
    vertices[nStars].size = radius;
    nStars++;
}

// ******** Star Renderer ********

void StarRenderer::process(ObjectHandle star, double dist, double appMag) const
{
    glm::dvec3 spos, rpos;
    double  srad;
    double  rdist;
    double  objSize;
    double  discSize;
    double  discScale;
    double  alpha, ptSize;
    color_t color;

    // Calculate relative position between star and
    // camera position in universal reference frame
    spos  = ofsGetObjectStarPosition(star) * KM_PER_PC;
    rpos  = spos - cpos;
    rdist = glm::length(rpos);

    // Calculate apparent size of star in view field
    srad    = ofsGetObjectRadius(star);
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

    // color = starColors->lookup(ofsGetObjectStarTemperature(star));
    color = { 0.7, 0.7, 0.7 };
    color.setAlpha(alpha);

    starBuffer->addStar(rpos, color, discSize);
}

// ******** Scene (star rendering section) ********

void Scene::initStarRenderer()
{
    starColors = new StarColors();
    starColors->load("data/stars/bbr_color_D58.txt");

    pgmStar = shmgr.createShader("point");

    StarVertex *starBuffer = new StarVertex(*this);
    starBuffer->pgm = pgmStar;
    starBuffer->mvp = mat4Uniform(pgmStar->getID(), "mvp");

    starRenderer = new StarRenderer();
    starRenderer->scene = this;
    starRenderer->starBuffer = starBuffer;
    starRenderer->starColors = starColors;
}

void Scene::renderStars(/* const StarDatabase &starlib, const Player &player, double faintest */)
{
    // vec3d_t obs = player.getuPosition();
    // quatd_t rot = player.getuOrientation();
    // Camera *cam = player.getCamera();
    // double  fov = cam->getFOV();
    // double  aspect = cam->getAspect();

    // starRenderer->cpos = obs;
    // starRenderer->pxSize = pixelSize;
    // starRenderer->baseSize = 5.0;
    // starRenderer->faintestMag = faintestMag;
    // starRenderer->faintestNightMag = faintest;
    // starRenderer->saturationMag = saturationMag;
    // starRenderer->starBuffer->start();

    // ctx.enableBlend();
    // starlib.findVisibleStars(*starRenderer, obs, rot, fov, aspect, faintest);
    // starRenderer->starBuffer->finish();
    // ctx.disableBlend();
}
