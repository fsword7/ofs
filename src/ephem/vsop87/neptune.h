// earth.cpp - Earth VSOP87 ephemeris package
//
// Author:  Tim Stark
// Date:    Nov 5, 2022

class OrbitVSOP87Neptune : public OrbitVSOP87
{
public:
    OrbitVSOP87Neptune(CelestialBody &cbody, vsop87p_t &series);
    virtual ~OrbitVSOP87Neptune() = default;

    uint16_t getOrbitData(double mjd, uint16_t req, double *res) override;

private:

};