// starcolors.h - Star color temperature package
//
// Author:  Tim Stark
// Date:    Oct 10, 2022

#pragma once

#define D58_nTEMP       0   // Surface temperature
#define D58_nK          1   // K
#define D58_nCMF        2   // CIE 1931/CIE 1924
#define D58_nX          3   // Chromaticity coordinates
#define D58_nY          4   //
#define D58_nP          5   // Power in semi-arbitary units
#define D58_nFRED       6   // Red - HDR normalized
#define D58_nFGREEN     7   // Green - HDR normalized
#define D58_nFBLUE      8   // Blue - HDR normalized
#define D58_nIRED       9   // Red - SDR 8-bit integer
#define D58_nIGREEN     10  // Green - SDR 8-bit integer
#define D58_nIBLUE      11  // Blue - SDR 8-bit integer
#define D58_nRGB        12  // RGB SDR 8-bit value
#define D58_nSIZE       13

class StarColors
{
public:
    StarColors() = default;
    ~StarColors() = default;

    bool load(const fs::path &fname);

    color_t lookup(int temp) const;

private:
    std::vector<color_t> colors2;
    std::vector<color_t> colors10;
    std::vector<int> temps;
    int minTemp = 0;
    int maxTemp = 0;
    double scaleTemp = 0.0;
};