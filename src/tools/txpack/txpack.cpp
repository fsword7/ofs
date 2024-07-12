// texpack.cpp - Texture Archiver Package
//
// Author:  Tim Stark
// Date:    Apr 23, 2024

#include <main/core.h>
#include <getopt.h>
#include <zlib.h>

#define MAKEFOURCC(ch0, ch1, ch2, ch3)                          \
    (static_cast<uint32_t>(static_cast<uint8_t>(ch0)) |         \
     static_cast<uint32_t>(static_cast<uint8_t>(ch1)) << 8 |    \
     static_cast<uint32_t>(static_cast<uint8_t>(ch2)) << 16 |   \
     static_cast<uint32_t>(static_cast<uint8_t>(ch3)) << 24 )

#define TREE_DEFLATE        0x0000'0001

struct zTreeHeader
{
    uint32_t magic;     // magic code
    uint32_t type;      // layer name
    uint32_t size;      // header length
    uint32_t flags;     // flags
    uint32_t ntoc;      // number of TOC entries
    uint32_t dataofs;   // data offset from the beginning
    size_t   total;     // total bytes
    uint8_t  ext[4];    // extension name

    uint32_t rootg[3];  // Array indices of global tiles
    uint32_t rootp[2];  // Array indices of LOD tiles (quadtree roots)

};

struct zNode
{
    zNode(int lod, int ilat, int ilng)
    : lod(lod), ilat(ilat), ilng(ilng)
    { }

    int lod;
    int ilat, ilng;
    zNode *child[4] = {};

    fs::path rpath;   // relative path from curreent working directory
    fs::path lpath;   // local path from root path
};

struct tocEntry
{
    size_t   pos  = 0;   // file position from end of TOC table
    uint32_t size = 0;  // uncompressed size
    uint32_t lod  = 0;
    uint32_t ilat = 0;
    uint32_t ilng = 0;
    int32_t  child[4] = { -1, -1, -1, -1 };
                        // array position of the children ( -1 = no child )
};

enum ofsMode { Archive, Extract, List } mode = Archive;

class zTree
{
public:
    zTree(cstr_t &root, cstr_t &layer, ofsMode mode);
    ~zTree();

    bool addLevel(int lod);
    void addLevels(int minlod, int maxlod);

    zNode *insertNode(int lod, int ilat, int ilng);
    void deleteNodes(zNode *node);
    zNode *findNode(int lod, int ilat, int ilng);
    void countNodes(zNode *node, int &count);
    int countNodes();

    void setupTreeDBr();
    void setupTreeDBw();
    void writeTreeDB();
    void extractTreeDB(int maxLOD);
    void listTreeDB(int maxLOD);
    int addTree(zNode *node);
    int writeTree(std::fstream &tree, zNode *node);
    void extractTree(std::fstream &ifile, int idx, int lod, int maxLOD);
    void listTree(std::fstream &ifile, int idx, int lod, int maxLOD);

    uint32_t deflateData(uint8_t *data, uint32_t dsize, uint8_t *zdata, uint32_t zsize);
    uint32_t inflateData(uint8_t *zdata, uint32_t zsize, uint8_t *data, uint32_t dsize);

private:
    zNode *rootg[3] = {};
    zNode *root[6] = {};
    str_t pathName;
    str_t layerName;
    str_t extName;
    int   base;     // 0 or 1

    fs::path rootPath;

    zTreeHeader thdr;

    tocEntry *toc = nullptr;

    int zsize = 1'024'000;
    uint8_t zdata[1'024'000];

    const int patLevels[9] = { 0, 1, 2, 3, 5, 13, 37, 137, 501 };
};

zTree::zTree(cstr_t &root, cstr_t &layer, ofsMode mode)
: rootPath(root)
{

    int pos = layer.find(":");
    layerName = layer.substr(0, pos);
    extName = "." + layer.substr(pos+1);
    pathName = fmt::format("{}/{}", root, layerName);

    std::cout << fmt::format("Layer: {}  Ext: {}\n", layerName, extName);
    std::cout << fmt::format("Root: {}\n", pathName);
}

zTree::~zTree()
{
    for (int idx = 0; idx < 6; idx++)
        deleteNodes(root[idx]);
    if (toc != nullptr)
        delete [] toc;
}

