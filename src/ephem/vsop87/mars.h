// earth.cpp - Earth VSOP87 ephemeris package
//
// Author:  Tim Stark
// Date:    Nov 5, 2022

class OrbitVSOP87Mars : public OrbitVSOP87
{
public:
    OrbitVSOP87Mars(Celestial &cbody, vsop87p_t &series);
    virtual ~OrbitVSOP87Mars() = default;

    uint16_t getOrbitData(double mjd, uint16_t req, double *res) override;

private:

};