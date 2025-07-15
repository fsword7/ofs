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
    T getBoolean(cjson &config, cchar_t *name, T defValue = false)
    {
        if (!config.contains(name))
            return defValue;
        if (!config[name].is_boolean())
            return defValue;
        ofsLogger->info("JSON: {}: {}\n",
            name, config[name].get<T>());
        return config[name].get<T>();
    }

    template <typename T>
    T getInteger(cjson &config, cchar_t *name, T defValue = 0)
    {
        if (!config.contains(name))
            return defValue;
        if (!config[name].is_number_integer())
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
        if (!config[name].is_number())
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

    template <typename T, typename U>
    T getFloatArray(cjson &config, cchar_t *name, T defValue = {})
    {
        if (!config.contains(name))
            return defValue;
        if (!config[name].is_array())
            return defValue;
        T value = {};
        cjson &items = config[name];
        ofsLogger->info("JSON: {}: {}\n", name, items.dump());
        for (int idx = 0; idx < items.size(); idx++) {
            if (items[idx].is_number()) {
                value[idx] = items[idx].get<U>();
                ofsLogger->info("JSON: {}[{:d}]: {:f}\n",
                    name, idx, value[idx]);
            }
        }
        return value;
    }

    template <typename T, typename U>
    T getFloatArray2(cjson &items, T defValue = {})
    {
        if (!items.is_array())
            return defValue;
        T value = {};
        ofsLogger->info("JSON: array: {}\n", items.dump());
        for (int idx = 0; idx < items.size(); idx++) {
            if (items[idx].is_number()) {
                value[idx] = items[idx].get<U>();
                ofsLogger->info("JSON: [{:d}]: {:f}\n", idx, value[idx]);
            }
        }
        return value;
    }
};