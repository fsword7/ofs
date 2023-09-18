// elp82b.cpp - ELP82B Data Generator package
//
// Author:  Tim Stark
// Date:    Sep 18, 2023

#include <iostream>
#include <fstream>
#include <string>
#include <fmt/format.h>

struct main_t
{
    int ilu[4];
    double coef[7];
};

void print(main_t *block, int nterms)
{
    for (int term = 0; term < nterms; term++)
    {
        main_t &b = block[term];

        std::cout << fmt::format("\t{{{{{:3d},{:3d},{:3d},{:3d}}},{{{:12.5f},{:10.2f},{:10.2f},{:10.2f},{:10.2f},{:10.2f},{:10.2f}}}}}\n",
            b.ilu[0], b.ilu[1], b.ilu[2], b.ilu[3],
            b.coef[0], b.coef[1], b.coef[2], b.coef[3],
            b.coef[4], b.coef[5], b.coef[6]);
    }
}

int main(int argc, char **argv)
{
    std::string line;

    {
        std::ifstream inFile(argv[1]);

        int nterms = 0;
        std::getline(inFile, line);
        while(!inFile.eof())
        {
            std::getline(inFile, line);
            if (line.empty())
                continue;
            nterms++;
        }

        // Reset file position
        inFile.clear();
        inFile.seekg(0, inFile.beg);
        std::getline(inFile, line); // ignore headline line

        std::cout << "Total terms = " << nterms << std::endl;

        main_t *block = new main_t[nterms];

        for (int term = 0; term < nterms; term++)
        {
            for (int idx = 0; idx < 4; idx++)
                inFile >> block[term].ilu[idx];
            for (int idx = 0; idx < 7; idx++)
                inFile >> block[term].coef[idx];
        }
        print(block, nterms);

        delete block;

        inFile.close();
    }
}