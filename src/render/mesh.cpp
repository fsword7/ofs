// mesh.cpp - Mesh 3D package
//
// Author:  Tim Stark
// Date:    Apr 28, 2022

#include "main/core.h"
#include "render/scene.h"
#include "render/mesh.h"

void Mesh3D::setup()
{

}

void Mesh3D::clear()
{

}

MeshGroup *Mesh3D::addGroup(vtxf_t *vtx, int nvtx, uint32_t *idx, int nidx, int midx, int tidx, int zbias)
{
    MeshGroup *grp = new MeshGroup;

    grp->vtx = vtx;
    grp->idx = idx;
    grp->nvtx = nvtx;
    grp->nidx = nidx;

    grp->mIndex = midx;
    grp->tIndex = tidx;
    grp->zBias = zbias;
    grp->flags = 0;
    grp->uFlags = 0;

    groups.push_back(grp);

    return grp;
}

std::istream &operator >> (std::istream &is, Mesh3D &mesh)
{
    char cbuf[256];

    int ngrp, gidx;
    bool bStaticMesh = false;
    // bool stop = false;

    // Clear all old mesh data
    mesh.clear();

    if (!is.getline(cbuf, 256))
        return is;
    if (strcmp(cbuf, "MSHX1"))
        return is;
    
    for (;;)
    {
        if (!is.getline(cbuf, 256))
            return is;
        if (!strncmp(cbuf, "GROUPS", 6))
        {
            if (sscanf(cbuf+6, "%d", &ngrp) != 1)
                return is;
            break;
        }
        else if (!strncmp(cbuf, "STATICMESH", 10))
            bStaticMesh = true;
    }

    for (int gidx = 0, stop = false; gidx < ngrp && !stop; gidx++)
    {
        int midx, tidx;
        int zbias;
        int flags = 0;
        int uflags = 0;
        int nvtx, nidx, ntri;
        bool bNormal = true;
        bool calcNormal = false;
        bool flipIdx = false;
        vtxf_t *vtx;
        uint32_t *vidx;

        for (;;)
        {
            if (!is.getline(cbuf, 256))
            {
                stop = true;
                break;
            }

            if (strncmp(cbuf, "MATERIAL", 8))
            {
                sscanf(cbuf+8, "%d", &midx);
                midx--;
            }
            else if (!strncmp(cbuf, "TEXTURE", 7))
            {
                sscanf(cbuf+7, "%d", &tidx);
                tidx--;
            }
            else if (!strncmp(cbuf, "ZBIAS", 5))
                sscanf(cbuf+5, "%hu", &zbias);
            else if (!strncmp(cbuf, "TEXWRAP", 7))
            {
                char uvstr[10] = "";
                sscanf(cbuf+7, "%9s", uvstr);
                if (uvstr[0] == 'U' || uvstr[1] == 'U')
                    flags |= 0x01;
                if (uvstr[0] == 'V' || uvstr[1] == 'V')
                    flags |= 0x02;
            }
            else if (!strncmp(cbuf, "NONORMAL", 8))
            {
                bNormal = false;
                calcNormal = true;
            }
            else if (!strncmp(cbuf, "FLAG", 4))
                sscanf(cbuf+4, "%lx", &uflags);
            else if(!strncmp(cbuf, "FLIP", 4))
                flipIdx = true;
            else if(!strncmp(cbuf, "LABEL", 5))
            {
                // Do nothing
            }
            else if (!strncmp(cbuf, "STATIC", 6))
                flags |= 0x04;
            else if (!strncmp(cbuf, "DYNAMIC", 7))
                flags ^= 0x04;
            else if (!strncmp(cbuf, "GEOM", 4))
            {
                if (sscanf(cbuf+4, "%d%d", &nvtx, &ntri) != 2)
                    break;
                nidx = ntri * 3;
                vtx = new vtxf_t[nvtx];
                int n;

                for (int idx = 0; idx < nvtx; idx++)
                {
                    vtxf_t &v = vtx[idx];
                    if (!is.getline(cbuf, 256))
                    {
                        delete [] vtx;
                        nvtx = 0;
                        break;
                    }
                    if (bNormal)
                    {
                        n = sscanf(cbuf, "%f%f%f%f%f%f%f%f",
                            &v.vx, &v.vy, &v.vz, &v.nx, &v.ny, &v.nz, &v.tu, &v.tv);
                        if (n < 6)
                            calcNormal = true;
                    }
                    else
                    {
                        n = sscanf(cbuf, "%f%f%f%f%f",
                            &v.vx, &v.vy, &v.vz, &v.tu, &v.tv);
                    }
                }

                vidx = new uint32_t[nidx];
                for (int idx = 0, j = 0; idx < ntri; idx++)
                {
                    if (!is.getline(cbuf, 256))
                    {
                        delete [] vtx;
                        delete [] vidx;
                        nvtx = nidx = 0;
                        break;
                    }
                    sscanf(cbuf, "%d%d%d", vidx+j, vidx+j+1, vidx+j+2);
                    j += 3;
                }
                if (flipIdx)
                {
                    for (int idx = 0; idx < ntri; idx++)
                    {
                        int tmp = vidx[idx*3+1];
                        vidx[idx*3+1] = vidx[idx*3+2];
                        vidx[idx*3+2] = tmp;
                    }
                }

                break;
            }
        }

        if (nvtx != 0 && nidx != 0)
        {
            MeshGroup *grp = mesh.addGroup(vtx, nvtx, vidx, nidx, midx, tidx, zbias);
            grp->flags = flags;
            grp->uFlags = uflags;

            // if (calcNormal)
            //     mesh.calculateNormals(grp);
        }
    }

    // Read material list

    // Read texture list

    // mesh.setup();
    is.clear();
    return is;
}