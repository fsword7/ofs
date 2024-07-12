// yaml.h - YAML package
//
// Author:  Tim Stark
// Date:    Apr 16, 2024

#pragma once

#include <yaml.h>

namespace yaml
{
    template <typename T>
    T getValue(YAML::Node &config, cstr_t &name, T defValue = 0)
    {
        if (!config[name].IsScalar())
            return defValue;
        return config[name].as<T>();
    }
};