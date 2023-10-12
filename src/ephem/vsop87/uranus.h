// earth.cpp - Earth VSOP87 ephemeris package
//
// Author:  Tim Stark
// Date:    Nov 5, 2022

class OrbitVSOP87Uranus : public OrbitVSOP87
{
public:
    OrbitVSOP87Uranus(CelestialBody &cbody, vsop87p_t &series);
    virtual ~OrbitVSOP87Uranus() = default;

    uint16_t getOrbitData(double mjd, uint16_t req, double *res) override;

private:

};