// constellations.h - Constellation database package
//
// Author:  Tim Stark
// Date:    Apr 23, 2022

#pragma once

struct Asterism
{
    str_t               fullName;
    str_t               shortName;
    int                 nLines;
    std::vector<int>    hipList;
};

class OFSAPI Constellations
{
public:
    Constellations() = default;
    ~Constellations() = default;

    bool load(const fs::path &fname);

    const std::vector<Asterism *> &getAsterisms() const { return asterisms; }

private:
    std::vector<Asterism *> asterisms;
};