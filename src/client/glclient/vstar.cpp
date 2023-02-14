// vstar.cpp - Visual Celestial Star package
//
// Author:  Tim Stark
// Date:    Sep 13, 2022

#include "main/core.h"
#include "engine/object.h"
#include "vobject.h"
#include "vstar.h"

#include <unordered_map>

vStar::vStar(const Object *object, Scene &scene)
: vObject(object, scene)
{

}

vStar::~vStar()
{
    
}

// Seed of Andromeda Icosphere Generator
// Written by Frank McCoy
// Use it for whatever, but remember where you got it from.

// http://www.songho.ca/opengl/gl_sphere.html
// https://en.wikipedia.org/wiki/Regular_icosahedron
// https://en.wikipedia.org/wiki/Geodesic_polyhedron

const static double GR = (1 + sqrt(5)) / 2; // Golden Ratio
const static int nIcosahedronVertices = 12; // 10T + 2
const static glm::dvec3 IcosahedronVertices[12] =
{
    glm::dvec3(-1.0, GR, 0.0),
    glm::dvec3(1.0, GR, 0.0),
    glm::dvec3(-1.0, -GR, 0.0),
    glm::dvec3(1.0, -GR, 0.0),

    glm::dvec3(0.0, -1.0, GR),
    glm::dvec3(0.0, 1.0, GR),
    glm::dvec3(0.0, -1.0, -GR),
    glm::dvec3(0.0, 1.0, -GR),

    glm::dvec3(GR, 0.0, -1.0),
    glm::dvec3(GR, 0.0, 1.0),
    glm::dvec3(-GR, 0.0, -1.0),
    glm::dvec3(-GR, 0.0, 1.0)
};

const static int nIcosahedronIndices = 60;
const static uint32_t IcosahedronIndices[60] =
{
    0, 11, 5,
    0, 5, 1,
    0, 1, 7,
    0, 7, 10,
    0, 10, 11,

    1, 5, 9,
    5, 11, 4,
    11, 10, 2,
    10, 7, 6,
    7, 1, 8,

    3, 9, 4,
    3, 4, 2,
    3, 2, 6,
    3, 6, 8,
    3, 8, 9,

    4, 9, 5,
    2, 4, 11,
    6, 2, 10,
    8, 6, 7,
    9, 8, 1
};

class dvec3Keys
{
public:
    size_t operator () (const glm::dvec3 &k) const
    {
        return std::hash<double>()(k.x) ^ std::hash<double>()(k.y) ^ std::hash<double>()(k.z);
    }

    bool operator () (const glm::dvec3 &a, const glm::dvec3 &b) const
    {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }
};

inline glm::dvec3 getMidpoints(const glm::dvec3 &v1, const glm::dvec3 &v2)
{
    return glm::normalize(glm::dvec3((v1.x + v2.x) / 2.0, (v1.y + v2.y) / 2.0, (v1.z + v2.z) / 2.0));
}

