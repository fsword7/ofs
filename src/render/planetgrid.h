// planetgrid.h - Planetocentric grid display package
//
// Author:  Tim Stark
// Date:    Jun 14, 2022

#pragma once

#include "render/refmark.h"

class PlanetographicGrid : public ReferenceMark
{
public:
    enum csType
    {
        csWestEast, // For Earth and Moon only
        csWestward, // For prograde rotators
        csEastward  // For retrograde rotators (Venus)
    };

    enum northType
    {
        csNorthNormal,      // North up
        csNorthReversed     // North down
    };

    PlanetographicGrid(const celBody &body);
    ~PlanetographicGrid() = default;

    void initGeometry();
    void labelLatLong(cstr_t &label, double longtitude, double latituide);
    void render(renderParam &prm);

private:
    const celBody &body;
    csType    lngType = csWestEast;
    northType latType = csNorthNormal;

    float minLongitudeStep = 10.0f;
    float minLatitudeStep = 10.0f;

    uint32_t circleSubdivisions = 100;
    
    std::vector<LineStripVertrex> xyCircles; // Longtitude circles [XY]
    std::vector<LineStripVertrex> xzCircles; // Latitude circles [XZ]
};