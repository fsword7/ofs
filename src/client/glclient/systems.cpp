// systems.cpp - Scene: Planetary system rendering routines
//
// Author: Tim Stark
// Date: Oct 28, 2022

#include "main/core.h"
#include "universe/universe.h"
#include "universe/body.h"

#include "client.h"
#include "scene.h"

void Scene::renderObjectAsPoint(ObjectListEntry &ole)
{

}

void Scene::renderCelestialBody(ObjectListEntry &ole)
{
    celBody *body = dynamic_cast<celBody *>(ole.object);

    vObject *vobj = getVisualObject(ole.object, true);
    if (ole.objSize > 1.0)
    {
        // ObjectProperties op;
        // LightState lights;
    
        glm::dmat3 orot = body->getuOrientation(now);
        // setObjectLighting(lightSources, ole.opos, orot, lights);

        // Apply the modelview for celestial body
        // Eigen::Affine3d transform = Eigen::Translation3d(ole.opos) * orot.conjugate();
        // mat4d_t mvPlanet = prm.dmView * transform.matrix();
    
        // op.body  = body;
        // op.color = body->getColor();
        // op.orad  = body->getRadius();
        // op.lpos  = body->getvPlanetocentricFromEcliptic(ole.opos, prm.jnow);
        // op.wpos  = body->getPlanetocentric(op.lpos);
        // op.opos  = ole.opos;
        // op.oqrot = orot;
        // op.mvp   = mvPlanet;
        
        // fmt::print("Planetocentric: {:.6f} {:.6f} {:.6f}\n", op.lpos.x, op.lpos.y, op.lpos.z);

        // vobj->render(prm, op, lights);
    }
    else
        renderObjectAsPoint(ole);
}

void Scene::buildSystems(FrameTree *tree, const glm::dvec3 &apos,
    const glm::dvec3 &vpnorm, const glm::dvec3 &origin)
{
    int nObjects = tree->getSystemSize();

    for (int idx = 0; idx < nObjects; idx++)
    {
        celBody *body = dynamic_cast<celBody *>(tree->getObject(idx));
        if (body == nullptr)
            continue;
    
        // logger->debug("Rendering {}...\n", ofsGetObjectName(body));

        {
            Frame *frame = body->getOrbitFrame();
            glm::dvec3 opos = body->getoPosition(now);
            glm::dvec3 spos = origin + frame->getOrientation(now) /* .conjugate() */ * opos;
            glm::dvec3 vpos = spos - apos;

            double vdist = glm::length(vpos);
            double vbnorm = glm::dot(vpnorm, vpos);
            double objSize = body->getRadius() / (vdist * pixelSize);

            // double appMag = std::numeric_limits<double>::infinity();
            // for (int idx = 0; idx < lightSources.size(); idx++)
            // {
            //     vec3d_t sun = lightSources[idx].spos;
            //     double lum  = lightSources[idx].luminosity;
            //     double mag  = body->getApparentMagnitude(sun, lum, vpos);
            //     appMag = std::min(appMag, mag);
            // }

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
                ole.appMag  = 0.0; //appMag;

                renderCelestialBody(ole);
            }

            // Rendering satellites orbiting around this celestial body
            PlanetarySystem *system = body->getOwnSystem();
            FrameTree *subTree = system->getSystemTree();
            // if (subTree != nullptr)
            //     buildNearSystems(subTree, player, apos, vpnorm, spos);
        }
    }
}