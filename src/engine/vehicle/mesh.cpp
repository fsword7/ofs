// mesh.cpp - Vehicle package - mesh
//
// Author:  Tim Stark
// Date:    Oct 25, 2024

#define OFSAPI_SERVER_BUILD

#include "main/core.h"
#include "engine/mesh.h"
#include "engine/vehicle/vehicle.h"

void Vehicle::createMesh(Mesh *mesh, const glm::dvec3 &ofs)
{
    MeshEntry *entry = new MeshEntry();

    entry->meshName = "";
    entry->mesh = mesh;
    entry->meshofs = ofs;
    // entry->visualMode = meshExternal;

    meshList.push_back(entry);
}

void Vehicle::createMesh(cstr_t &name, const glm::dvec3 &ofs)
{
    MeshEntry *entry = new MeshEntry();

    entry->meshName = name;
    entry->mesh = nullptr;
    entry->meshofs = ofs;
    // entry->visualMode = meshExternal;

    meshList.push_back(entry);
}
