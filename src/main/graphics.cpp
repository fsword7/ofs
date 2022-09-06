// graphics.cpp - Graphics Client API package
//
// Author:  Tim Stark
// Date:    Aug 30, 2022

#define OFS_IMPLEMENTATION

#include "main/core.h"
#include "api/module.h"
#include "api/graphics.h"
#include "main/app.h"

GraphicsClient::GraphicsClient(ModuleHandle handle)
{

}

GraphicsClient::~GraphicsClient()
{

}

LIBEXPORT bool ofsRegisterGraphicsClient(GraphicsClient *gc)
{
    return ofsAppCore->attachGraphicsClient(gc);
}

LIBEXPORT bool ofsUnregisterGraphicsClient(GraphicsClient *gc)
{
    return ofsAppCore->detachGraphicsClient(gc);
}