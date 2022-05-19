// strings.cpp - String utilities package
//
// Author:  Tim Stark
// Date:    May 18, 2022

#include "main/core.h"
#include "utils/string.h"

int compareIgnoreCase(std::string_view s1, std::string_view s2)
{
    auto i1 = s1.begin();
    auto i2 = s2.begin();

    while (i1 != s1.end() && i2 != s2.end())
    {
        if (toupper(*i1) != toupper(*i2))
            return (toupper(*i1) < toupper(*i2)) ? -1 : 1;
        i1++, i2++;
    }
    return 0;
    // return s2.size() - s1.size();
}

int compareIgnoreCase(std::string_view s1, std::string_view s2, int n)
{
    auto i1 = s1.begin();
    auto i2 = s2.begin();

    while (i1 != s1.end() && i2 != s2.end() && n > 0)
    {
        if (toupper(*i1) != toupper(*i2))
            return (toupper(*i1) < toupper(*i2)) ? -1 : 1;
        i1++, i2++, n--;
    }
    return 0;
    // return n > 0 ? s2.size() - s1.size() : 0;
}

bool CompareIgnoreCasePredicate::operator()(std::string_view s1, std::string_view s2) const
{
    return compareIgnoreCase(s1, s2) < 0;
}