zNode *zTree::findNode(int lod, int ilat, int ilng)
{
    if (lod < 4)
        return rootg[lod];
    if (lod == 4)
        return root[ilng];
    zNode *pnode = findNode(lod-1, ilat/2, ilng/2);
    if (pnode == nullptr)
        return 0;
    return pnode->child[((ilat & 1) << 1) | (ilng & 1)];
}

zNode *zTree::insertNode(int lod, int ilat, int ilng)
{
    if (lod < 4)
        return nullptr;

    zNode *node = new zNode(lod, ilat, ilng);
    if (lod == 4)
        root[ilng] = node;
    else {
        zNode *pnode = findNode(lod-1, ilat/2, ilng/2);
        if (pnode == nullptr)
            pnode = insertNode(lod-1, ilat/2, ilng/2);
        pnode->child[((ilat & 1) << 1) | (ilng & 1)] = node;
    }
    return node;
}

void zTree::deleteNodes(zNode *node)
{
    if (node == nullptr)
        return;
    for (int idx = 0; idx < 4; idx++)
        deleteNodes(node->child[idx]);
    delete node;
}

void zTree::countNodes(zNode *node, int &count)
{
    if (node == nullptr)
        return;
    count++;
    for (int idx = 0; idx < 4; idx++)
        countNodes(node->child[idx], count);
}

int zTree::countNodes()
{
    int count = 0;
    for (int idx = 0; idx < 6; idx++)
        countNodes(root[idx], count);
    return count;
}

bool zTree::addLevel(int lod)
{
    str_t rpath = fmt::format("{}/{:02d}", pathName, lod);      // relative path from current working directory
    str_t lpath = fmt::format("{}/{:02d}", layerName, lod);     // local path from root path

    if (!fs::exists(rpath) || !fs::is_directory(rpath))
        return false;
    std::cout << fmt::format("Path: {}\n", lpath);

    for(auto &dir : fs::directory_iterator(rpath))
    {
        int nlat = 1 << lod;
        int nlng = 2 << lod;
        int ilat, ilng;

        if (!fs::is_directory(dir))
            continue;
        str_t pdirName = dir.path().string();
        str_t dirName = dir.path().filename().string();

        // Check directory name validation
        if (dirName.size() != 6)
            continue;
        for (auto ch : dirName)
            if (!isdigit(ch))
                continue;

        int pos = pdirName.find(layerName);
        std::cout << fmt::format("Path: {}\n", pdirName.substr(pos));
        ilat = atoi(dirName.c_str());

        for (auto &file : fs::directory_iterator(dir))
        {
            std::cout << fmt::format("Validating {}\n", file.path().string());

            if (!fs::is_regular_file(file))
                continue;

            // Check file name validation
            if (file.path().extension() != extName)
                continue;           
            str_t stemName = file.path().stem().string();
            if (stemName.size() != 6)
                continue;
            for (auto ch : stemName)
                if (!isdigit(ch))
                    continue;
            ilng = atoi(stemName.c_str());

            str_t fileName = file.path().string();
            int pos = fileName.find(layerName);
            std::cout << fmt::format("File: {}\n", fileName.substr(pos));

            // Add node to tree data
            zNode *node = insertNode(lod, ilat, ilng);
            node->rpath = file.path();
            node->lpath = fileName.substr(pos);
        }

        std::cout << std::endl;
    }

    return true;
}

void zTree::addLevels(int minlod, int maxlod)
{
    for (int lod = minlod; lod <= maxlod; lod++)
        if (!addLevel(lod))
            break;

    std::cout << fmt::format("Total nodes = {}\n", countNodes());
}

uint32_t zTree::deflateData(uint8_t *data, uint32_t dsize, uint8_t *zdata, uint32_t zsize)
{
    z_stream zstrm;
    zstrm.zalloc = Z_NULL;
    zstrm.zfree = Z_NULL;
    zstrm.opaque = Z_NULL;
    int ret, flush;

    ret = deflateInit(&zstrm, Z_DEFAULT_COMPRESSION);

    zstrm.next_in = data;
    zstrm.avail_in = dsize;
    zstrm.next_out = zdata;
    zstrm.avail_out = zsize;
    flush = Z_FINISH;

    ret = deflate(&zstrm, flush);
    if (ret != Z_STREAM_END)
        exit(1);
    if (zstrm.avail_in > 0)
        exit(1);

    deflateEnd(&zstrm);
    return zstrm.total_out;
}

