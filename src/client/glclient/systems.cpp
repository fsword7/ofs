// systems.cpp - Scene: Planetary system rendering routines
//
// Author: Tim Stark
// Date: Oct 28, 2022

#include "main/core.h"
#include "universe/universe.h"
#include "universe/celbody.h"

#include "client.h"
#include "scene.h"
#include "vobject.h"

void Scene::renderObjectAsPoint(ObjectListEntry &ole)
{

}

void Scene::renderCelestialBody(ObjectListEntry &ole)
{
    CelestialBody *body = dynamic_cast<CelestialBody *>(ole.object);

    vObject *vobj = getVisualObject(ole.object, true);
    if (ole.objSize > 1.0) 
    {
        // ObjectProperties op;
        // LightState lights;
    
        // glm::dmat3 orot = body->getuOrientation(now);
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

void Scene::renderSystemObjects()
{
    if (renderList.empty())
        return;

    glm::dmat4 view = camera->getViewMatrix();
    glm::dmat4 proj = camera->getProjMatrix();

    for (auto ole : renderList)
    {
        ObjectProperties op;

        op.mjd   = ole.mjd;
        op.dTime = 0.0; // now;
        op.color = ole.color;
        op.orad  = ole.objSize;
        op.opos  = ole.opos;
        op.orot  = glm::dmat3(1);

        op.mvp = glm::dmat4(view * proj);
        op.clip = camera->getClip();

        logger->debug("Rendering {}...\n", ole.object->getName());

        ole.visual->render(op);
    }
}

void Scene::buildSystems(secondaries_t &bodies, const glm::dvec3 &obs,
    const glm::dvec3 &vpnorm)
{
    // logger->debug("{}: {} bodies to being rendered.\n", "(unknown)", bodies.size());

    for (auto body : bodies)
    {
        if (body->getCelestialType() == cbObserver)
            continue;

        {
            glm::dvec3 opos = body->getoPosition();
            glm::dvec3 vpos = opos - obs;

            double vdist = glm::length(vpos);
            double vbnorm = glm::dot(vpnorm, vpos);
            double vSize  = body->getRadius() / vdist;

            // if (pxSize > 1)
            // {
                ObjectListEntry ole;

                ole.object  = body;
                ole.visual = getVisualObject(ole.object, true);
                ole.objSize = body->getRadius();

                // ole.spos    = spos;
                ole.opos    = vpos;
                ole.vdist   = vdist;
                ole.vSize   = vSize;
                ole.pxSize  = vSize * pixelSize;
                ole.appMag  = 0.0; //appMag;

                ole.zCenter = 0.0;
                ole.zFar    = 1e24;
                ole.zNear   = 0.0001;
                ole.camClip = glm::vec2(ole.zNear, ole.zFar);

                // logger->debug("{}: Adding to rendering list\n", body->getsName());
                logger->debug("{}: P({:.6f},{:.6f},{:.6f}))\n",
                    body->getsName(), ole.opos.x, ole.opos.y, ole.opos.z);

                addRenderList(ole);
                // renderCelestialBody(ole);
            // }

            secondaries_t &secondaries = body->getSecondaries();
            if (secondaries.size() > 0)
                buildSystems(secondaries, obs, vpnorm);
        }
    }
}

// void Scene::buildSystems(FrameTree *tree, const glm::dvec3 &apos,
//     const glm::dvec3 &vpnorm, const glm::dvec3 &origin)
// {
//     int nObjects = tree->getSystemSize();

//     for (int idx = 0; idx < nObjects; idx++)
//     {
//         CelestialBody *body = dynamic_cast<CelestialBody *>(tree->getObject(idx));
//         if (body == nullptr)
//             continue;
    
//         // logger->debug("Rendering {}...\n", ofsGetObjectName(body));

//         {
//             Frame *frame = body->getOrbitFrame();
//             glm::dvec3 opos = body->getoPosition(now);
//             glm::dvec3 spos = origin + frame->getOrientation(now) /* .conjugate() */ * opos;
//             glm::dvec3 vpos = spos - apos;

//             double vdist = glm::length(vpos);
//             double vbnorm = glm::dot(vpnorm, vpos);
//             double objSize = body->getRadius() / (vdist * pixelSize);

//             // double appMag = std::numeric_limits<double>::infinity();
//             // for (int idx = 0; idx < lightSources.size(); idx++)
//             // {
//             //     vec3d_t sun = lightSources[idx].spos;
//             //     double lum  = lightSources[idx].luminosity;
//             //     double mag  = body->getApparentMagnitude(sun, lum, vpos);
//             //     appMag = std::min(appMag, mag);
//             // }

//             // fmt::printf("%s: Object size: %lf, distance: %lf\n",
//             //     body->getsName(), objSize, vdist);
//             // fmt::printf("Position:   (%lf,%lf,%lf)\n", opos.x, opos.y, opos.z);
//             // fmt::printf("Observer:   (%lf,%lf,%lf)\n", apos.x, apos.y, apos.z);
//             // fmt::printf("View:       (%lf,%lf,%lf)\n", vpos.x, vpos.y, vpos.z);
//             // fmt::printf("Camera:     (%lf,%lf,%lf)\n", cpos.x, cpos.y, cpos.z);
//             // fmt::printf("Sun:        (%lf,%lf,%lf)\n", spos.x, spos.y, spos.z);

//             if (objSize > 1)
//             {
//                 ObjectListEntry ole;

//                 ole.object  = body;
//                 ole.spos    = spos;
//                 ole.opos    = vpos;
//                 ole.vdist   = vdist;
//                 ole.objSize = objSize;
//                 ole.appMag  = 0.0; //appMag;

//                 renderCelestialBody(ole);
//             }

//             // Rendering satellites orbiting around this celestial body
//             PlanetarySystem *system = body->getOwnSystem();
//             FrameTree *subTree = system->getSystemTree();
//             // if (subTree != nullptr)
//             //     buildNearSystems(subTree, player, apos, vpnorm, spos);
//         }
//     }
// }