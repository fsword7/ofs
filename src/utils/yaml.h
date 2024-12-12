// yaml.h - YAML package
//
// Author:  Tim Stark
// Date:    Apr 16, 2024

#pragma once

#include <yaml-cpp/yaml.h>

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

    template <class T, typename U>
    T getArray(YAML::Node &config, cstr_t &name, T defValue)
    {
        T val;
        if (!config[name].IsSequence())
            return defValue;
        for (int idx = 0; idx < config[name].size(); idx++)
            val[idx] = config[name][idx].as<U>();

        ofsLogger->info("{}: [ ", name);
        for (int idx = 0; idx < config[name].size(); idx++)
            ofsLogger->info("{}{}", config[name][idx].as<U>(),
            idx < config[name].size()-1 ? ", " : " ");
        ofsLogger->info("]\n");

        return val;
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