uint32_t zTree::inflateData(uint8_t *zdata, uint32_t zsize, uint8_t *data, uint32_t dsize)
{
    unsigned long ndata = dsize;
    if (uncompress(data, &ndata, zdata, zsize) == Z_OK)
        return ndata;
    return 0;
}

void zTree::setupTreeDBw()
{
    // Initialize tree header
    thdr.magic = MAKEFOURCC('O', 'F', 'S', '0');
    if (layerName == "surf")
        thdr.type = MAKEFOURCC('S', 'U', 'R', 'F');
    else if (layerName == "mask")
        thdr.type = MAKEFOURCC('M', 'A', 'S', 'K');
    else if (layerName == "clouds")
        thdr.type = MAKEFOURCC('C', 'L', 'O', 'U');
    else if (layerName == "elev")
        thdr.type = MAKEFOURCC('E', 'L', 'E', 'V');
    else if (layerName == "elev_mod")
        thdr.type = MAKEFOURCC('E', 'L', 'V', 'M');
    else if (layerName == "label")
        thdr.type = MAKEFOURCC('L', 'A', 'B', 'E');

    // Set extension name (first 4 letters)
    int esize = std::min((int)extName.size(), 4);
    int cidx = 0;
    for (cidx = 0; cidx < esize; cidx++)
        thdr.ext[cidx] = extName[cidx+1];
    for (;cidx < 4; cidx++)
        thdr.ext[cidx] = 0;

    thdr.size  = sizeof(zTreeHeader);
    thdr.flags = TREE_DEFLATE;
    thdr.ntoc  = 0;
    thdr.total = 0;
    base = 0;

    int ntoc = countNodes();
    thdr.dataofs = thdr.size + ntoc*sizeof(tocEntry);
    toc = new tocEntry[ntoc];
}

void zTree::setupTreeDBr()
{

}

void zTree::writeTreeDB()
{
    fs::path treePath = rootPath / "ofstree";

    if (!fs::exists(treePath))
        fs::create_directory(treePath);
    if (!fs::is_directory(treePath))
        return;
    fs::path treeName = treePath / (layerName + ".tree");

    std::cout << fmt::format("Opening {}\n", treeName.string());

    std::fstream otree(treeName, std::ios::binary|std::ios::out);
    if (otree.fail())
    {
        std::cout << fmt::format("{}: {}\n", treeName.string(), strerror(errno));
        exit(1);
    }

    // Reserve header and TOC entries first. Move to 
    // file position after end of TOC entries.
    otree.seekg(thdr.dataofs);
    for (int idx = 0; idx < 2; idx++)
        thdr.rootp[idx] = writeTree(otree, findNode(4, 0, idx));
    
    // Rewind back to the beginning of file
    otree.seekg(0);

    otree.write((char *)&thdr, sizeof(zTreeHeader));
    otree.write((char *)toc, thdr.ntoc*sizeof(tocEntry));

    otree.close();
}

void zTree::extractTreeDB(int maxLOD)
{
    std::fstream tree;
    fs::path treePath = rootPath / "ofstree";

    if (!fs::exists(treePath) || !fs::is_directory(treePath))
        return;
    fs::path treeName = treePath / (layerName + ".tree");

    std::cout << fmt::format("Opening {}\n", treeName.string());

    std::fstream itree(treeName, std::ios::binary|std::ios::in);
    if (itree.fail())
    {
        std::cout << fmt::format("{}: {}\n", treeName.string(), strerror(errno));
        exit(1);
    }

    itree.read((char *)&thdr, sizeof(zTreeHeader));
    toc = new tocEntry[thdr.ntoc * sizeof(tocEntry)];
    itree.read((char *)toc, thdr.ntoc*sizeof(tocEntry));
    base = 0;

    for (int idx = 0; idx < 2; idx++)
        extractTree(itree, thdr.rootp[idx], 4, maxLOD);

    itree.close();
}

