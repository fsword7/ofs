// atmo.cpp - Earth atomspheric model NRLMSISE-00 package
//
// Author:  Tim Stark
// Date:    Jul 9, 2022

#include "main/core.h"
#include "ephem/sol/earth/atmo.h"

AtmosphereEarthNRLMSISE00::AtmosphereEarthNRLMSISE00()
: atm(swFlags)
{
    // // Set default input values
    // input = {
    //     0,      // year - currently ignored
    //     172,    // day of year
    //     29000,  // second of day
    //     0,      // altitude [km]
    //     0,      // geodetic latitude [deg]
    //     0,      // geodetic longtitude [deg]
    //     0,      // local apparent solar time [hours]
    //     140,    // 81 day average of F10.7 flux
    //     140,    // daily F10.7 flux for previous day
    //     3.0,    // magnetic index (daily)
    //     nullptr // pointer to deltailed magnetic values
    // };

}

str_t AtmosphereEarthNRLMSISE00::getsAtmName() const
{
    return "Earth NRLMSISE-00 Edition Model";
}

void AtmosphereEarthNRLMSISE00::getAtmConstants(atmconst_t &atmc) const
{
    // Assign atomspheric constants;
    atmc.altLimit = 2500;
    atmc.p0 = 101325;
    atmc.rho0 = 1.2250;
    atmc.R = 286.91;
    atmc.gamma = 1.4;
}

void AtmosphereEarthNRLMSISE00::getAtmParams(const iatmprm_t &in, atmprm_t &out)
{
    std::array<double, 7> ap = { 3.0 };
    std::array<double, 9> d = { 0 };
    std::array<double, 2> t = { 0 };

    double ijd, mjd = 0;
    double h = 24.0 * modf(mjd, &ijd);
    double doy;
    double sec = h * 3600.0;

    double lat = ofs::degrees(in.lat);
    double lng = ofs::degrees(in.lng);
    double lst = h * lng / 15.0;
    double f107a = 140;
    double f107 = 140;

    {
        double c, e, mjd2;
        int a, b, f, m, y;
        if (ijd < -100840)
            c = ijd + 2401525.0;
        else
        {
            b = (int)((ijd + 532784.75) / 36524.25);
            c = ijd + 2401526.0 + (b - b/4);
        }
        a = (int)((c - 122.1) / 365.25);
        e = 365.0 * a + a/4;
        f = (int)((c - e)/30.6001);
        m = f-1 - 12 * (f/14);
        y = a - 4715 - ((7 + m)/10) - 1;
        double a2 = (double)(10000 * y + 1231);
        if (a2 <= 15821004.1)
            b = (y + 4716)/4 - 1181;
        else
            b = y/400 - y/100 + y/4;
        mjd2 = 365.0 * y + b - 78576.0;
        doy = (int)(mjd - mjd2);
        // pmjd = mjd;
    }


    atm.gtd7(doy, sec, in.alt, lat, lng, lst, f107a, f107, ap, d, t);

    double n = d[0] + d[1] + d[2] + d[3] + d[4] + d[6] + d[7];
    double k = 1.380649e-23 * 1e6; // Boltzmann constant and scale conversion of cm^3 to m^-3

    out.T = t[1];
    out.rho = d[5];
    out.p = n*k*out.T;

}
