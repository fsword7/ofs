// annotation.cpp - Scene: Annotation package
//
// Author:  Tim Stark
// Date:    Jun 13, 2022

#include "main/core.h"
#include "osd/gl/context.h"
#include "render/scene.h"

void Scene::addAnnotation(std::vector<Annotation> &annotations,
    cstr_t &labelText, color_t color, const vec3d_t &pos, float size)
{
    int view[4] = { 0, 0, ctx.getWidth(), ctx.getHeight() };
    vec3d_t wpos;

    ofs::setProjectPerspective(prm.dmProj, prm.dmView, view, pos, wpos);

    // double depth = pos.x() * prm.dmView(2, 0) +
    //                pos.y() * prm.dmView(2, 1) +
    //                pos.z() * prm.dmView(2, 2);
    // wpos.z() = -depth;

    Annotation label;

    label.labelText = labelText;
    label.color = color;
    label.tpos = wpos;
    label.size = size;

    annotations.push_back(label);
}


void Scene::renderAnnotations(const std::vector<Annotation> &annotations)
{

}