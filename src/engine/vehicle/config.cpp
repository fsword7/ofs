// config.cpp - Vehicle json configuration package
//
// Author:  Tim Stark
// Date:    Jul 23, 2025

#include "main/core.h"
#include "api/draw.h"
#include "control/hud/panel.h"
#include "ephem/elements.h"
#include "engine/celestial.h"
#include "engine/rigidbody.h"
#include "engine/mesh.h"
#include "engine/vehicle/vehicle.h"
#include "universe/astro.h"
#include "universe/body.h"
#include "utils/json.h"

void Vehicle::configure(cjson &config, Celestial *object)
{
    // Set vehicle names
    str_t name = myjson::getString<str_t>(config, "name");
    if (name.empty()) {
        ofsLogger->error("Vehicle/json: required name - aborted\n");
        return;
    }
    str_t tagName, descName;
    int pos = name.find(':', 0);
    if (pos != str_t::npos) {
        tagName = name.substr(0, pos);
        descName = name.substr(pos+1, name.size());
    }
    setsName(tagName);
    addName(descName);

    // Loading vehicle module
    str_t modName = myjson::getString<str_t>(config, "module");
    if (modName.empty()) {
        ofsLogger->error("{}: Required module to load - aborted\n", getsName());
        return;
    }
    loadModule(modName);

    // Setup flight status
    str_t stName = myjson::getString<str_t>(config, "status");
    if (stName == "landed") {
        glm::dvec3 loc = myjson::getFloatArray<glm::dvec3, double>(config, "location");
        double dir = myjson::getFloat<double>(config, "heading");
        surfParam.setLanded(loc, dir, cbody);
    } else {
        ofsLogger->info("{}: Unknown flight status: {} - aborted\n",
            getsName(), stName);
        return;
    }

    ofsLogger->info("Vehicle name: {} desciption: {}\n", getsName(), getsName(1));
}
