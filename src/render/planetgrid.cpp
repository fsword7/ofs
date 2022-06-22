// planetgrid.cpp - Planetocentric grid display package
//
// Author:  Tim Stark
// Date:    Jun 14, 2022

#include "main/core.h"
#include "osd/gl/context.h"
#include "osd/gl/shader.h"
#include "universe/body.h"
#include "render/scene.h"
#include "render/planetgrid.h"


PlanetographicGrid::PlanetographicGrid(const celBody &body)
: body(body)
{
    initGeometry();
}

void PlanetographicGrid::initGeometry()
{
    xzCircles.reserve((circleSubdivisions + 2) * 2);
    xyCircles.reserve((circleSubdivisions + 2) * 2);

    for (uint32_t idx = 0; idx <= circleSubdivisions+1; idx++)
    {
        float theta = float(std::numbers::pi * 2.0) * float(idx) / float(circleSubdivisions);
        float s = sin(theta), c = cos(theta);

        vec3f_t xzPoint(c, 0.0f, s);
        vec3f_t xyPoint(c, s, 0.0f);

        // Latitude circles
        xzCircles.emplace_back(xzPoint, -0.5f);
        xzCircles.emplace_back(xzPoint, 0.5f);

        // Longtitude circles
        xyCircles.emplace_back(xyPoint, -0.5f);
        xyCircles.emplace_back(xyPoint, 0.5f);
    }
}

void PlanetographicGrid::labelLatLong(cstr_t &label, double longtitude, double latituide)
{

}

void PlanetographicGrid::render(renderParam &prm)
{
    vec3d_t pos;

    ShaderProgram *pgmGrid = nullptr;

    // Determine scale a height above planet surface
    // due to Z-fighting effect. 
    float scale  = std::max((prm.pxSize + 1) / prm.pxSize, 1.001);
    float offset = scale - 1.0f;

    float latStep = minLatitudeStep;
    float lngStep = minLongitudeStep;

    // quatd_t qrot = quatd_t(Eigen::AngleAxis<double>(std::numbers::pi, vec3d_t::UnitY())) * body.getBodyFixedFromEcliptic(prm.jnow);
    quatd_t qrot  = body.getBodyFixedFromEcliptic(prm.jnow);
    quatf_t qrotf = qrot.cast<float>();

    vec3d_t semiAxes = body.getSemiAxes();

    vec3d_t viewNormal = prm.crot.conjugate() * vec3d_t::UnitZ();
    vec3f_t vn = viewNormal.cast<float>();

    Eigen::Affine3d transform = Eigen::Translation3d(pos) * qrot.conjugate() * Eigen::Scaling(scale * semiAxes);
    mat4f_t mView = (prm.dmView * transform.matrix()).cast<float>();

    pgmGrid->use();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineStripVertrex) * 2, &xzCircles[0].point);

    // Display XZ (latitude) circles
    for (float latitude = -90.0f; latitude < 90.0f; latitude += latStep)
    {
        float phi = ofs::radians(latitude);
        float rad = cos(phi);

        // mvp = mProj * mView * ofs::translate(0.0f, sin(phi), 0.0f) * ofs::scale(rad);

        // Draw a circle now
        glDrawArrays(GL_LINE_STRIP, 0, (xzCircles.size() - 2) / 2);

        // Annotation labels
        // {
        //     char ns;
        //     if (latitude < 0.0f)
        //         ns = latType == csNorthNormal ? 'S' : 'N';
        //     else
        //         ns = latType == csNorthReversed ? 'N' : 'S';
        //     str_t label = fmt::format("{}{}", int(fabs(latitude)), ns);
        // }

        // labelLatLong(label, 0.0, latitude, semiAxes, q);
        // labelLatLong(label, 180.0, latitude, semiAxes, q);
    }

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineStripVertrex) * 2, &xyCircles[0].point);

    // Display XY (longitude) circles
    for (float longtitude = 0.0f; longtitude < 180.0f; longtitude = lngStep)
    {
        float theta = ofs::radians(longtitude);
        // mat4f_t mvp = (prm.dmProj * prm.dmView).cast<float>() * rotate(Eigen::AngleAxisf(theta), vec3d_t::UnitY());

        // Annotation labels
        // {
        //     char ew;
        //     if (latitude < 0.0f)
        //         ew = latType == csNorthNormal ? 'S' : 'N';
        //     else
        //         ew = latType == csNorthReversed ? 'N' : 'S';
        //     str_t label = fmt::format("{}{}", int(fabs(latitude)), ew);
        // }

        // labelLatLong(label, 0.0, latitude, semiAxes, q);
        // labelLatLong(label, 180.0, latitude, semiAxes, q);
    }

    glDisableVertexAttribArray(0);
    
    pgmGrid->release();
}