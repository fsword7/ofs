// json.h - JSON package
//
// Author:  Tim Stark
// Date:    Jun 29, 2025

#pragma once

// #include <nlohmann/json.hpp>

// using json = nlohmann::json;
// using cjson = const nlohmann::json;

namespace myjson
{

    template <typename T>
    T getInteger(cjson &config, cchar_t *name, T defValue = 0)
    {
        if (!config.contains(name))
            return defValue;
        if (!config[name].is_number_integer() &&
            !config[name].is_boolean())
            return defValue;
        ofsLogger->info("JSON: {}: {:d}\n",
            name, config[name].get<T>());
        return config[name].get<T>();
    }

    template <typename T>
    T getFloat(cjson &config, cchar_t *name, T defValue = 0)
    {
        if (!config.contains(name))
            return defValue;
        if (!config[name].is_number_float())
            return defValue;
        ofsLogger->info("JSON: {}: {:f}\n",
            name, config[name].get<T>());
        return config[name].get<T>();
    }

    template <typename T>
    T getString(cjson &config, cchar_t *name, T defValue = "")
    {
        if (!config.contains(name))
            return defValue;
        if (!config[name].is_string())
            return defValue;
        ofsLogger->info("JSON: {}: {}\n",
            name, config[name].get<cstr_t>());
        return config[name].get<T>();
    }
    
    // template <typename T>
    // T getBoolean(cjson &config, cstr_t &name, T defValue = false)
    // {
    //     if (!config[name].is_boolean())
    //         return defValue;
    //     ofsLogger->info("JSON: {}: {}\n",
    //         name, config[name].get<bool>());
    //     return config[name].get<bool>();
    // }
};