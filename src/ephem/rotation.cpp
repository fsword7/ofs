// rotation.cpp - Planet Precession/Rotation Package
//
// Author:  Tim Stark
// Date:    Apr 15, 2024

#include "main/core.h"
#include "ephem/rotation.h"
#include "utils/yaml.h"

// **** Base Rotation ****


Rotation *Rotation::create(YAML::Node &config)
{
    Rotation *rot = nullptr;

    cstr_t name = yaml::getValue<str_t>(config, "Rotation");
    if (name == "Uniform")
        rot = new RotationUniform(config);

    return rot;
}

// **** Uniformed Rotation ****

RotationUniform::RotationUniform(YAML::Node &config)
{
    configure(config);
}

void RotationUniform::configure(YAML::Node &config)
{
    // Get sideral rotation parameters
    Dphi = yaml::getValue<double>(config, "SidRotOffset");
    rotT = yaml::getValue<double>(config, "SidRotPeriod");

    // Get prcession parameters
    Lrel0   = yaml::getValue<double>(config, "LAN");
    mjd_rel = yaml::getValue<double>(config, "LAN_MJD");
    eps_rel = yaml::getValue<double>(config, "Obliquity");
    lan_ref = yaml::getValue<double>(config, "PrecessionLAN");
    eps_ref = yaml::getValue<double>(config, "PrecessionObliquity");
    precT   = yaml::getValue<double>(config, "PrecessionPeriod");
}

double RotationUniform::spin(double t)
{
    return ofs::posangle(Dphi + rotOmega*t + crotofs);
}

void RotationUniform::update(double mjd)
{
    Lrel = Lrel0 + precOmega * (mjd - mjd_rel);
    double sinl = sin(Lrel), cosl = cos(Lrel);

    glm::dmat3 Rrel = { cosl,   -sinl*sin_eps,      -sinl*cos_eps, 
                        0,      cos_eps,            -sin_eps,
                        sinl,   cosl*sin_eps,       cosl*cos_eps };

    if (eps_ref)
        R_ref_rel = R_ref * R_ref_rel;
    
    Raxis = R_ref_rel * glm::dvec3( 0, 1, 0 );
    eps_ecl = acos(Raxis.y);
    lan_ecl = atan2(-Raxis.y, Raxis.z);

    double sinL = sin(lan_ecl), cosL = cos(lan_ecl);
    double sine = sin(eps_ecl), cose = cos(eps_ecl);

    Recl = { cosL,  -sinL*sine, -sinL*cose,
            0,      cose,       -sine,
            sinL,   cosL*sine,  cosL*cose };
    
    double cos_poff = cosL*R_ref_rel[0][0] * sinL*R_ref_rel[2][0];
    double sin_poff = -(cosL*R_ref_rel[0][2] * sinL*R_ref_rel[2][2]);
    rotOffset = atan2(sin_poff, cos_poff);
}