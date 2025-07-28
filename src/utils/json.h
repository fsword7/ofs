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
        ofsLogger->debug("JSON: {}: {}\n",
            name, config[name].dump());
        // ofsLogger->info("JSON: {}: {}\n",
        //     name, config[name].get<T>());
        return config[name].get<T>();
    }

    template <typename T>
    T getInteger(cjson &config, cchar_t *name, T defValue = 0)
    {
        if (!config.contains(name))
            return defValue;
        if (!config[name].is_number_integer())
            return defValue;
        ofsLogger->debug("JSON: {}: {}\n",
            name, config[name].dump());
        // ofsLogger->info("JSON: {}: {:d}\n",
        //     name, config[name].get<T>());
        return config[name].get<T>();
    }

    template <typename T>
    T getFloat(cjson &config, cchar_t *name, T defValue = 0)
    {
        if (!config.contains(name))
            return defValue;
        if (!config[name].is_number())
            return defValue;
        ofsLogger->debug("JSON: {}: {}\n",
            name, config[name].dump());
        // ofsLogger->info("JSON: {}: {:f}\n",
        //     name, config[name].get<T>());
        return config[name].get<T>();
    }

    template <typename T>
    T getString(cjson &config, cchar_t *name, T defValue = "")
    {
        if (!config.contains(name))
            return defValue;
        if (!config[name].is_string())
            return defValue;
        ofsLogger->debug("JSON: {}: {}\n",
            name, config[name].dump());
        // ofsLogger->info("JSON: {}: {}\n",
        //     name, config[name].get<cstr_t>());
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
        ofsLogger->debug("JSON: {}: {}\n",
            name, items.dump());
        for (int idx = 0; idx < items.size(); idx++) {
            if (items[idx].is_number()) {
                value[idx] = items[idx].get<U>();
                ofsLogger->info("JSON: {}[{:d}]: {:f}\n",
                    name, idx, value[idx]);
            }
        }
        return value;
    }

    template <typename T>
    void setFloatArray(cjson &items, T *val, int size)
    {
        if (!items.is_array())
            return;
        ofsLogger->debug("JSON: array: {}\n", items.dump());
        for (int idx = 0; idx < size || idx < items.size(); idx++) {
            if (items[idx].is_number()) {
                val[idx] = items[idx].get<T>();
                ofsLogger->info("JSON: [{:d}]: {:f}\n", idx, val[idx]);
            }
        }
    }

    template <typename T>
    void setFloatArray(cjson &config, cchar_t *name, T *val, int size)
    {
        if (!config.contains(name))
            return;
        cjson &items = config[name];
        if (!items.is_array())
            return;
        ofsLogger->debug("JSON: {}: {}\n", name, items.dump());
        for (int idx = 0; idx < size || idx < items.size(); idx++) {
            if (items[idx].is_number()) {
                val[idx] = items[idx].get<T>();
                ofsLogger->info("JSON: {}[{:d}]: {:f}\n", name, idx, val[idx]);
            }
        }
    }
};