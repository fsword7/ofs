// constellations.cpp - Constellation database package
//
// Author:  Tim Stark
// Date:    Apr 23, 2022

#define OFSAPI_SERVER_BUILD

#include "main/core.h"
#include "universe/constellations.h"

bool Constellations::load(const fs::path &fname)
{
    std::ifstream data(fname, std::ios::in);
    str_t line;
    int   lineno;
    int   nAsterisms;

    Asterism *asterism;

    if (!data.is_open())
    {
        ofsLogger->error("File {}: {}\n", fname.string(), strerror(errno));
        return false;
    }

    lineno = 0;
    nAsterisms = 0;

    while (std::getline(data, line))
    {
        std::stringstream lineStream(line);
        std::vector<str_t> cells;
        str_t cell;

        if (line.empty())
            continue;
        if (line[0] == '#')
            continue;
        if (line[0] != '.')
            nAsterisms++;

        cells.clear();
        while (std::getline(lineStream, cell, ' '))
            cells.push_back(cell);
        
        asterism = new Asterism();
        asterism->shortName = cells[0];
        sscanf(cells[1].c_str(), "%d", &asterism->nLines);
        asterism->hipList.clear();
        for (int idx = 2; idx < cells.size(); idx++)
        {
            int hip;

            if (cells[idx].empty())
                continue;
            sscanf(cells[idx].c_str(), "%d", &hip);
            asterism->hipList.push_back(hip);
        }

        asterisms.push_back(asterism);
    }

    data.close();
    ofsLogger->info("Total {} constellations\n", nAsterisms);

    return true;
}