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

    cbody = object;

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

        // Converted to radians
        loc.x = ofs::radians(loc.x);
        loc.y = ofs::radians(loc.y);
        dir = ofs::radians(dir);

        initLanded(object, loc, dir);
    } else if (stName == "orbiting") {

        glm::dvec3 rpos, rvel;
        if (config.contains("elements")) {
            double el[NELEMENTS] = DEFAULT_ELEMENTS;
            double mjd, mjdref = ofsDate->getMJDReference();

            myjson::setFloatArray(config, "elements", el, ARRAY_SIZE(el));
            mjd = myjson::getFloat<double>(config, "mjd", mjdref);

            // orbital elements - convert degrees to radians
            el[2] = ofs::radians(el[2]); // inclination
            el[3] = ofs::radians(el[3]); // longtitude of ascending node
            el[4] = ofs::radians(el[4]); // longtitude of peripasis
            el[5] = ofs::radians(el[5]); // mean longtitude at epoch

            // initialize orbital elements
            el[0] *= M_PER_KM;
            oel.configure(el, mjd);
            oel.setup(mass, cbody->getMass(), ofsDate->getMJDReference());
            oel.start(ofsDate->getSimDeltaTime0(), rpos, rvel);
    
            ofsLogger->info("{}: rpos {:.3f},{:.3f},{:.3f} ({:.3f})\n", getsName(),
                rpos.x, rpos.y, rpos.z, glm::length(rpos));
            ofsLogger->info("{}: rvel {:.4f},{:.4f},{:.4f} - {:.4f} mph\n", getsName(),
                rvel.x, rvel.y, rvel.z, glm::length(rvel) * 3600 * 0.621);

        } else {
            rpos = myjson::getFloatArray<glm::dvec3, double>(config, "rpos");
            rvel = myjson::getFloatArray<glm::dvec3, double>(config, "rvel");
            oel.setup(mass, cbody->getMass(), ofsDate->getMJDReference());
        }

        double alt = myjson::getFloat<double>(config, "alt");
     
        glm::dvec3 arot;
        if (config.contains("arot")) {
            glm::dvec3 arot = myjson::getFloatArray<glm::dvec3, double>(config, "arot");

            // arot - convert degrees to radians
            arot.x = ofs::radians(arot.x);
            arot.y = ofs::radians(arot.y);
            arot.z = ofs::radians(arot.z);
        }

        glm::dvec3 vrot;
        if (config.contains("vrot")) {
            glm::dvec3 vrot = myjson::getFloatArray<glm::dvec3, double>(config, "vrot");

            // vrot - convert degrees to radians
            vrot.x = ofs::radians(vrot.x);
            vrot.y = ofs::radians(vrot.y);
            vrot.z = ofs::radians(vrot.z);
        }

        // ofsLogger->info("{}: rpos {},{},{}\n", getsName(), rpos.x, rpos.y, rpos.z);
        // ofsLogger->info("{}: rvel {},{},{}\n", getsName(), rvel.x, rvel.y, rvel.z);
        initOrbiting(rpos, rvel, arot, &vrot);

    } else {
        ofsLogger->info("{}: Unknown flight status: {} - aborted\n",
            getsName(), stName);
        return;
    }

    ofsLogger->info("Vehicle name: {} desciption: {}\n", getsName(), getsName(1));
}

void Vehicle::setDefaultPack()
{

    switch (fsType)
    {
    case fsFlight:
        break;
    }
}