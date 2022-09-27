// camera.cpp - Visual Camera package
//
// Author:  Tim Stark
// Date:    Sep 16, 2022

#include "main/core.h"
#include "client.h"
#include "camera.h"


Camera::Camera(int width, int height)
{
    setSize(width, height);
    fov = glm::radians(SCR_FOV);
    zNear = 0.0001;
    zFar = 10000000;
}

void Camera::setSize(int w, int h)
{
    width = w;
    height = h;
    aspect = double(width) / double(height);

    glViewport(0, 0, width, height);
}

void Camera::update()
{
    // Get camera position/rotation from server
    // gpos = ofsGetCameraGlobalPosition();
    // gdir = ofsGetCameraGlobalDirection();
    // grot = ofsGetCameraRotationMatrix();
    gpos = { 0, 0, 60000 };
    grot = glm::dmat3(1);

    // Update view/projection matrix
    dmProj = glm::perspective(fov, aspect, zNear, zFar);
    dmView = glm::dmat4(1);
    dmView = grot;
    dmViewProj = dmProj * dmView;
}