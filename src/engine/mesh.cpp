// mesh.cpp - Mesh Package for spacecrafts
//
// Author:  Tim Stark
// Date:    Nov 5, 2023

#include "main/core.h"
#include "engine/mesh.h"

Mesh::~Mesh()
{
    clear();
}

void Mesh::clear()
{
    for (auto group : groups)
    {
        if (group->vtx != nullptr)
            delete [] group->vtx;
        if (group->idx != nullptr)
            delete [] group->idx;
        delete group;
    }
    groups.clear();
}

void Mesh::setup()
{

}

void Mesh::calculateNormals(MeshGroup *group, bool missing)
{

}

void Mesh::addGroup(MeshGroup *group)
{
    groups.push_back(group);
}



std::istream &operator >> (std::istream &is, Mesh &mesh)
{
    char cbuf[256];
    int nGroups;
    int nMaterials;
    int nTextures;
    bool bStaticMesh = false;

    mesh.clear();

    if (!is.getline(cbuf, sizeof(cbuf)))
        return is;
    if (cbuf[strlen(cbuf)] == '\r')
        cbuf[strlen(cbuf)] = '\0';
    if (strncmp(cbuf, "MSHX1", 5))
        return is;

    for (;;)
    {
        if (!is.getline(cbuf, sizeof(cbuf)))
            return is;
        if (!strncmp(cbuf, "GROUPS", 6))
        {
            if (sscanf(cbuf+6, "%d", &nGroups) != 1)
                return is;
            break;
        }
        if (!strncmp(cbuf, "STATICMESH", 10))
            bStaticMesh = true;
    }

    // Read group list from file
    for (int gidx = 0; gidx < nGroups; gidx++)
    {
        uint32_t Flags = bStaticMesh ? 4 : 0;
        uint32_t userFlags = 0;
        bool bFlip = false;
        int texIndex = -1;
        int mtrlIndex = -1;
        bool bNormal = true;
        bool bCalcNormal = false;
        int zBias = 0;

        int nvtx = 0;
        int nidx = 0;
        int ntri = 0;
        MeshVertex *vtx = nullptr;
        uint16_t *idx = nullptr;

        for (;;)
        {
            if (!is.getline(cbuf, sizeof(cbuf)))
                return is;

            if (!strncmp(cbuf, "LABEL", 5))
            {
                // Ignore group labels
            }
            else if (!strncmp(cbuf, "TEXWRAP", 7))
            {
                char uvStr[10] = "";
                sscanf(cbuf+7, "%9s", uvStr);
                if (uvStr[0] == 'U' || uvStr[1] == 'U')
                    Flags |= 1;
                if (uvStr[0] == 'V' || uvStr[1] == 'V')
                    Flags |= 2;
            }
            else if (!strncmp(cbuf, "MATERIAL", 8))
            {
                sscanf(cbuf+8, "%d", &mtrlIndex);
                mtrlIndex--;
            }
            else if (!strncmp(cbuf, "TEXTURE", 7))
            {
                sscanf(cbuf+7, "%d", &texIndex);
                texIndex--;
            }
            else if (!strncmp(cbuf, "FLAG", 4))
            {
                sscanf(cbuf+4, "%d", &userFlags);
            }
            else if (!strncmp(cbuf, "ZBIAS", 5))
            {
                sscanf(cbuf+5, "%u", &zBias);
            }
            else if (!strncmp(cbuf, "STATIC", 6))
            {
                Flags |= 4;
            }
            else if (!strncmp(cbuf, "DYNAMIC", 7))
            {
                Flags ^= 4;
            }
            else if (!strncmp(cbuf, "NONORMAL", 8))
            {
                bNormal = false;
                bCalcNormal = true;
            }
            else if (!strncmp(cbuf, "FLIP", 4))
            {
                bFlip = true;
            }
            else if (!strncmp(cbuf, "GEOM", 4))
            {
                if (sscanf(cbuf+4, "%d%d", &nvtx, &ntri) != 2)
                    break;
                nidx = ntri * 3;
                
                vtx = new MeshVertex[nvtx];
                memset(vtx, 0, sizeof(MeshVertex)*nvtx);
                for (int vidx = 0; vidx < nvtx; vidx++)
                {
                    MeshVertex &v = vtx[vidx];
                    if (!is.getline(cbuf, sizeof(cbuf)))
                    {
                        delete [] vtx;
                        nvtx = 0;
                        break;
                    }
                    if (bNormal)
                    {
                        int j = sscanf(cbuf, "%lf%lf%lf%lf%lf%lf%lf%lf",
                            &v.x, &v.y, &v.z, &v.nx, &v.ny, &v.nz, &v.tu, &v.tv);
                        if (j < 6)
                            bCalcNormal = true;
                    }
                    else
                    {
                        int j = sscanf(cbuf, "%lf%lf%lf%lf%lf",
                            &v.x, &v.y, &v.z, &v.tu, &v.tv);
                    }
                }

                idx = new uint16_t[nidx];
                memset(idx, 0, sizeof(uint16_t)*nidx);
                int jidx = 0;
                for (int iidx = 0; iidx < nidx; iidx++)
                {
                    if (!is.getline(cbuf, sizeof(cbuf)))
                    {
                        delete [] vtx;
                        delete [] idx;
                        nvtx = 0;
                        nidx = 0;
                        break;
                    }
                    int j = sscanf(cbuf, "%hd%hd%hd",
                        idx+jidx+0, idx+jidx+1, idx+jidx+2);
                    jidx += 3;
                }

                if (bFlip)
                {
                    for (int i = 0; i < ntri; i++)
                    {
                        uint16_t tmp = idx[i*3+1];
                        idx[i*3+1] = idx[i*3+2];
                        idx[i*3+2] = tmp;
                    }
                }

                break;
            }
        }

        if (nvtx > 0 && nidx > 0)
        {
            MeshGroup *group = new MeshGroup;

            // Assign vertices to group
            group->nvtx = nvtx;
            group->vtx = vtx;
            // Assign indices to group
            group->nidx = nidx;
            group->idx = idx;

            group->Flags = Flags;
            group->userFlags = userFlags;
            group->texIndex = texIndex;
            group->mtrlIndex = mtrlIndex;

            if (bCalcNormal)
                mesh.calculateNormals(group, true);
            mesh.addGroup(group);
        }
    }

    // Read material list from file
    if (is.getline(cbuf, sizeof(cbuf)) && !strncmp(cbuf, "MATERIALS", 9) && (sscanf(cbuf+9, "%d", &nMaterials) == 1))
    {
        MeshMaterial mtrl;
        std::string *names = new std::string[nMaterials];
        int res;

        for (int idx = 0; idx < nMaterials; idx++)
        {
            is.getline(cbuf, sizeof(cbuf));
            sscanf(cbuf, "%s", names[idx]);
        }
        for (int idx = 0; idx < nMaterials; idx++)
        {
            memset(&mtrl, 0, sizeof(MeshMaterial));
            is.getline(cbuf, sizeof(cbuf));
            sscanf(cbuf+8, "%s", mtrl.name);
            is.getline(cbuf, sizeof(cbuf));
            sscanf(cbuf, "%f%f%f%f", &mtrl.diffuse.r, &mtrl.diffuse.g, &mtrl.diffuse.b, &mtrl.diffuse.a);
            is.getline(cbuf, sizeof(cbuf));
            sscanf(cbuf, "%f%f%f%f", &mtrl.ambient.r, &mtrl.ambient.g, &mtrl.ambient.b, &mtrl.ambient.a);
            is.getline(cbuf, sizeof(cbuf));
            sscanf(cbuf, "%f%f%f%f%f", &mtrl.specular.r, &mtrl.specular.g, &mtrl.specular.b, &mtrl.specular.a,
                &mtrl.power);
            is.getline(cbuf, sizeof(cbuf));
            sscanf(cbuf, "%f%f%f%f", &mtrl.emissive.r, &mtrl.emissive.g, &mtrl.emissive.b, &mtrl.emissive.a);

            // mesh.addMaterial(mtrl);
        }

        delete [] names;
    }

    // Read texture list from file
    if (is.getline(cbuf, sizeof(cbuf)) && !strncmp(cbuf, "TEXTURES", 9) && (sscanf(cbuf+9, "%d", &nTextures) == 1))
    {
        std::string texName, flagStr;
        bool uncompress = false;

        for (int idx = 0; idx < nTextures; idx++)
        {
            is.getline(cbuf, sizeof(cbuf));
            sscanf(cbuf, "%s%s", texName, flagStr);
            if (!texName.empty())
                uncompress = toupper(flagStr[0]) == 'D';

        }
    }

    mesh.setup();

    is.clear();
    return is;
}