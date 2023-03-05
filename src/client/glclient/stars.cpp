// stars.cpp - Procedural Star Renderer package
//
// Author:  Tim Stark
// Date:    Oct 10, 2022

#include "main/core.h"
#include "universe/universe.h"
#include "universe/star.h"
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
    vao = new VertexArray(1);
    vbo = (VertexBuffer *)vao->create(1, VertexArray::VBO);
}

void StarVertex::start()
{
    pgm->use();
    vao->bind();

    vbo->allocate(120'000 * sizeof(StarVertex), nullptr, GL_DYNAMIC_DRAW);
    vertices = reinterpret_cast<starVertex *>(vbo->map());
    if (vertices == nullptr)
    {
        logger->fatal("Can't render stars - aborted (error code: {})\n",
            glGetError());
        pgm->release();
        vao->unbind();
        return;
    }

    glEnable(GL_PROGRAM_POINT_SIZE);

    glm::dmat4 view = scene.getCamera()->getViewMatrix();
    glm::dmat4 proj = scene.getCamera()->getProjMatrix();

    // Set infinity perpsective (hack)
    // proj[2][2] = 1.0;
    // proj[3][2] = -1.0;

    // logger->debug("View Matrix:\n");
    // logger->debug("{} {} {} {}\n", view[0][0], view[0][1], view[0][2], view[0][3]);
    // logger->debug("{} {} {} {}\n", view[1][0], view[1][1], view[1][2], view[1][3]);
    // logger->debug("{} {} {} {}\n", view[2][0], view[2][1], view[2][2], view[2][3]);
    // logger->debug("{} {} {} {}\n", view[3][0], view[3][1], view[3][2], view[3][3]);

    // logger->debug("Projection Matrix:\n");
    // logger->debug("{} {} {} {}\n", proj[0][0], proj[0][1], proj[0][2], proj[0][3]);
    // logger->debug("{} {} {} {}\n", proj[1][0], proj[1][1], proj[1][2], proj[1][3]);
    // logger->debug("{} {} {} {}\n", proj[2][0], proj[2][1], proj[2][2], proj[2][3]);
    // logger->debug("{} {} {} {}\n", proj[3][0], proj[3][1], proj[3][2], proj[3][3]);

    mvp = glm::mat4(proj * view);

    nStars = 0;
    type = useSprites;
    flagStarted = true;
}

void StarVertex::render()
{
    vbo->unmap();
    vertices = nullptr;

    // logger->info("Display {} stars on the screen\n", nStars);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(starVertex), (void *)0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(starVertex), (void *)(3 * sizeof(float)));
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(starVertex), (void *)(7 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
 
    glDrawArrays(GL_POINTS, 0, nStars);
    nStars = 0;
    scene.checkErrors();

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

        vao->unbind();
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

void StarRenderer::process(CelestialStar &star, double dist, double appMag) const
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
    spos  = star.getStarPosition() * KM_PER_PC;
    rpos  = spos - cpos;
    rdist = glm::length(rpos);

    // Calculate apparent size of star in view field
    srad    = star.getRadius();
    objSize = ((srad / rdist) * 2.0) / pxSize;

    // Determine color temperature
    color   = starColors->lookup(star.getTemperature());

    if (objSize > pxSize)
    {
        discSize = objSize;

        ObjectListEntry ole;

        ole.object  = &star;
        ole.visual  = scene->getVisualObject(ole.object);
    
        ole.spos    = spos;
        ole.opos    = spos;
        ole.vdist   = rdist;
        ole.objSize = objSize;
        ole.appMag  = appMag;
        ole.color   = color;

        ole.zCenter = 0.0;
        ole.zFar    = 1e10;
        ole.zNear   = 0.1;

        scene->addRenderList(ole);
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

        color.setAlpha(alpha);

        // logger->info("HIP {}: {} {} {}\n",
        //     ofsGetObjectStarHIPNumber(star), rpos.x, rpos.y, rpos.z);
        // logger->info("HIP {}: Color ({} {} {} {}) Size {}\n",
        //     ofsGetObjectStarHIPNumber(star), color.getRed(), color.getGreen(), color.getBlue(),
        //     color.getAlpha(), discSize);

        starBuffer->addStar(rpos, color, discSize);
    }
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

void Scene::renderStars(double faintest)
{
    glm::dvec3 obs = camera->getGlobalPosition();
    glm::dmat3 rot = camera->getGlobalRotation();
    double fov = camera->getFOV();
    double aspect = camera->getAspect();

    starRenderer->cpos = obs;
    starRenderer->pxSize = pixelSize;
    starRenderer->baseSize = 5.0;
    starRenderer->faintestMag = faintestMag;
    starRenderer->faintestNightMag = faintest;
    starRenderer->saturationMag = saturationMag;
    starRenderer->starBuffer->start();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    universe->findVisibleStars(*starRenderer, obs, rot, fov, aspect, faintest);
    starRenderer->starBuffer->finish();
    glDisable(GL_BLEND);
}