void zTree::listTreeDB(int maxLOD)
{
    std::fstream tree;
    fs::path treePath = rootPath / "ofstree";

    if (!fs::exists(treePath) || !fs::is_directory(treePath))
        return;
    fs::path treeName = treePath / (layerName + ".tree");

    std::fstream itree(treeName, std::ios::binary|std::ios::in);
    if (itree.fail())
    {
        std::cout << fmt::format("{}: {}\n", treeName.string(), strerror(errno));
        exit(1);
    }

    itree.read((char *)&thdr, sizeof(zTreeHeader));
    toc = new tocEntry[thdr.ntoc * sizeof(tocEntry)];
    itree.read((char *)toc, thdr.ntoc*sizeof(tocEntry));

    for (int idx = 0; idx < 2; idx++)
        listTree(itree, thdr.rootp[idx], 4, maxLOD);

    itree.close();
}

int zTree::addTree(zNode *node)
{
    if (node == nullptr)
        return -1;
    int tidx = thdr.ntoc;

    if (fs::exists(node->rpath))
    {
        uint32_t fsize = fs::file_size(node->rpath);

        std::ifstream ifile(node->rpath, std::ios::binary);
        uint8_t *data = new uint8_t[fsize];
        ifile.read((char *)data, fsize);
        // if (!ifile)
        //     std::cout << fmt::format("{}: error - only {} can read\n",
        //         node->rpath.string(), ifile.gcount());
        ifile.close();

        uint32_t dsize;
        if (thdr.flags & TREE_DEFLATE)
            dsize = deflateData(data, fsize, zdata, zsize);
        else
            dsize = fsize;

        delete [] data;

        toc[tidx].size = fsize;
        toc[tidx].pos  = thdr.total;
        toc[tidx].lod  = node->lod;
        toc[tidx].ilat = node->ilat;
        toc[tidx].ilng = node->ilng;
        thdr.total += dsize;
    }
    thdr.ntoc++;

    for (int cidx = 0; cidx < 4; cidx++)
        toc[tidx].child[cidx] = addTree(node->child[cidx]);

    return tidx;
}

void zTree::listTree(std::fstream &tree, int idx, int lod, int maxLOD)
{
    if (lod > maxLOD)
        return;
    if (idx >= thdr.ntoc)
        return;
    tocEntry *entry = toc+idx;

    uint32_t dsize = entry->size;
    uint32_t zsize = (idx < thdr.ntoc-1 ? toc[idx+1].pos : thdr.total) - entry->pos;

    std::cout << fmt::format("Index {}: File {} ({}/{} bytes) [{}%] ilat {} ilng {}\n",
        idx, "(unknown)", zsize, dsize, (zsize * 100) / dsize, entry->ilat, entry->ilng);

    if (lod < 4)
        return;
    for (int cidx = 0; cidx < 4; cidx++)
        listTree(tree, entry->child[cidx], lod+1, maxLOD);
}

void zTree::extractTree(std::fstream &tree, int idx, int lod, int maxLOD)
{
    if (lod > maxLOD)
        return;
    if (idx >= thdr.ntoc)
        return;

    tocEntry *entry = toc+idx;
    int ilat = entry->ilat;
    int ilng = entry->ilng;

    uint32_t dsize = entry->size;
    if (dsize == 0)
        return;
    
    size_t zsize = (idx < thdr.ntoc-1 ? toc[idx+1].pos : thdr.total) - entry->pos;
    uint8_t *zdata = new uint8_t[zsize];
    tree.seekg(thdr.dataofs + entry->pos);
    tree.read((char *)zdata, zsize);
    int nread = tree.gcount();

    uint8_t *data = new uint8_t[dsize];
    inflateData(zdata, zsize, data, dsize);

    // fs::path fileName = fmt::format("{}/{}/{:02d}/{:06d}/{:06d}.{}",
    //     rootPath.string(), layerName, lod, ilat, ilng, extName);
    fs::path rpath = fmt::format("{}/{}/{:02d}/{:06d}/",
        rootPath.string(), layerName, lod, ilat);
    fs::path lpath = fmt::format("{:02d}/{:06d}/{:06d}{}",
        lod, ilat, ilng, extName);
    fs::path fileName = rpath / fmt::format("{:06d}{}",
        ilng, extName);

    std::cout << fmt::format("Inflating {}\n", lpath.string());
    fs::create_directories(rpath);
    std::ofstream ofile(fileName, std::ios::binary|std::ios::out);

    ofile.write((char *)data, dsize);
    ofile.close();

    delete [] data;
    delete [] zdata;

    if (lod < 4)
        return;
    for (int cidx = 0; cidx < 4; cidx++)
        extractTree(tree, entry->child[cidx], lod+1, maxLOD); 
}

