// radio.h - Radio Navigation signal transmitter
//
// Author:  Tim Stark
// Date:    Oct 8, 2024

#pragma once

#define NAV_FREQ_MIN    108.0
#define NAV_FREQ_MAX    137.0
#define NAV_FREQ_NSTEP  640

class RadioNavigation
{
public:
    RadioNavigation(float freq = NAV_FREQ_MIN, float range = 500);

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
    float freq;
    float range;
    int step;
    cstr_t id;
};

class NavigationManager
{
public:
    NavigationManager();
    ~NavigationManager();

    bool setup(YAML::Node &config);

    inline int getNavSize() const                   { return navList.size(); }
    inline RadioNavigation *getNav(int idx) const   { return idx < navList.size() ? navList[idx] : nullptr; }

    void addNav(RadioNavigation *nav);

private:
    std::vector<RadioNavigation *> navList;
};