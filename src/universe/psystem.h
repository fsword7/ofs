// psystem.h -      Solar (Planetary) System package
//
// Author:  Tim Stark
// Date:    Sep 17, 2023

#pragma once

class CelestialStar;
class CelestialBody;
class TimeDate;
class pSystem
{
public:
    pSystem() = default;
    pSystem(CelestialStar *star);
    ~pSystem() = default;

    void addStar(CelestialStar *cbody);
    void addBody(CelestialBody *cbody);
    void addPlanet(CelestialBody *planet, CelestialBody *cbody);
    void addGravity(CelestialBody *grav);

    void update(const TimeDate &td);
    void finalizeUpdate();

private:
    CelestialStar *primaryStar = nullptr;

    std::vector<CelestialStar *> stars;
    std::vector<CelestialBody *> bodies;
    std::vector<CelestialBody *> planets;
    std::vector<CelestialBody *> gravities;
};