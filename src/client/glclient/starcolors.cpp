// starcolors.cpp - Star color temperature package
//
// Author:  Tim Stark
// Date:    Oct 10, 2022

#include "main/core.h"
#include "client.h"
#include "starcolors.h"

bool StarColors::load(const fs::path &fname)
{
    std::ifstream data(fname, std::ios::in);
    std::string   line;
    int lineno;
    int temp;
    float r, g, b;
    color_t color;

    if (!data.is_open())
    {
        glLogger->error("I/O error: file: {}: {}\n",
            fname.string(), strerror(errno));
        return false;
    }

    lineno = 0;
    while(std::getline(data, line))
    {
        std::stringstream lineStream(line);
        str_t cell;
        std::vector<str_t> cells;

        if (lineno++ == 0)
            continue;
        if (line.size() == 0)
            continue;
        if (line[0] == '#')
            continue;

        cells.clear();
        while (!lineStream.eof())
        {
            str_t tmp;
            lineStream >> tmp;
            if (tmp.empty())
                continue;
            cells.push_back(tmp);
        }
        if (cells.size() != D58_nSIZE)
            continue;

        sscanf(cells[D58_nTEMP].c_str(), "%d", &temp);
        sscanf(cells[D58_nFRED].c_str(), "%f", &r);
        sscanf(cells[D58_nFGREEN].c_str(), "%f", &g);
        sscanf(cells[D58_nFBLUE].c_str(), "%f", &b);

        color = { r, g, b };
        if (cells[D58_nCMF] == "2deg")
        {
            colors2.push_back(color);
            temps.push_back(temp);
        }
        if (cells[D58_nCMF] == "10deg")
            colors10.push_back(color);
    }
    minTemp = temps[0];
    maxTemp = temps[temps.size()-1];
    scaleTemp = (maxTemp - minTemp) / (temps.size()-1);

    glLogger->info("Total {} black body colors (2deg)\n", colors2.size());
    glLogger->info("Total {} black body colors (10deg)\n", colors10.size());
    glLogger->info("Range temperature: {} to {} (scale: {})\n",
        minTemp, maxTemp, scaleTemp);
    return true;
}

color_t StarColors::lookup(int temp) const
{
    color_t color;

    if (temp < minTemp)
        return colors2[0];
    if (temp > maxTemp)
        return colors2[colors2.size()-1];
    return colors2[(temp - minTemp) / scaleTemp];
}