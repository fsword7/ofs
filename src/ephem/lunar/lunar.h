// lunar.h - ELP82 epheremeis data package
//
// Author:  Tim Stark
// Date:    Oct 20, 2023

#pragma once

class OrbitELP82Lunar : public OrbitELP82
{
public:
    OrbitELP82Lunar(Celestial &cbody);
    virtual ~OrbitELP82Lunar() = default;

    uint16_t getOrbitData(double mjd, uint16_t req, double *res) override;

private:

};