// earth.cpp - Earth VSOP87 ephemeris package
//
// Author:  Tim Stark
// Date:    Nov 5, 2022

#include "ephem/vsop87a.h"

class OrbitVSOP87Earth : public OrbitVSOP87
{
public:
    OrbitVSOP87Earth(ObjectHandle object);
    virtual ~OrbitVSOP87Earth() = default;

    uint16_t getOrbitData(double mjd, uint16_t req, double *res) override;

private:

};