// astro.cpp - OFS astronomy formula package
//
// Author:  Tim Stark
// Date:    Apr 18, 2022

#include "main/core.h"
#include "universe/astro.h"

namespace astro
{ 
    // vec3d_t convertEquatorialToEcliptic(double ra, double de, double pc)
    // {
    //     double theta, phi;
    //     double x, y, z;
    //     vec3d_t opos;

    //     theta = ofs::radians(ra) + pi;
    //     phi   = ofs::radians(de) - pi/2.0;

    //     opos  = vec3d_t(sin(phi)*cos(theta), cos(phi), sin(phi)*-sin(theta)) * pc;

    //     return J2000ObliquityRotation * opos;
    // }

    glm::dvec3 convertEquatorialToEcliptic(double ra, double de, double pc)
    {
        double theta, phi;
        double x, y, z;
        glm::dvec3 opos;

        theta = ofs::radians(ra) + pi;
        phi   = ofs::radians(de) - pi/2.0;

        opos  = glm::dvec3(cos(theta)*sin(phi), cos(phi), sin(theta)*sin(phi)) * pc;

        return J2000ObliquityRotation * opos;
    }

    str_t getMJDDateStr(double mjd)
    {
        static const char *wdaystr[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
        static const char *monstr[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

        double ijd, c, e, h;
        int a, f, wday, mday, mon, year, hour, min, sec;

        h = 24.0 * modf(mjd, &ijd);
        if (ijd < -100840)
            c = ijd + 2401525.0;
        else {
            int b = (int)((ijd + 532784.75) / 36524.25);
            c = ijd + 2401526.0 + (b - b/4);
        }
        a = (int)((c - 122.1) / 365.25);
        e = 365.0 * a + a/4;
        f = (int)((c-e)/30.6001);

        wday = ((int)mjd + 3) % 7;
        mday = (int)(c-e+0.5) - (int)(30.6001*f);
        mon  = f-1 - 12 * (f/14);
        year = a-4715 - ((7 + mon)/10);
        hour = (int)h;
        min  = (int)(h = 60.0 * (h - hour));
        sec  = (int)(h = 60.0 * (h - min));

        return fmt::format("{} {} {:02d} {:02d}:{:02d}:{:02d} {}",
            wdaystr[wday], monstr[mon-1], mday, hour, min, sec, year);
    }

    struct tm *getMJDDate(double mjd)
    {
        static struct tm date;
        double ijd, c, e, h;
        int a, f;

        h = 24.0 * modf(mjd, &ijd);
        if (ijd < -100840)
            c = ijd + 2401525.0;
        else {
            int b = (int)((ijd + 532784.75) / 36524.25);
            c = ijd + 2401526.0 + (b - b/4);
        }
        a = (int)((c - 122.1) / 365.25);
        e = 365.0 * a + a/4;
        f = (int)((c-e)/30.6001);

        date.tm_wday  = ((int)mjd + 3) % 7;
        date.tm_mday  = (int)(c-e+0.5) - (int)(30.6001*f);
        date.tm_mon   = f-1 - 12 * (f/14);
        date.tm_year  = a-4715 - ((7 + date.tm_mon)/10);
        date.tm_hour  = (int)h;
        date.tm_min   = (int)(h = 60.0 * (h - date.tm_hour));
        date.tm_sec   = (int)(h = 60.0 * (h - date.tm_min));
        date.tm_isdst = 0;

        return &date;
    }
}