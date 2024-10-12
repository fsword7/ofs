// navigation.cpp - Navigation signal transmitter
//
// Author:  Tim Stark
// Date:    Oct 8, 2024

#include "main/core.h"
#include "engine/navigation.h"

RadioNavigation::RadioNavigation(float freq, float range)
: freq(freq), range(range)
{
    setFreq(freq);
}

void RadioNavigation::setFreq(float nfreq)
{
    freq = nfreq;
    step = (int)((freq - NAV_FREQ_MIN) * 20.0 + 0.5);
}

void RadioNavigation::setStep(int nstep)
{
    step = nstep;
    freq = NAV_FREQ_MIN + (step * 0.05);
}

double RadioNavigation::length(const glm::dvec3 &gpos)
{
    return glm::length(getPosition() - gpos);
}

double RadioNavigation::stretch(const glm::dvec3 &gpos)
{
    return (range*range) / std::max(glm::length2(getPosition()), 1.0);
}

bool RadioNavigation::isInRange(const glm::dvec3 &gpos)
{
    return glm::length2(gpos) < (range*range);
}