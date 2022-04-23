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

        quatd_t orot = body->getuOrientation(prm.jnow);

        op.color = body->getColor();
        op.orad  = body->getRadius();
        op.opos  = ole.opos;
        op.oqrot = orot;

        vobj->render(prm, op);
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
            vec3d_t opos = body->getoPosition(prm.jnow);
            vec3d_t spos = origin + opos;
            vec3d_t vpos = spos - apos;

            double vdist = glm::length(vpos);
            double vbnorm = glm::dot(vpnorm, vpos);
            double objSize = body->getRadius() / (vdist * pixelSize);

            double appMag = std::numeric_limits<double>::infinity();

            if (objSize > 1)
            {
                ObjectListEntry ole;

                ole.object  = body;
                ole.spos    = spos;
                ole.opos    = opos;
                ole.vdist   = vdist;
                ole.objSize = objSize;
                ole.appMag  = appMag;

                renderCelestialBody(ole);
            }

            // Rendering satellites orbiting around this celestial body
            // PlanetarySystem *system = body->getOwnSystem();
            // FrameTree *subTree = system->getSystemTree();
            // if (subTree != nullptr)
            //     buildNearSystems(subTree, player, apos, vpnorm, spos);
        }
    }
}