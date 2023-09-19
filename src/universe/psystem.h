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

    void update(const TimeDate &td);

private:
    std::vector<CelestialStar *> stars;
    std::vector<CelestialBody *> bodies;
};