#pragma once

#include "gfx3d.h"
#include <IDrzEngine.h>

#include <iostream>
#include <vector>

using namespace drz;

//Base class to load a 3d model
class Model {
  public:
    Model(std::vector<GFX3D::vec3d>* verts, std::vector<face>* faces) : verts(verts), faces(faces){}

    virtual void Update(float fElapsedTime) = 0;

    void SetupModel() {
      //std::cout << "Vert count: " << verts->size() << std::endl;
      //std::cout << "Face count: " << faces->size() << std::endl;
      //std::cout << "Loading model" << std::endl;
      _loadTriRefs();
      std::cout << "Model loaded with " << tris.size() << " triangles" << std::endl;
    }

    std::vector<GFX3D::vec3d>* verts;
    std::vector<face>* faces;
    std::vector<GFX3D::triangle> tris;

    Matrix4x4 rotationMatrix;
    Matrix4x4 translationMatrix;
      
  private:
    void _loadTriRefs() {
      for (int i=0; i<faces->size(); i++) {
        face* f = &faces->at(i);
        drz::GFX3D::vec3d p0 = verts->at(f->f[0] - 1);
        drz::GFX3D::vec3d p1 = verts->at(f->f[1] - 1);
        drz::GFX3D::vec3d p2 = verts->at(f->f[2] - 1);
        GFX3D::triangle tri = {p2, p1, p0, 	{0,0,0},drz::WHITE, drz::WHITE, drz::WHITE};
        tris.push_back(tri);
      } 
    }
};
