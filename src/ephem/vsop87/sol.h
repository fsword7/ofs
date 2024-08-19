// sol.h - Sun VSOP87 ephemeris package (barycentric)
//
// Author:  Tim Stark
// Date:    Sep 18, 2023

class OrbitVSOP87Sol : public OrbitVSOP87
{
public:
    OrbitVSOP87Sol(Celestial &cbody, vsop87p_t &series);
    virtual ~OrbitVSOP87Sol() = default;

    uint16_t getOrbitData(double mjd, uint16_t req, double *res) override;

private:

};