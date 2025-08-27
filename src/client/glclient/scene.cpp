// scene.cpp - Scene renderer package
//
// Author:  Tim Stark
// Date:    Sep 3, 2022

#include "main/core.h"
#include "engine/object.h"
#include "engine/player.h"
#include "universe/universe.h"
#include "universe/celbody.h"
#include "universe/star.h"

#include "client.h"
// #include "camera.h"
#include "scene.h"
#include "vobject.h"

Scene::Scene(int width, int height)
: shmgr(ofsPath.generic_string() + "/shaders/gl")
{
    resize(width, height);
}

void Scene::checkErrors()
{
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR)
    {
        glLogger->debug("OpenGL Error: {}\n", err);
    }
}

void Scene::init(Universe *uv)
{
    universe = uv;

    vobjList.clear();

    initConstellations();
    initStarRenderer();
}

void Scene::start()
{

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CW);
    glEnable(GL_CULL_FACE);

    // ObjectHandle earth = ofsGetObjectByName("Sol/Earth");
    // if (earth != nullptr)
    //     vEarth = addVisualObject(earth);
    // else
    //     printf("Can't find planet Earth for visual object\n");
}

void Scene::resize(int w, int h)
{
    width = w;
    height = h;
    
    glViewport(0, 0, width, height);
}

glm::dvec3 Scene::getAstrocentricPosition(const Object *object, const glm::dvec3 &vpos, int time)
{
    return vpos - object->getgPosition();
}

void Scene::updateAutoMag()
{
    double fieldCorr = camera->getFieldCorrection();
    faintestMag = faintestAutoMag45Deg * sqrt(fieldCorr);
    saturationMag = saturationMagNight * (1.0 + fieldCorr * fieldCorr);
}

void Scene::calculatePointSize(double appMag, double size, double &pointSize, double &alpha)
{

    alpha = std::max(0.0, (faintestMag - appMag)); // * brightnessScale + brightnessBias);
    // glareAlpha = 0;
    // glareSize = 0;

    pointSize = size;
    if (alpha > 1.0)
    {
        // if (starStyle == useScaledStars)
        // {
        //     double starScale = std::min(maxScaledStarSize, pow(2.0, 0.3 * (satPoint - appMag)));
        //     pointSize *= std::max(1.0, starScale);

        //     // glareAlpha = std::min(0.5, starScale / 4.0);
        //     // glareSize = pointSize * 3.0;
        // }
        // else
        {
            // double starScale = std::min(100.0, satPoint - appMag + 2.0);

            // glareAlpha = std::min(glareOpacity, (starScale - 2.0) / 4.0);
            // glareSize = 2.0 * pointScale * size;
        }
        alpha = 1.0;
    }
}

void Scene::update(Player *player)
{
    observer = player;
    camera = player->getCamera();

    // pixelSize = (2.0 * tan(camera->getFOV() / 2.0)) / camera->getHeight();
    pixelSize = camera->getPixelSize();

    mjd = player->getTimeDate()->getMJD0();
    now = player->getTimeDate()->getSimTime0();

    visibleStars.clear();
    renderList.clear();
    lightSources.clear();
    secondaryLights.clear();

    nearStars = universe->getNearStars();
    setupPrimaryLightSources(nearStars, observer->getPosition(), lightSources);

    for (auto star : nearStars)
    {
        vObject *vstar = getVisualObject(star, true);
    }
}

void Scene::render(Player *player)
{
    // update(player);

    // Clear all framebuffer
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    renderConstellations();
    renderStars(faintestMag, mjd);

    glm::dvec3 obs = observer->getPosition(); //  camera->getGlobalPosition();

    // for (auto sun : nearStars)
    // {
    //     // logger->info("Sun: {}\n", ofsGetObjectName(sun));

    //     // if (!sun->hasSolarSystem())
    //     //     continue;
    //     // System *system = sun->getSolarSystem(); 

    //     // PlanetarySystem *objects = system->getPlanetarySystem();
    //     // FrameTree *tree = objects->getSystemTree();

    //     if (!sun->haspSystem())
    //         continue;
    //     pSystem *psys = sun->getpSystem();

    //     glm::dvec3 apos = getAstrocentricPosition(sun, obs, now);
    //     glm::dvec3 vpn = camera->getGlobalRotation() * glm::dvec3(0, 0, -1);

    //     // buildSystems(tree, apos, vpn, { 0, 0, 0 });
    // }

    for (auto sun : nearStars)
    {
        // glLogger->info("Sun: {}\n", sun->getsName());

        if (!sun->hasSystem())
            continue;
        pSystem *psys = sun->getSystem();

        glm::dvec3 apos = obs - sun->getbPosition();
        glm::dvec3 vpn = obs * glm::dvec3(0, 0, -1);

        buildSystems(sun->getSecondaries(), apos, vpn);
    }

    // Set light sources for planetshines
    setupSecondaryLightSources(lightSources, secondaryLights);

    renderSystemObjects();
}