// mesh.cpp - Mesh Package for spacecrafts
//
// Author:  Tim Stark
// Date:    Nov 5, 2023

#include "main/core.h"
#include "main/app.h"
#include "api/graphics.h"
#include "utils/yaml.h"
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
    const float eps = 1e-8f;
    uint16_t *idx = group->idx;
    MeshVertex *vtx = group->vtx;

    for (int i = 0; i < group->nvtx; i++)
        vtx[i].nml.x = vtx[i].nml.y = vtx[i].nml.z = 0.0f;

    for (int i = 0; i < group->nidx; i += 3)
    {
        int i0 = idx[i+0], i1 = idx[i+1], i2 = idx[i+2];
        glm::vec3 v01 = { vtx[i1].vtx.x - vtx[i0].vtx.x, vtx[i1].vtx.y - vtx[i0].vtx.y, vtx[i1].vtx.z - vtx[i0].vtx.z };
        glm::vec3 v02 = { vtx[i2].vtx.x - vtx[i0].vtx.x, vtx[i2].vtx.y - vtx[i0].vtx.y, vtx[i2].vtx.z - vtx[i0].vtx.z };
        glm::vec3 v12 = { vtx[i2].vtx.x - vtx[i1].vtx.x, vtx[i2].vtx.y - vtx[i1].vtx.y, vtx[i2].vtx.z - vtx[i1].vtx.z };
        glm::vec3 nm = glm::cross(v01, v02);
        float len = glm::length(nm);

        if (len > eps)
        {
            nm.x /= len, nm.y /= len, nm.z /= len;
            float d01 = glm::length(v01);
            float d02 = glm::length(v02);
            float d12 = glm::length(v12);

            float a0 = acos(d01 * d01 + d02 * d02 - d12 * d12) / (2.0f * d01 * d02);
            vtx[i0].nml.x += nm.x * a0, vtx[i0].nml.y += nm.y * a0, vtx[i0].nml.z += nm.z * a0;

            float a1 = acos(d01 * d01 + d12 * d12 - d02 * d02) / (2.0f * d01 * d12);
            vtx[i1].nml.x += nm.x * a1, vtx[i1].nml.y += nm.y * a1, vtx[i1].nml.z += nm.z * a1;
            
            float a2 = acos(d02 * d02 + d12 * d12 - d01 * d01) / (2.0f * d02 * d12);
            vtx[i2].nml.x += nm.x * a2, vtx[i2].nml.y += nm.y * a2, vtx[i2].nml.z += nm.z * a2;

        }
    }

    for (int i = 0; i < group->nvtx; i++)
        vtx[i].nml /= glm::length(vtx[i].nml);
}

void Mesh::addGroup(MeshGroup *group)
{
    groups.push_back(group);
}

void Mesh::addMaterial(MeshMaterial *mtrl)
{
    materials.push_back(mtrl);
}

