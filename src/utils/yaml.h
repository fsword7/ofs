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
        ofsLogger->info("{}: {}\n", name, config[name].as<str_t>());
        return config[name].as<T>();
    }

    template <typename T>
    T getValueString(YAML::Node &config, cstr_t &name, T defValue = "")
    {
        if (!config[name].IsScalar())
            return defValue;
        ofsLogger->info("{}: {}\n", name, config[name].as<str_t>());
        return config[name].as<T>();
    }
};