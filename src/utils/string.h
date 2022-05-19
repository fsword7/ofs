// strings.h - String utilities package
//
// Author:  Tim Stark
// Date:    May 18, 2022

#pragma once

#include <string_view>

int compareIgnoreCase(std::string_view s1, std::string_view s2);
int compareIgnoreCase(std::string_view s1, std::string_view s2, int n);

struct CompareIgnoreCasePredicate
{
    bool operator ()(std::string_view s1, std::string_view s2) const;
    // {
    //     return compareIgnoreCase(s1, s2) < 0;
    // }
};