void Mesh::load(YAML::Node &config, Mesh &mesh)
{
    int nGroups;
    bool bStaticMesh;

    bStaticMesh = yaml::getValue(config, "StaticMesh", false);

    if (!config["MeshData"].IsSequence())
        return;
    YAML::Node data = config["MeshData"];
    nGroups = data.size();

    for (int gidx = 0; gidx < nGroups; gidx++)
    {
        YAML::Node entry = data["MeshData"][gidx];

        bool bNormals = false;
        bool bCalcNormals = false;
        int mtrlIndex = -1;
        int texIndex = -1;
        int nvtx, nidx, ntri;
        MeshVertex *vtx = nullptr;
        uint16_t *idx = nullptr;

        uint32_t flags = bStaticMesh ? 4 : 0;
        uint32_t userFlags = 0;

        mtrlIndex = yaml::getValue<int>(entry, "material", -1);
        texIndex = yaml::getValue<int>(entry,"texture", -1);
        bNormals = yaml::getValue<bool>(entry, "Normals", false);
        userFlags = yaml::getValue<uint32_t>(entry, "Flags", 0);

        str_t wrap = yaml::getValueString<str_t>(entry, "TexWrap");
        if (!wrap.empty()) {
            if (toupper(wrap[0]) == 'U' || toupper(wrap[1]) == 'U')
                flags |= 1;
            if (toupper(wrap[0]) == 'V' || toupper(wrap[1]) == 'V')
                flags |= 2;
        }

        if (!entry["vertices"].IsSequence())
            return;
        nvtx = entry["vertices"].size();
        vtx = new MeshVertex[nvtx];
        memset(vtx, 0, sizeof(MeshVertex)*nvtx);
        for (int vidx = 0; vidx < nvtx; vidx++)
        {
            YAML::Node geom = entry["vertices"][vidx];
            if (!geom.IsSequence())
                return;
            MeshVertex &v = vtx[vidx];
            if (bNormals) {
                v.vtx.x = geom[0].as<double>(), v.vtx.y = geom[1].as<double>(), v.vtx.z = geom[2].as<double>();
                v.nml.x = geom[3].as<double>(), v.nml.y = geom[4].as<double>(), v.nml.z = geom[5].as<double>();
                v.tc.x  = geom[6].as<double>(), v.tc.y  = geom[7].as<double>();
            } else {
                v.vtx.x = geom[0].as<double>(), v.vtx.y = geom[1].as<double>(), v.vtx.z = geom[2].as<double>();
                v.tc.x  = geom[3].as<double>(), v.tc.y  = geom[4].as<double>();
                bCalcNormals = true;
            }
        }

        if (!entry["indices"].IsSequence())
            return;
        ntri = entry["indices"].size(), nidx = ntri * 3;
        idx = new uint16_t[nidx];
        memset(vtx, 0, sizeof(MeshVertex)*nvtx);
        for (int eidx = 0, jidx = 0; eidx < ntri; eidx++, jidx += 3)
        {
            YAML::Node value = entry["indices"][eidx];
            if (!value.IsSequence())
                return;
            idx[jidx+0] = value[0].as<double>();
            idx[jidx+1] = value[1].as<double>();
            idx[jidx+2] = value[2].as<double>();
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

            group->Flags = flags;
            group->userFlags = userFlags;
            group->texIndex = texIndex;
            group->mtrlIndex = mtrlIndex;

            if (bCalcNormals)
                mesh.calculateNormals(group, true);
            mesh.addGroup(group);
        }
    }

    if (!config["Materials"].IsSequence())
        return;
    YAML::Node mlist = config["Materials"];
    for (int midx = 0; midx < mlist.size(); midx++)
    {
        MeshMaterial *mtrl = new MeshMaterial;

        YAML::Node entry = mlist[midx];
        mtrl->name = yaml::getValueString<str_t>(entry, "Name");
        mtrl->diffuse = yaml::getArray<glm::vec4, float>(entry, "Diffuse", { 0.0, 0.0, 0.0, 0.0 });
        mtrl->specular = yaml::getArray<glm::vec4, float>(entry, "Specular", { 0.0, 0.0, 0.0, 0.0 });
        mtrl->ambient = yaml::getArray<glm::vec4, float>(entry, "Ambient", { 0.0, 0.0, 0.0, 0.0 });
        mtrl->emissive = yaml::getArray<glm::vec4, float>(entry, "Emissive", { 0.0, 0.0, 0.0, 0.0 });
        mtrl->power = yaml::getValue<float>(entry, "Power", 0.0);

        addMaterial(mtrl);
    }

    if (!config["Textures"].IsSequence())
        return;
    YAML::Node tlist = config["Textures"];
    GraphicsClient *gclient = ofsAppCore->getClient();
    for (int tidx = 0; tidx < tlist.size(); tidx++)
    {
        YAML::Node entry = tlist[tidx];
        Texture *txImage;
        str_t texName = yaml::getValueString<str_t>(entry, "texname");
        bool uncompress = yaml::getValue<bool>(entry, "uncompress", false);
        int flags = uncompress ? 2 : 0;
        if (gclient != nullptr)
        {
            txImage = gclient->loadTexture(texName, flags | 8);
            txImages.push_back(txImage);
        }
    }

    mesh.setup();
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
                        int j = sscanf(cbuf, "%f%f%f%f%f%f%f%f",
                            &v.vtx.x, &v.vtx.y, &v.vtx.z, &v.nml.x, &v.nml.y, &v.nml.z, &v.tc.x, &v.tc.y);
                        if (j < 6)
                            bCalcNormal = true;
                    }
                    else
                    {
                        int j = sscanf(cbuf, "%f%f%f%f%f",
                            &v.vtx.x, &v.vtx.y, &v.vtx.z, &v.tc.x, &v.tc.y);
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
        MeshMaterial *mtrl = nullptr;
        std::string *names = new std::string[nMaterials];
        int res;

        for (int idx = 0; idx < nMaterials; idx++)
        {
            is.getline(cbuf, sizeof(cbuf));
            names[idx] = std::string(sizeof(cbuf)+1, '\0');
            sscanf(cbuf, "%s", names[idx].data());
        }
        for (int idx = 0; idx < nMaterials; idx++)
        {
            mtrl = new MeshMaterial;
            memset((void *)mtrl, 0, sizeof(MeshMaterial));
            is.getline(cbuf, sizeof(cbuf));
            sscanf(cbuf+8, "%s", mtrl->name.data());
            is.getline(cbuf, sizeof(cbuf));
            sscanf(cbuf, "%f%f%f%f", &mtrl->diffuse.r, &mtrl->diffuse.g, &mtrl->diffuse.b, &mtrl->diffuse.a);
            is.getline(cbuf, sizeof(cbuf));
            sscanf(cbuf, "%f%f%f%f", &mtrl->ambient.r, &mtrl->ambient.g, &mtrl->ambient.b, &mtrl->ambient.a);
            is.getline(cbuf, sizeof(cbuf));
            sscanf(cbuf, "%f%f%f%f%f", &mtrl->specular.r, &mtrl->specular.g, &mtrl->specular.b, &mtrl->specular.a,
                &mtrl->power);
            is.getline(cbuf, sizeof(cbuf));
            sscanf(cbuf, "%f%f%f%f", &mtrl->emissive.r, &mtrl->emissive.g, &mtrl->emissive.b, &mtrl->emissive.a);

            mesh.addMaterial(mtrl);
        }

        delete [] names;
    }

    // Read texture list from file
    if (is.getline(cbuf, sizeof(cbuf)) && !strncmp(cbuf, "TEXTURES", 9) && (sscanf(cbuf+9, "%d", &nTextures) == 1))
    {
        std::string texName(256, '\0'), flagStr;
        bool uncompress = false;
        GraphicsClient *gclient = ofsAppCore->getClient();

        for (int idx = 0; idx < nTextures; idx++)
        {
            is.getline(cbuf, sizeof(cbuf));
            sscanf(cbuf, "%s%s", texName.data(), flagStr.data());
            if (!texName.empty())
                uncompress = toupper(flagStr[0]) == 'D';
            int flags = uncompress ? 2 : 0;
            if (gclient != nullptr)
            {
                Texture *txImage = gclient->loadTexture(texName, flags | 8);
                mesh.txImages.push_back(txImage);
            }
        }
    }

    mesh.setup();

    is.clear();
    return is;
}

// ******** MeshManager ********

MeshManager::~MeshManager()
{
    cleanup();
}

void MeshManager::cleanup()
{
    for (auto &mesh : meshList)
        if (mesh != nullptr)
            delete mesh;
    meshList.clear();
}

const Mesh *MeshManager::loadMesh(cstr_t &fname)
{
    using namespace std;

    ifstream ifs(fname, ios::in);
    Mesh *mesh = new Mesh;
    ifs >> *mesh;

    meshList.push_back(mesh);

    return mesh;
}