// radio.h - Radio Navigation signal transmitter
//
// Author:  Tim Stark
// Date:    Oct 8, 2024

#pragma once

#define NAV_FREQ_MIN    108.0
#define NAV_FREQ_MAX    137.0
#define NAV_FREQ_NSTEP  640

class CelestialPlanet;
class Base;

enum rnavType
{
    rnavNone = 0,
    rnavVOR,        // VHF Omnidirectional Range 
    rnavVTOL,       // Vertical Takeoff/Landing
    rnavILS,        // Instrument Landing System
    rnavIDS         // Instrument Docking System
};

class RadioNavigation
{
public:
    RadioNavigation(float freq = NAV_FREQ_MIN, float range = 500);

    inline rnavType getType() const     { return type; }
    inline float getFreq() const        { return freq; }
    inline float getRange() const       { return range; }
    inline int getStep() const          { return step; }
    inline str_t getsName() const       { return id; }
    inline cchar_t *getcName() const    { return id.c_str(); }

    void setFreq(float nfreq);
    void setStep(int step);

    double length(const glm::dvec3 &gpos);
    double stretch(const glm::dvec3 &gpos);
    bool isInRange(const glm::dvec3 &gpos);

    virtual glm::dvec3 getPosition() = 0;
    virtual str_t getID(cstr_t &name) = 0;

protected:
    rnavType type = rnavNone;
    float range = 500;
    float freq;
    int step;
    cstr_t id;
};

// Very High Frequency Omnidirectional Range Station (VOR)
class RadioVOR : public RadioNavigation
{
public:
    RadioVOR(CelestialPlanet *planet);
    RadioVOR(CelestialPlanet *planet, double lat, double lng);
    RadioVOR(CelestialPlanet *planet, double lat, double lng, float freq, float range = 500);
    RadioVOR(CelestialPlanet *planet, cstr_t &name);

    inline CelestialPlanet *getPlanet() const   { return planet; }
    inline glm::dvec3 getPosition() const       { return lpos; }
    inline glm::dvec3 getLocation() const       { return { lat, lng, 0 }; }

protected:
    CelestialPlanet *planet;
    double lat, lng;
    glm::dvec3 lpos;
};

// Instrument Landing System (ILS)
class RadioILS : public RadioNavigation
{
public:
    RadioILS(Base *base, double lat, double lng, double dir, float freq, float range = 500);

    inline double getDirection() const          { return dir; }
    // inline glm::dvec3 getPosition() const       { return lpos; }
    inline glm::dvec3 getLocation() const       { return { lat, lng, 0 }; }

protected:
    Base *base;
    double lat, lng;
    double dir;
};

class NavigationManager
{
public:
    NavigationManager();
    ~NavigationManager();

    bool setup(json &config);

    inline int getNavSize() const                   { return navList.size(); }
    inline RadioNavigation *getNav(int idx) const   { return idx < navList.size() ? navList[idx] : nullptr; }

    void addNav(RadioNavigation *nav);

private:
    std::vector<RadioNavigation *> navList;
};