int zTree::writeTree(std::fstream &tree, zNode *node)
{
    if (node == nullptr)
        return -1;
    int tidx = thdr.ntoc;

    if (fs::exists(node->rpath))
    {
        size_t fsize = fs::file_size(node->rpath);
        
        std::ifstream ifile(node->rpath, std::ios::binary);
        uint8_t *data = new uint8_t[fsize];

        ifile.read((char *)data, fsize);
        if (ifile.gcount() < fsize)
        {
            std::cout << fmt::format("File {}: Unexpected end of file ({}/{} bytes) - aborted\n",
                node->lpath.string(), ifile.gcount(), fsize);
            ifile.close();
            exit(1);
        }
        ifile.close();

        size_t dsize;
        if (thdr.flags & TREE_DEFLATE)
        {
            dsize = deflateData(data, fsize, zdata, zsize);
            std::cout << fmt::format("Deflating {} (index {}) ({}/{} bytes) [{}%]\n",
                node->lpath.string(), tidx, dsize, fsize, (dsize * 100)/fsize);
            tree.write((char *)zdata, dsize);
        }
        else
        {
            std::cout << fmt::format("Copying {} (index {}) ({} bytes)\n",
                node->lpath.string(), tidx, dsize, fsize, (dsize * 100)/fsize);
            tree.write((char *)data, fsize);
            dsize = fsize;
        }

        delete [] data;

        toc[tidx].size = fsize;
        toc[tidx].pos  = thdr.total;
        toc[tidx].lod  = node->lod;
        toc[tidx].ilat = node->ilat;
        toc[tidx].ilng = node->ilng;
        thdr.total += dsize;
    }
    thdr.ntoc++;

    for (int cidx = 0; cidx < 4; cidx++)
        toc[tidx].child[cidx] = writeTree(tree, node->child[cidx]);

    return tidx;
}

void usage(cchar_t *cmd)
{
    std::cout << fmt::format("Usage: texpack [-e] [-l max lod] <path> <layer[:ext]>\n");
}

int main(int argc, char **argv)
{
    int maxLOD = 0;
    int idx, opt;

    while((opt = getopt(argc, argv, "elm:h")) != -1)
    {
        switch(opt)
        {
        case 'e':
            mode = Extract;
            continue;
        case 'l':
            mode = List;
            continue;
        case 'm':
            maxLOD = atoi(optarg);
            continue;

        case 'h':
        default:
            usage(argv[0]);
            return 0;
        }
    }

    idx += optind;
    if (idx+2 > argc)
    {
        usage(argv[0]);
        return 0;
    }

    cstr_t path = argv[idx++];
    cstr_t layer = argv[idx++];

    std::cout << ((mode == Archive) ? "Packing " : "Unpacking ") << layer << " for " << path << std::endl; 
    if (maxLOD > 0)
        std::cout << "Maximum LOD level: " << maxLOD << std::endl;
    else
        maxLOD = 19;

    zTree tree(path, layer, mode);

    switch(mode)
    {
    case Archive:
        std::cout << fmt::format("\nBuilding OFS terrain tree ...\n");
        tree.addLevels(4, maxLOD);
        tree.setupTreeDBw();
        tree.writeTreeDB();
        break;

    case Extract:
        std::cout << fmt::format("\nUnpacking OFS terrain tree ...\n");
        tree.setupTreeDBr();
        tree.extractTreeDB(maxLOD);
        break;
    
    case List:
        std::cout << fmt::format("\nListing OFS terrain tree ...\n");
        tree.listTreeDB(maxLOD);
        break;
    }

    return 0;
}