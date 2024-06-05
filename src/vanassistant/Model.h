#pragma once

#include "../../include/IDrzEngine.h"

#include <iostream>
#include <vector>

//Base class to load a 3d model
class Model {
  public:
    Model(std::vector<vec3d>* verts, std::vector<face>* faces) : verts(verts), faces(faces){
    
    }
    
    void LoadModel() {
      std::cout << "Vert count: " << verts->size() << std::endl;
      std::cout << "Face count: " << faces->size() << std::endl;
      std::cout << "Loading model" << std::endl;
      LoadTriRefs();
      std::cout << "Model loaded with " << tris.size() << " triangles" << std::endl;
    }

    std::vector<vec3d>* verts;
    std::vector<face>* faces;
    std::vector<triangleref> tris;

    private:
      void LoadTriRefs() {
        for (int i=0; i<faces->size(); i++) {
          face* f = &faces->at(i);
          vec3d* p0 = &verts->at(f->f[0] - 1);
          vec3d* p1 = &verts->at(f->f[1] - 1);
          vec3d* p2 = &verts->at(f->f[2] - 1);
          tris.push_back({
            p0, p1, p2
          });
        } 
      }
};