void vStar::createSphere(int maxlod)
{
    std::vector<uint32_t>   indices;
    std::vector<glm::dvec3> vertices;

    std::unordered_map<glm::dvec3, uint32_t, dvec3Keys, dvec3Keys> vertexLookup;

    indices.resize(nIcosahedronIndices);
    for (auto index : IcosahedronIndices)
        indices.push_back(index);
    
    vertices.resize(nIcosahedronVertices);
    for (auto pos : IcosahedronVertices)
        vertices.push_back(pos);
    
    for (size_t lod = 0; lod < maxlod; lod++)
    {
        std::vector<uint32_t> newIndices;

        for (int idx = 0; idx < indices.size(); idx++)
        {
            glm::dvec3 v1 = vertices[indices[idx + 0]];
            glm::dvec3 v2 = vertices[indices[idx + 1]];
            glm::dvec3 v3 = vertices[indices[idx + 2]];

            glm::dvec3 mp12 = getMidpoints(v1, v2);
            glm::dvec3 mp23 = getMidpoints(v2, v3);
            glm::dvec3 mp13 = getMidpoints(v1, v3);

            uint32_t mp12idx, mp23idx, mp13idx;

            auto iter = vertexLookup.find(mp12);
            if (iter != vertexLookup.end())
                mp12idx = iter->second;
            else 
            {
                mp12idx = vertices.size();
                vertices.push_back(mp12);
                vertexLookup[mp12] = mp12idx;
            }

            iter = vertexLookup.find(mp23);
            if (iter != vertexLookup.end())
                mp23idx = iter->second;
            else 
            {
                mp23idx = vertices.size();
                vertices.push_back(mp23);
                vertexLookup[mp23] = mp23idx;
            }

            iter = vertexLookup.find(mp13);
            if (iter != vertexLookup.end())
                mp13idx = iter->second;
            else 
            {
                mp13idx = vertices.size();
                vertices.push_back(mp13);
                vertexLookup[mp13] = mp13idx;
            }

            newIndices.push_back(indices[idx]);
            newIndices.push_back(mp12idx);
            newIndices.push_back(mp13idx);

            newIndices.push_back(mp12idx);
            newIndices.push_back(indices[idx+1]);
            newIndices.push_back(mp23idx);

            newIndices.push_back(mp13idx);
            newIndices.push_back(mp23idx);
            newIndices.push_back(indices[idx+2]);

            newIndices.push_back(mp12idx);
            newIndices.push_back(mp23idx);
            newIndices.push_back(mp13idx);
        }

        indices.swap(newIndices);
        newIndices.clear();
     }
}

// /// Seed of Andromeda Icosphere Generator
// /// Written by Frank McCoy
// /// Use it for whatever, but remember where you got it from.

// #include <cstdint>
// #include <unordered_map>
// #include <vector>
// #include <glm/glm.hpp>
// // GLM is awesome ^ ^ ^

// const static float GOLDEN_RATIO = 1.61803398875f;

// const static int NUM_ICOSOHEDRON_VERTICES = 12;
// const static glm::vec3 ICOSOHEDRON_VERTICES[12] = {
//     glm::vec3(-1.0f, GOLDEN_RATIO, 0.0f),
//     glm::vec3(1.0f, GOLDEN_RATIO, 0.0f),
//     glm::vec3(-1.0f, -GOLDEN_RATIO, 0.0f),
//     glm::vec3(1.0f, -GOLDEN_RATIO, 0.0f),

//     glm::vec3(0.0f, -1.0f, GOLDEN_RATIO),
//     glm::vec3(0.0f, 1.0f, GOLDEN_RATIO),
//     glm::vec3(0.0f, -1.0, -GOLDEN_RATIO),
//     glm::vec3(0.0f, 1.0f, -GOLDEN_RATIO),

//     glm::vec3(GOLDEN_RATIO, 0.0f, -1.0f),
//     glm::vec3(GOLDEN_RATIO, 0.0f, 1.0f),
//     glm::vec3(-GOLDEN_RATIO, 0.0f, -1.0f),
//     glm::vec3(-GOLDEN_RATIO, 0.0, 1.0f)
// };

// const static int NUM_ICOSOHEDRON_INDICES = 60;
// const static uint32_t ICOSOHEDRON_INDICES[60] = {
//     0, 11, 5,
//     0, 5, 1,
//     0, 1, 7,
//     0, 7, 10,
//     0, 10, 11,

//     1, 5, 9,
//     5, 11, 4,
//     11, 10, 2,
//     10, 7, 6,
//     7, 1, 8,

//     3, 9, 4,
//     3, 4, 2,
//     3, 2, 6,
//     3, 6, 8,
//     3, 8, 9,

//     4, 9, 5,
//     2, 4, 11,
//     6, 2, 10,
//     8, 6, 7,
//     9, 8, 1
// };

// // Hash functions for the unordered map
// class Vec3KeyFuncs {
// public:
//     size_t operator()(const glm::vec3& k)const {
//         return std::hash<float>()(k.x) ^ std::hash<float>()(k.y) ^ std::hash<float>()(k.z);
//     }

//     bool operator()(const glm::vec3& a, const glm::vec3& b)const {
//         return a.x == b.x && a.y == b.y && a.z == b.z;
//     }
// };

