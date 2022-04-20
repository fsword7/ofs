// date.h - date/time package
//
// Author:  Tim Stark
// Date:    Apr 20, 2022

#pragma once

#define DAYS_PER_YEAR       365.25

#define HOURS_PER_DAY       24.0
#define MINUTES_PER_DAY     1440.0
#define SECONDS_PER_DAY     86400.0

class Date
{
public:
    Date() = default;
    Date(double);
    Date(int y, int m, int d)
    : year(y), month(m), day(d)
    { }  
    ~Date() = default;

    double getJulian() const;
    str_t  getString() const;

    inline operator double() { return getJulian(); }

    void reset();
    double update();
    void now();
    
private:
    double  lastTime = 0.0;
    double  sysTime = 0.0;
    double  jdTime = 0.0;

    int     year = 0;
    int     month = 0;
    int     day = 0;

    int     hour = 0;
    int     minute = 0;
    double  second = 0.0;

    int     wday = 0;

    bool    dst = false;
    int     utcOffset = 0;
    str_t   tzName = "UTC";
};