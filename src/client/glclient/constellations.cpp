// constellations.cpp - Constellation Renderer for OpenGL interface
//
// Author:  Tim Stark
// Date:    Mar 22, 2022

#include "main/core.h"
#include "universe/astro.h"
#include "universe/universe.h"
#include "client.h"
#include "scene.h"
#include "buffer.h"

void Scene::initConstellations()
{
    const Constellations &constellations = ofsGetConstellations();
    const std::vector<Asterism *> &asterisms = constellations.getAsterisms();

    pgmAsterism = shmgr.createShader("line");

    pgmAsterism->use();

    vaoAsterism = new VertexArray(1);
    vboAsterism = (VertexBuffer *)vaoAsterism->create(1, VertexArray::VBO);

    for (auto aster : asterisms)
        asterismLines += aster->hipList.size();

    logger->info("Total {} asterism lines\n", asterismLines);
    
    mvp = mat4Uniform(pgmAsterism->getID(), "mvp");

    pgmAsterism->release();
}

void Scene::renderConstellations()
{
    const Constellations &constellations = ofsGetConstellations();
    const StarDatabase &starlib = ofsGetStarDatabase();
    const std::vector<Asterism *> &asterisms = constellations.getAsterisms();

    glm::dvec3 vpos = ofsGetCameraGlobalPosition();
    int cLines = 0;

    AsterismVertex *vertices = nullptr;

    pgmAsterism->use();
    vaoAsterism->bind();

    vboAsterism->allocate(asterismLines * sizeof(AsterismVertex), nullptr, GL_DYNAMIC_DRAW);
    vertices = reinterpret_cast<AsterismVertex *>(vboAsterism->map());

    // if (vertices == nullptr)
    // {
    //     Logger::getLogger()->fatal("Can't render constellations - aborted (error code: {})\n",
    //         glGetError());
    //     vbufAsterism->release();
    //     pgmAsterism->release();
    //     return;
    // }

    int rLines = 0;
    for (int idx = 0; idx < asterisms.size(); idx++)
    {
        Asterism *aster = asterisms[idx];
        for (int sidx = 0; sidx < aster->hipList.size(); sidx += 2)
        {
            ObjectHandle star1 = starlib.getHIPstar2(aster->hipList[sidx]);
            ObjectHandle star2 = starlib.getHIPstar2(aster->hipList[sidx+1]);

            if (star1 == nullptr)
                logger->warn("HIP {} not found in catalogue\n", aster->hipList[sidx]);
            if (star2 == nullptr)
                logger->warn("HIP {} not found in catalogue\n", aster->hipList[sidx+1]);
            if (star1 == nullptr || star2 == nullptr)
                continue;

            vertices[rLines].spos = (ofsGetObjectStarPosition(star1) * KM_PER_PC) - vpos;
            vertices[rLines].color = color_t(0.5, 0.5, 0.5, 1.0);
            rLines++;
            vertices[rLines].spos = (ofsGetObjectStarPosition(star2) * KM_PER_PC) - vpos;
            vertices[rLines].color = color_t(0.5, 0.5, 0.5, 1.0);
            rLines++;
        }
    }

    vboAsterism->unmap();

    // if (!glUnmapBuffer(GL_ARRAY_BUFFER))
    // {
    //     Logger::getLogger()->fatal("Data buffer corruption - aborted (error code: {})\n", glGetError());
    //     vbufAsterism->release();
    //     pgmAsterism->release();
    //     return;
    // }

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(AsterismVertex), (void *)0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(AsterismVertex), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glm::dmat4 proj = ofsGetCameraProjectionMatrix();
    glm::dmat4 view = ofsGetCameraViewMatrix();
    mvp = glm::mat4(proj * view);

    glDrawArrays(GL_LINES, 0, rLines);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    vaoAsterism->unbind();
    pgmAsterism->release();
}