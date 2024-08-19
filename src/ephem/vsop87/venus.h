// earth.cpp - Earth VSOP87 ephemeris package
//
// Author:  Tim Stark
// Date:    Nov 5, 2022

class OrbitVSOP87Venus : public OrbitVSOP87
{
public:
    OrbitVSOP87Venus(Celestial &cbody, vsop87p_t &series);
    virtual ~OrbitVSOP87Venus() = default;

    uint16_t getOrbitData(double mjd, uint16_t req, double *res) override;

private:

};