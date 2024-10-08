// main.cpp - Main OFS routines
//
// Author:  Tim Stark
// Date:    Sep 4, 2022

#include "main/core.h"
#include "main/app.h"

class CoreApp;

int main(int argc, char **argv)
{
    std::cout << "Working directory: " << fs::current_path() << std::endl;

    ofsAppCore = new CoreApp();

    ofsAppCore->init();

    printf("Running world...\n");
    ofsAppCore->run();

    ofsAppCore->cleanup();
    delete ofsAppCore;
    exit(0);
}