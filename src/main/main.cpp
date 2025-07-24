// main.cpp - Main OFS routines
//
// Author:  Tim Stark
// Date:    Sep 4, 2022

#include "main/core.h"
#include "main/app.h"

class CoreApp;

int main(int argc, char **argv)
{
#ifdef OFS_HOME_DIR
    std::cout << "Home directory: " << OFS_HOME_DIR << std::endl;
    std::cout << "Library directory: " << OFS_LIBRARY_DIR << std::endl;
#endif // OFS_HOME_DIR
    std::cout << "Working directory: " << fs::current_path() << std::endl;

    ofsAppCore = new CoreApp();

    ofsAppCore->init();

    printf("Running world...\n");
    ofsAppCore->launch();
    ofsAppCore->run();

    ofsAppCore->cleanup();
    delete ofsAppCore;
    exit(0);
}