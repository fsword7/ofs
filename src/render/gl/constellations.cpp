// constellations.cpp - Constellation Renderer for OpenGL interface
//
// Author:  Tim Stark
// Date:    Mar 22, 2022

#include "main/core.h"
#include "osd/gl/context.h"
#include "osd/gl/buffers.h"
#include "osd/gl/shader.h"
#include "engine/player.h"
#include "universe/frame.h"
#include "universe/star.h"
#include "universe/system.h"
#include "universe/universe.h"
#include "render/scene.h"
#include "render/vobject.h"

void Scene::initConstellations(Universe &universe)
{
    const Constellations &constellations = universe.getConstellations();
    const StarDatabase &starlib = universe.getStarDatabase();
    const std::vector<Asterism *> &asterisms = constellations.getAsterisms();

    pgmAsterism = ctx.getShaderManager()->createShader("line");

    pgmAsterism->use();

    vbufAsterism = new VertexBuffer(ctx, 1);
    vbufAsterism->createBuffer(VertexBuffer::VBO, 1);

    for (int idx = 0; idx < asterisms.size(); idx++)
    {
        Asterism *aster = asterisms[idx];
        asterismLines += aster->hipList.size();
    }

    // std::cout << "Total " << asterismLines << " asterism lines\n" << std::flush;
    Logger::getLogger()->info("Total {} asterism lines\n", asterismLines);

    pgmAsterism->release();
}

void Scene::renderConstellations(Universe &universe, const Player &player)
{
    const Constellations &constellations = universe.getConstellations();
    const StarDatabase &starlib = universe.getStarDatabase();
    const std::vector<Asterism *> &asterisms = constellations.getAsterisms();

    Camera *cam = player.getCamera();
    vec3d_t cpos = cam->getuPosition();
    int cLines = 0;

    VertexLine *vertices = nullptr;
    uint32_t vbo = vbufAsterism->getVBO();

    pgmAsterism->use();
    vbufAsterism->bind();

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, asterismLines * sizeof(VertexLine), nullptr, GL_STREAM_DRAW);
    vertices = reinterpret_cast<VertexLine *>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
    if (vertices == nullptr)
    {
        Logger::getLogger()->fatal("Can't render constellations - aborted (error code: {})\n",
            glGetError());
        vbufAsterism->release();
        pgmAsterism->release();
        return;
    }

    int rLines = 0;
    for (int idx = 0; idx < asterisms.size(); idx++)
    {
        Asterism *aster = asterisms[idx];
        for (int sidx = 0; sidx < aster->hipList.size(); sidx += 2)
        {
            const celStar *star1 = starlib.getHIPstar(aster->hipList[sidx]);
            const celStar *star2 = starlib.getHIPstar(aster->hipList[sidx+1]);

            if (star1 == nullptr)
                Logger::getLogger()->warn("HIP {} not found in catalogue\n", aster->hipList[sidx]);
            if (star2 == nullptr)
                Logger::getLogger()->warn("HIP {} not found in catalogue\n", aster->hipList[sidx+1]);
            if (star1 == nullptr || star2 == nullptr)
                continue;

            vertices[rLines].spos = vec3f_t((star1->getStarPosition() * KM_PER_PC) - cpos);
            vertices[rLines].color = color_t(0.5, 0.5, 0.5, 1.0);
            rLines++;
            vertices[rLines].spos = vec3f_t((star2->getStarPosition() * KM_PER_PC) - cpos);
            vertices[rLines].color = color_t(0.5, 0.5, 0.5, 1.0);
            rLines++;
        }
    }

    if (!glUnmapBuffer(GL_ARRAY_BUFFER))
    {
        Logger::getLogger()->fatal("Data buffer corruption - aborted (error code: {})\n", glGetError());
        vbufAsterism->release();
        pgmAsterism->release();
        return;
    }

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLine), (void *)0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexLine), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    mat4f_t mvp = mat4f_t(prm.dmProj * prm.dmView);

    uint32_t mvpLoc = glGetUniformLocation(pgmAsterism->getID(), "mvp");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

    glDrawArrays(GL_LINES, 0, rLines);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    vbufAsterism->release();
    pgmAsterism->release();
}