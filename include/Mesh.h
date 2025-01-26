#ifndef MESH_H_
#define MESH_H_
#include <ngl/Vec3.h>
#include <ngl/Mat4.h>
#include <ngl/Obj.h>
#include <ngl/Transformation.h>
#include "WindowParams.h"
// this must be included after NGL includes else we get a clash with gl libs
#include <QOpenGLWindow>
#include <memory>


class Mesh
{
    std::string m_OBJFilename;
    std::string m_TEXFilename;
    ngl::Vec3 m_POS;
    std::unique_ptr<ngl::Obj> m_MESH;
public:
    Mesh() : m_OBJFilename("models/test2.obj"), m_TEXFilename("textures/planks.png"){}
    Mesh(const std::string &_OBJname, const std::string &_TEXname)
        : m_OBJFilename(_OBJname), m_TEXFilename(_TEXname) {}
    void CreateVAO();
    void Draw();
    void Transform(float _xDif, float _yDif, float _zDif);
    ngl::Transformation m_TRANS;
    unsigned int texId;
};



#endif