// inline glm::vec3 findMidpoint(glm::vec3 vertex1, glm::vec3 vertex2) {
//     return glm::normalize(glm::vec3((vertex1.x + vertex2.x) / 2.0f, (vertex1.y + vertex2.y) / 2.0f, (vertex1.z + vertex2.z) / 2.0f));
// }

// /// Generates an icosphere with radius 1.0f.
// /// @param lod: Number of subdivisions
// /// @param indices: Resulting indices for use with glDrawElements
// /// @param positions: Resulting vertex positions
// void generateIcosphereMesh(size_t lod, std::vector<uint32_t>& indices, std::vector<glm::vec3>& positions) {
//     std::vector<uint32_t> newIndices;
//     newIndices.reserve(256);

//     std::unordered_map<glm::vec3, uint32_t, Vec3KeyFuncs, Vec3KeyFuncs> vertexLookup;
    
//     indices.resize(NUM_ICOSOHEDRON_INDICES);
//     for (uint32_t i = 0; i < NUM_ICOSOHEDRON_INDICES; i++) {
//         indices[i] = ICOSOHEDRON_INDICES[i];
//     }
//     positions.resize(NUM_ICOSOHEDRON_VERTICES);
//     for (uint32_t i = 0; i < NUM_ICOSOHEDRON_VERTICES; i++) {
//         positions[i] = glm::normalize(ICOSOHEDRON_VERTICES[i]);
//         vertexLookup[glm::normalize(ICOSOHEDRON_VERTICES[i])] = i;
//     }

//     for (size_t i = 0; i < (size_t)lod; i++) {
//         for (size_t j = 0; j < indices.size(); j += 3) {
//             /*
//             j
//             mp12   mp13
//             j+1    mp23   j+2
//             */
//             // Defined in counter clockwise order
//             glm::vec3 vertex1 = positions[indices[j + 0]];
//             glm::vec3 vertex2 = positions[indices[j + 1]];
//             glm::vec3 vertex3 = positions[indices[j + 2]];

//             glm::vec3 midPoint12 = findMidpoint(vertex1, vertex2);
//             glm::vec3 midPoint23 = findMidpoint(vertex2, vertex3);
//             glm::vec3 midPoint13 = findMidpoint(vertex1, vertex3);

//             uint32_t mp12Index;
//             uint32_t mp23Index;
//             uint32_t mp13Index;

//             auto iter = vertexLookup.find(midPoint12);
//             if (iter != vertexLookup.end()) { // It is in the map
//                 mp12Index = iter->second;
//             } else { // Not in the map
//                 mp12Index = (uint32_t)positions.size();
//                 positions.push_back(midPoint12);
//                 vertexLookup[midPoint12] = mp12Index;
//             }

//             iter = vertexLookup.find(midPoint23);
//             if (iter != vertexLookup.end()) { // It is in the map
//                 mp23Index = iter->second;
//             } else { // Not in the map
//                 mp23Index = (uint32_t)positions.size();
//                 positions.push_back(midPoint23);
//                 vertexLookup[midPoint23] = mp23Index;
//             }

//             iter = vertexLookup.find(midPoint13);
//             if (iter != vertexLookup.end()) { // It is in the map
//                 mp13Index = iter->second;
//             } else { // Not in the map
//                 mp13Index = (uint32_t)positions.size();
//                 positions.push_back(midPoint13);
//                 vertexLookup[midPoint13] = mp13Index;
//             }

//             newIndices.push_back(indices[j]);
//             newIndices.push_back(mp12Index);
//             newIndices.push_back(mp13Index);

//             newIndices.push_back(mp12Index);
//             newIndices.push_back(indices[j + 1]);
//             newIndices.push_back(mp23Index);

//             newIndices.push_back(mp13Index);
//             newIndices.push_back(mp23Index);
//             newIndices.push_back(indices[j + 2]);

//             newIndices.push_back(mp12Index);
//             newIndices.push_back(mp23Index);
//             newIndices.push_back(mp13Index);
//         }
//         indices.swap(newIndices);
//         newIndices.clear();
//     }
// }