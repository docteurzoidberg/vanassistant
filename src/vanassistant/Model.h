#pragma once

#include "../../include/IDrzEngine.h"

#include <vector>

//Base class to load a 3d model
class Model {
  public:
    Model() {
      //Load the face vertices intro triangles and points references
      loadTriRefs();
    }
    void loadTriRefs() {
      for (auto& f : faces)
      {
        triangleref t;
        t.p[0] = &verts[f.f[0] - 1];
        t.p[1] = &verts[f.f[1] - 1];
        t.p[2] = &verts[f.f[2] - 1];
        tris.push_back(t);
      } 
    }
    std::vector<vec3d> verts;
    std::vector<face> faces;
    std::vector<triangleref> tris;
};
