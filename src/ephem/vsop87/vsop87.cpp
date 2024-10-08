// vsop87.cpp - VSOP87 Ephemeris data package
//
// Author:  Tim Stark
// Date:    Nov 3, 2022

#include "main/core.h"
#include "api/ofsapi.h"
#include "api/celbody.h"
#include "ephem/ephemeris.h"
#include "ephem/vsop87/vsop87.h"
#include "universe/astro.h"

#include "ephem/vsop87/sol.h"
#include "ephem/vsop87/mercury.h"
#include "ephem/vsop87/venus.h"
#include "ephem/vsop87/earth.h"
#include "ephem/vsop87/mars.h"
#include "ephem/vsop87/jupiter.h"
#include "ephem/vsop87/saturn.h"
#include "ephem/vsop87/uranus.h"
#include "ephem/vsop87/neptune.h"

#define VSOP_SERIES(series) vsop87s_t(series, ARRAY_SIZE(series))
#define VSOP_PARAM(series)  (series), ARRAY_SIZE(series)

#include "ephem/vsop87/vsop87sun.cpp" // Sun/Sol
#include "ephem/vsop87/vsop87mer.cpp" // Mercury
#include "ephem/vsop87/vsop87ven.cpp" // Venus
#include "ephem/vsop87/vsop87ear.cpp" // Earth
#include "ephem/vsop87/vsop87mar.cpp" // Mars
#include "ephem/vsop87/vsop87jup.cpp" // Jupiter
#include "ephem/vsop87/vsop87sat.cpp" // Saturn
#include "ephem/vsop87/vsop87ura.cpp" // Uranus
#include "ephem/vsop87/vsop87nep.cpp" // Neptune

OrbitVSOP87::OrbitVSOP87(Celestial &cbody, vsop87p_t &series)
: OrbitEphemeris(cbody), series(series)
{
	setSeries(series.type);
}

OrbitVSOP87::~OrbitVSOP87()
{

}

void OrbitVSOP87::setSeries(char series)
{
    sid = std::toupper(series);

    fmtFlags = 0;

	// Check type for A and C rectangular series.
	if (sid == 'A' || sid == 'C')
		fmtFlags |= EPHEM_RECT;
    // Check type for B and D heliocentric series
    else if (sid == 'B' || sid == 'D')
        fmtFlags |= EPHEM_POLAR;
    // Check type for E barycentric series
    else if (sid == 'E')
        fmtFlags |= EPHEM_PARENT;
	else
	{

	}
}

void OrbitVSOP87::getEphemeris(double mjd, double *res)
{
	static const double mjd2000 = 51544.5;
	static const double jd2000 = 2451545.0;
	static const double a1000 = 365250.0;
	static const double rsec = 1.0 / (a1000 * 86400.0);

	static const double c0 = 299792458;				// speed of light [m/s]
	static const double tauA = 499.004783806;		// light time for 1 AU [s]
	static const double AU = (c0 * tauA) / 1000.0;	// 1 AU in kilometers
	static const double pscl = AU;					// convert AU to km
	static const double vscl = AU * rsec;			// convert AU/millenium to km/s

	// Clear all ephemeris parameters
	for (int idx = 0; idx < VSOP_PARAMS; idx++)
		res[idx] = 0.0;

	double a, b, c;
	double arg, tm, tmdot;

	// Set up time series
	double t[VSOP_MAXALPHA+1];
	t[0] = 1.0;
	t[1] = (mjd - mjd2000) / a1000;
	for (int idx = 2; idx <= VSOP_MAXALPHA; idx++)
		t[idx] = t[idx-1] * t[1];

	// Logger::getLogger()->debug("MJD {} - {}\n", mjd, astro::getMJDDateStr(mjd));

	// compute term series
	for (int idx = 0; idx < 3; idx++)
	{
		int nalpha = series.alpha[idx];
		vsop87s_t *group = series.groups[idx];

		for (int alpha = 0; alpha < nalpha; alpha++)
		{
			vsop87_t *terms = group[alpha].terms;
			int nTerms = group[alpha].nTerms;

			// Logger::getLogger()->debug("{}: {} terms\n", alpha, nTerms);

			tm = tmdot = 0.0;

			for (int term = 0; term < nTerms; term++)
			{
				// f(tm) = a * cos (b + c * T)
				// f'(tm) = a * -sin (b + c * T) * c

				a = terms[term].a;
				b = terms[term].b;
				c = terms[term].c;

				// if (((term >= 0) && (term < 3)) || (term >= nTerms-3 && term < nTerms))
				// 	Logger::getLogger()->debug("{}: {:.12f} {:.12f} {:.12f}\n", term, a, b, c);

				arg = b + c * t[1];
				tm += a * cos(arg);
				tmdot -= c * a * sin(arg);
			}

			res[idx]  += t[alpha] * tm;
			res[idx+3] += t[alpha] * tmdot +
				(alpha > 0 ? alpha * t[alpha - 1] * tm : 0.0);
		}		
	}

	// Logger::getLogger()->debug("VSOP87: Name: {} Type: {} MJD: {} JD: {}\n",
	// 	series.name, series.type, mjd, (t[1] * a1000) + jd2000);
	// Logger::getLogger()->debug("VSOP87: P({:14.10f}, {:14.10f}, {:14.10f})\n",
	// 	std::fmod(res[0], pi*2), res[1], res[2]);
	// Logger::getLogger()->debug("VSOP87: V({:14.10f}, {:14.10f}, {:14.10f})\n",
	// 	res[3], res[4], res[5]);

	if (fmtFlags & EPHEM_POLAR)
	{
		for (int idx = 3; idx < 6; idx++)
			res[idx] *= rsec;
	}
	else
	{
		for (int idx = 0; idx < 3; idx++)
			res[idx] *= pscl;
		for (int idx = 3; idx < 6; idx++)
			res[idx] *= vscl;

		// swap Y and Z for mapping left-handed cordinates
		double tmp;
		tmp = res[1]; res[1] = res[2]; res[2] = tmp;
		tmp = res[4]; res[4] = res[5]; res[5] = tmp;
	}
}

OrbitEphemeris *OrbitVSOP87::create(Celestial &cbody, cstr_t &name)
{
	if (name == "vsop87e-sol")
		return new OrbitVSOP87Sol(cbody, sun_XYZ);
    if (name == "vsop87b-mercury")
        return new OrbitVSOP87Mercury(cbody, mercury_LBR);
    if (name == "vsop87b-venus")
        return new OrbitVSOP87Venus(cbody, venus_LBR);
    if (name == "vsop87b-earth")
        return new OrbitVSOP87Earth(cbody, earth_LBR);
    if (name == "vsop87b-mars")
        return new OrbitVSOP87Mars(cbody, mars_LBR);
    if (name == "vsop87b-jupiter")
        return new OrbitVSOP87Jupiter(cbody, jupiter_LBR);
    if (name == "vsop87b-saturn")
        return new OrbitVSOP87Saturn(cbody, saturn_LBR);
    if (name == "vsop87b-uranus")
        return new OrbitVSOP87Uranus(cbody, uranus_LBR);
    if (name == "vsop87b-neptune")
        return new OrbitVSOP87Neptune(cbody, neptune_LBR);

    return nullptr;
}
