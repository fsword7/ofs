// vobject.cpp - Visual object package
//
// Author:  Tim Stark
// Date:    Sep 6, 2022

#include "main/core.h"
#include "api/ofsapi.h"
#include "engine/object.h"
#include "client.h"
#include "shader.h"
#include "scene.h"
#include "vobject.h"
#include "vbody.h"
#include "vstar.h"
#include "vvehicle.h"

vObject::vObject(const Object *obj, Scene &scene)
: object(obj), scene(scene)
{ 
    init();
}

vObject::~vObject()
{
    // if (pgmObjectAsPoint != nullptr)
    //     delete pgmObjectAsPoint;
}

void vObject::init()
{
    ShaderManager &shmgr = scene.getShaderManager();
    pgmObjectAsPoint = shmgr.createShader("point");

    pgmObjectAsPoint->use();

    mvp = mat4Uniform(pgmObjectAsPoint->getID(), "mvp");
    uCamClip = vec2Uniform(pgmObjectAsPoint->getID(), "uCamClip");

    vao = new VertexArray();
    vao->bind();

    vbo = new VertexBuffer(&vtx, 1 * sizeof(objVertex), GL_STATIC_DRAW);
    vbo->bind();

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(objVertex), (void *)0);
    glEnableVertexAttribArray(0);
    checkErrors();

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(objVertex), (void *)12);
    glEnableVertexAttribArray(1);
    checkErrors();

    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(objVertex), (void *)28);
    glEnableVertexAttribArray(2);
    checkErrors();

    vao->unbind();
    pgmObjectAsPoint->release();

    // Assign surface data to internal object class
    const CelestialBody *cbody = dynamic_cast<const CelestialBody *>(object);
    if (cbody != nullptr)
        cbody->setVisualObject(this);
}

void vObject::update(int now)
{

    // gpos  = object->getuPosition(now);
    gpos  = object->getgPosition();
    vpos  = gpos - scene.getObserver()->getPosition();
    vdist = glm::length(vpos);

    grot = glm::dmat3(1.0); // ofsGetObjectGlobalRotation(object);

    // dmWorld = glm::transpose(grot);
    // dmWorld = glm::translate(dmWorld, vpos);
    dmWorld = { grot[0][0], grot[1][0], grot[2][0], 0,
                grot[0][1], grot[1][1], grot[2][1], 0,
                grot[0][2], grot[1][2], grot[2][2], 0,
                vpos.x,     vpos.y,     vpos.z,     1 };
}

void vObject::renderObjectAsPoint(const ObjectListEntry &ole)
{
    double maxStarSize = 1.0;
    double maxBlendStarSize = maxStarSize + 3.0;

    if (ole.pxSize < maxBlendStarSize)
    {
        double alpha, pointSize;
        double fade = 1.0;

        if (ole.pxSize > maxStarSize)
        {
            fade = std::min(1.0, (maxBlendStarSize - ole.pxSize) /
                                 (maxBlendStarSize - maxStarSize));
            
        }

        scene.calculatePointSize(ole.appMag, 5.0, pointSize, alpha);

        alpha *= fade;


        pgmObjectAsPoint->use();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glEnable(GL_PROGRAM_POINT_SIZE);

        vtx.posObject = ole.vpos;
        vtx.color = { ole.color, float(alpha) };
        vtx.size = pointSize;

        vao->bind();
        vbo->update(&vtx, sizeof(objVertex));

        glm::dmat4 proj = scene.getCamera()->getProjMatrix();
        glm::dmat4 view = scene.getCamera()->getViewMatrix();
        mvp = glm::mat4(proj * view);
        uCamClip = scene.getCamera()->getClip();

        glDrawArrays(GL_POINTS, 0, 1 /* vbo.getCount() */);
        scene.checkErrors();

        vao->unbind();

        glDisable(GL_PROGRAM_POINT_SIZE);
        glDisable(GL_BLEND);

        pgmObjectAsPoint->release();
    }
}

vObject *vObject::create(const Object *object, Scene &scene)
{
    switch (object->getType())
    {
    case ObjectType::objCelestialStar:
        return new vStar(object, scene);
    case ObjectType::objCelestialBody:
        return new vBody(object, scene);
    case ObjectType::objVehicle:
        return new vVehicle(object, scene);
    }

    return nullptr;
}

// ******** Scene ********

vObject *Scene::addVisualObject(const Object *object)
{
    vObject *vobj = nullptr;

    vobj = vObject::create(object, *this);
    vobjList.push_back(vobj);

    return vobj;
}

vObject *Scene::getVisualObject(const Object *object, bool bCreate)
{
    for (int idx = 0; idx < vobjList.size(); idx++)
        if (vobjList[idx]->getObject() == object)
            return vobjList[idx];

    if (bCreate == true)
        return addVisualObject(object);
    return nullptr;
}
