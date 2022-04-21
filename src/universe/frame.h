// frame.h - Frame Reference package
//
// Author:  Tim Stark
// Date:    Apt 21, 2022

#pragma once

class Object;
class celBody;
class celStar;

class FrameTree
{
public:
    FrameTree(celBody *body);
    FrameTree(celStar *star);
    virtual ~FrameTree();

    inline celStar *getStar() const         { return parentStar; }
    inline celBody *getBody() const         { return parentBody; }
    inline int getSystemSize() const        { return objects.size(); }
    inline bool isRoot() const              { return parentBody == nullptr; }
    
    void addObject(Object *object);
    Object *getObject(int idx) const;

private:
    celStar *parentStar = nullptr;
    celBody *parentBody = nullptr;

    std::vector<Object *> objects;

    // Frame *defaultFrame = nullptr;
};