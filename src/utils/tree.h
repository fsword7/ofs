// tree.h - Quadtree/Octree package
//
// Author:  Tim Stark
// Date:    Apr 18, 2022

#pragma once

#define QTREE_NODES 4   // Quadtree structure
#define OTREE_NODES 8   // Octree structure

template <class Object, int nodes>
class Tree
{
public:
    Tree(Object *parent = nullptr)
    : parent(parent)
    {
        for (int idx = 0; idx < nodes; idx++)
            child[idx] = nullptr;
    }

    ~Tree()
    {
        for (int idx = 0; idx < nodes; idx++)
            if (child[idx] != nullptr)
                delete child[idx];
    }

    inline Object *getParent() const        { return parent; }
    inline Object *getChild(int idx) const  { return (idx < nodes) ? child[idx] : nullptr; }

    Object *addChild(int idx, Object *nChild)
    {
        if (idx < 0 || idx >= nodes)
            return nullptr;
        child[idx] = nChild;

        return child[idx];
    }

    bool deleteChild(int idx)
    {
        if (idx < 0 || idx >= nodes)
            return false;
        if (child[idx] != nullptr)
        {
            delete child[idx];
            child[idx] = nullptr;
            return true;
        }
        return false;
    }

    bool deleteChildren()
    {
        bool ok = true;
        for (int idx = 0; idx < nodes; idx++)
        {
            if (child[idx] != nullptr)
            {
                if (child[idx]->deleteChildren() == true)
                {
                    delete child[idx];
                    child[idx] = nullptr;
                }
                else
                    ok = false;
            }
        }
        return ok;
    }

private:
    Object *parent = nullptr;
    Object *child[nodes];
};
