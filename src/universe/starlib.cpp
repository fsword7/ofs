// starlib.cpp - Star database library package
//
// Author:  Tim Stark
// Date:    Apr 18, 2022

#include "main/core.h"
#include "universe/star.h"
#include "universe/startree.h"
#include "universe/starlib.h"
#include "universe/handle.h"
#include "universe/universe.h"
#include "universe/astro.h"
#include "universe/xhipdata.h"

bool StarDatabase::loadXHIPData(const fs::path &pname)
{
    fs::path mfname = pname / "main.dat";
    fs::path pfname = pname / "photo.dat";
    fs::path bfname = pname / "biblio.dat";
    fs::path gfname = pname / "groups.dat";

    std::ifstream mdata(mfname, std::ios::in);
    std::ifstream pdata(pfname, std::ios::in);
    std::ifstream bdata(bfname, std::ios::in);
    // std::ifstream gdata(gfname, std::ios::in);

    if (!mdata.is_open())
    {
        fmt::printf("File '%s': %s\n", mfname.string(), strerror(errno));
        return false;
    }

    if (!pdata.is_open())
    {
        fmt::printf("File '%s': %s\n", pfname.string(), strerror(errno));
        pdata.close();
        return false;
    }

    if (!bdata.is_open())
    {
        fmt::printf("File '%s': %s\n", bfname.string(), strerror(errno));
        mdata.close();
        pdata.close();
        return false;
    }

    celStar *star;
    str_t mline, pline, bline;
    int lineno = 0;
    int hip, phip, bhip;
    int hd;

    // Star parameters
    double ra, de, plx, epix, dist, edist;
    double bMag, vMag, ci, lum;
    char   spType[26];
    
    int    cnplx = 0, czplx = 0;

    // Create the Sun (Sol)
    star = celStar::createTheSun();
    uStars.push_back(star);

    while (std::getline(mdata, mline) &&
           std::getline(pdata, pline) &&
           std::getline(bdata, bline))
    {
        std::stringstream mlStream(mline);
        std::stringstream plStream(pline);
        std::stringstream blStream(bline);

        str_t cell;
        std::vector<str_t> mcells, pcells, bcells;

        lineno++;

        mcells.clear();
        pcells.clear();
        bcells.clear();

        while (std::getline(mlStream, cell, '|'))
            mcells.push_back(cell);
        while (std::getline(plStream, cell, '|'))
            pcells.push_back(cell);
        while (std::getline(blStream, cell, '|'))
            bcells.push_back(cell);

        sscanf(mcells[XHIP_M_nHIP].c_str(), "%d", &hip);
        sscanf(pcells[XHIP_P_nHIP].c_str(), "%d", &phip);
        sscanf(bcells[XHIP_B_nHIP].c_str(), "%d", &bhip);
        if (hip != phip || hip != bhip)
        {
            fmt::printf("HIP %d - data mismatch (%d, %d, %d)\n", hip, hip, phip, bhip);
            break;
        }

        sscanf(mcells[XHIP_M_nRADEG].c_str(), "%lf", &ra);
        sscanf(mcells[XHIP_M_nDEDEG].c_str(), "%lf", &de);
        sscanf(mcells[XHIP_M_nPLX].c_str(), "%lf", &plx);
        sscanf(mcells[XHIP_M_nSPTYPE].c_str(), "%s", spType);

        sscanf(pcells[XHIP_P_nVAPPMAG].c_str(), "%lf", &vMag);
        if (sscanf(pcells[XHIP_P_nBAPPMAG].c_str(), "%lf", &bMag) != 1)
            bMag = vMag;
        ci = bMag - vMag;

        sscanf(pcells[XHIP_P_nLUM].c_str(), "%lf", &lum);

        if (plx < 0)
        {
            dist = 100000;
            cnplx++;
        }
        else if (plx == 0.0)
        {
            dist = 100000;
            czplx++;
        }
        else
        {
            dist = 1000.0 / plx;
        }

        star = celStar::create(ra, de, dist, spType, vMag, ci, lum);
        if (star != nullptr)
        {
            star->setHIPnumber(hip);
            star->setsName(bcells[XHIP_B_nNAME]);
            uStars.push_back(star);
        }
    }

    mdata.close();
    pdata.close();
    bdata.close();

    fmt::printf("Total %d stars with negative parallex.\n", cnplx);
    fmt::printf("Total %d stars with zero parallel.\n", czplx); 

    finish();
    
    return true;
}

void StarDatabase::addStar(celStar *star)
{
    uStars.push_back(star);
}

void StarDatabase::initOctreeData(std::vector<celStar *> stars)
{
    double absMag = astro::convertAppToAbsMag(STARTREE_MAGNITUDE,
        STARTREE_ROOTSIZE * sqrt(3.0));

    starTree = new StarTree({ 1000.0, 1000.0, 1000.0 }, absMag);
    for (int idx = 0; idx < uStars.size(); idx++)
        starTree->insert(*uStars[idx], STARTREE_ROOTSIZE);

    fmt::printf("Star database has %d nodes and %d objects\n",
        starTree->countNodes(), starTree->countObjects());
}

void StarDatabase::finish()
{
    fmt::printf("Total star count: %d\n", uStars.size());

    initOctreeData(uStars);

    // Initialize HIP star catalogue
    int hip, maxHip = 0;
    for (int idx = 0; idx < uStars.size(); idx++)
    {
        hip = uStars[idx]->getHIPnumber();
        if (hip > maxHip)
            maxHip = hip;
    }
    hipList = new celStar*[maxHip];
    for (int idx = 0; idx < maxHip; idx++)
        hipList[idx] = nullptr;
    for (int idx = 0; idx < uStars.size(); idx++)
        hipList[uStars[idx]->getHIPnumber()] = uStars[idx];
}

celStar *StarDatabase::find(cstr_t &name) const
{
    for (int idx = 0; idx < uStars.size(); idx++)
        if (uStars[idx]->getName() == name)
            return uStars[idx];        
    return nullptr;
}

void StarDatabase::findVisibleStars(const ofsHandler &handle, const vec3d_t &obs,
    const quatd_t &rot, double fov, double aspect, double limitMag) const
{
    if (starTree == nullptr)
        return;
    starTree->processVisibleStars(handle, obs / KM_PER_PC, limitMag, STARTREE_ROOTSIZE);
}

int StarDatabase::findCloseStars(const vec3d_t &obs, double radius,
    std::vector<const celStar *> &stars) const
{
    if (starTree == nullptr)
        return 0;
    starTree->processCloseStars(obs / KM_PER_PC, radius, STARTREE_ROOTSIZE, stars);
    return stars.size();
}
