// loader.cpp - Terrain Handler package
//
// Author:  Tim Stark
// Date:    Mar 12, 2022

#include "main/core.h"
#include "client.h"
// #include "mesh.h"
#include "scene.h"
#include "vobject.h"
#include "surface.h"

SurfaceHandler::SurfaceHandler()
{
    msFreq = 50;
    start();
}

SurfaceHandler::~SurfaceHandler()
{
    if (loader.joinable())
        shutdown();
}

void SurfaceHandler::start()
{
    // Start handle() in separate thread process
    loader = std::thread([this]{ handle(); });
}

void SurfaceHandler::shutdown()
{
    // terminate handle() in separate thread process
    runHandler = false;
    loader.join();
}

void SurfaceHandler::handle()
{
    const int nTiles = 12;
    SurfaceTile *tile[nTiles];
    int nCount = 0;

    runHandler = true;
    while (runHandler)
    {
        // Loading tile(s) from queue
        if (!tiles.empty())
        {
            std::unique_lock<std::mutex> mu1(muQueue);
            while (!tiles.empty() && (nCount < nTiles))
            {
                tile[nCount++] = tiles.front();
                tiles.pop();
            }
            mu1.unlock();
        }

        // Loading texture images
        if (nCount > 0)
        {
            std::unique_lock<std::mutex> mu2(muLoading);
            for (int idx = 0; idx < nCount; idx++)
                tile[idx]->load();
            mu2.unlock();
            nCount = 0;
        }

        // Periodic idle
        std::this_thread::sleep_for(std::chrono::milliseconds(msFreq));
    }
}

void SurfaceHandler::queue(SurfaceTile *tile)
{
    if (tile == nullptr)
        return;
    if (tile->type == SurfaceTile::tileInQueue)
        return;
    
    tile->type = SurfaceTile::tileInQueue;
    tiles.push(tile);
}

bool SurfaceHandler::unqueue(SurfaceTile *tile)
{
    if (tile == nullptr)
        return false;
    if (tiles.empty())
        return false;
    if (tile->type != SurfaceTile::tileInQueue)
        return false;

    std::unique_lock<std::mutex> mu(muQueue);

    mu.unlock();
    return true;
}

void SurfaceHandler::unqueue(SurfaceManager *mgr)
{
    if (mgr == nullptr)
        return;
    if (tiles.empty())
        return;
}