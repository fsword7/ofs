// system.cpp - Scene: Planetary system rendering routines
//
// Author:  Tim Stark
// Date:    Apt 21, 2022

#include "main/core.h"
#include "universe/body.h"
#include "universe/frame.h"
#include "render/scene.h"
#include "render/vobject.h"

void Scene::renderObjectAsPoint(ObjectListEntry &ole)
{

}

void Scene::renderCelestialBody(ObjectListEntry &ole)
{
    celBody *body = dynamic_cast<celBody *>(ole.object);

    vObject *vobj = getVisualObject(*ole.object, true);
    if (ole.objSize > 1.0)
    {
        ObjectProperties op;
        LightState lights;
    
        quatd_t orot = body->getuOrientation(prm.jnow);
        setObjectLighting(lightSources, ole.opos, orot, lights);

        op.color = body->getColor();
        op.orad  = body->getRadius();
        op.opos  = ole.opos;
        op.oqrot = orot;

        vobj->render(prm, op, lights);
    }
    else
        renderObjectAsPoint(ole);
}

void Scene::buildNearSystems(FrameTree *tree, Player &player, vec3d_t apos, vec3d_t vpnorm, vec3d_t origin)
{
    int nObjects = tree->getSystemSize();

    for (int idx = 0; idx < nObjects; idx++)
    {
        celBody *body = dynamic_cast<celBody *>(tree->getObject(idx));
        if (body == nullptr)
            continue;
    
        {
            Frame *frame = body->getOrbitFrame();
            vec3d_t opos = body->getoPosition(prm.jnow);
            vec3d_t spos = origin + glm::conjugate(frame->getOrientation(prm.jnow)) * opos;
            vec3d_t vpos = spos - apos;

            double vdist = glm::length(vpos);
            double vbnorm = glm::dot(vpnorm, vpos);
            double objSize = body->getRadius() / (vdist * pixelSize);

            double appMag = std::numeric_limits<double>::infinity();
            for (int idx = 0; idx < lightSources.size(); idx++)
            {
                vec3d_t sun = lightSources[idx].spos;
                double lum  = lightSources[idx].luminosity;
                double mag  = body->getApparentMagnitude(sun, lum, vpos);
                appMag = std::min(appMag, mag);
            }

            // fmt::printf("%s: Object size: %lf, distance: %lf\n",
            //     body->getsName(), objSize, vdist);
            // fmt::printf("Position:   (%lf,%lf,%lf)\n", opos.x, opos.y, opos.z);
            // fmt::printf("Observer:   (%lf,%lf,%lf)\n", apos.x, apos.y, apos.z);
            // fmt::printf("View:       (%lf,%lf,%lf)\n", vpos.x, vpos.y, vpos.z);
            // fmt::printf("Camera:     (%lf,%lf,%lf)\n", cpos.x, cpos.y, cpos.z);
            // fmt::printf("Sun:        (%lf,%lf,%lf)\n", spos.x, spos.y, spos.z);

            if (objSize > 1)
            {
                ObjectListEntry ole;

                ole.object  = body;
                ole.spos    = spos;
                ole.opos    = vpos;
                ole.vdist   = vdist;
                ole.objSize = objSize;
                ole.appMag  = appMag;

                renderCelestialBody(ole);
            }

            // Rendering satellites orbiting around this celestial body
            PlanetarySystem *system = body->getOwnSystem();
            FrameTree *subTree = system->getSystemTree();
            if (subTree != nullptr)
                buildNearSystems(subTree, player, apos, vpnorm, spos);
        }
    }
}