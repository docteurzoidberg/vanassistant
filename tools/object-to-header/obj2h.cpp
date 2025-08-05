#include <fstream>
#include <gfx3d.h>
#include <iostream>
#include <sstream>
#include <string>

using namespace drz;

bool LoadOBJFile(GFX3D::mesh &model, std::string sFilename, bool bHasTexture) {
  std::ifstream f(sFilename);
  if (!f.is_open())
    return false;

  // Local cache of verts
  std::vector<drz::GFX3D::vec3d> verts;
  std::vector<drz::GFX3D::vec3d> norms;
  std::vector<drz::GFX3D::vec2d> texs;

  while (!f.eof()) {
    char line[128];
    f.getline(line, 128);

    std::stringstream s;
    s << line;

    std::cout << line << std::endl;

    char junk;

    if (line[0] == 'v') {
      if (line[1] == 't') {
        drz::GFX3D::vec2d v;
        s >> junk >> junk >> v.x >> v.y;
        // v.x = 1.0f - v.x;
        v.y = 1.0f - v.y;
        texs.push_back(v);
      } else if (line[1] == 'n') {
        drz::GFX3D::vec3d v;
        s >> junk >> junk >> v.x >> v.y >> v.z;
        norms.push_back(v);
      } else {
        drz::GFX3D::vec3d v;
        s >> junk >> v.x >> v.y >> v.z;
        verts.push_back(v);
      }
    }

    /*if (!bHasTexture)
    {
      if (line[0] == 'f')
      {
        int f[3];
        s >> junk >> f[0] >> f[1] >> f[2];
        tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
      }
    }
    else*/
    {
      if (line[0] == 'f') {
        s >> junk;

        std::string tokens[9];
        int nTokenCount = 0;
        while (!s.eof()) {
          char c = s.get();
          if (c == ' ' || c == '/') {
            if (tokens[nTokenCount].size() > 0) {
              nTokenCount++;
            }
          } else
            tokens[nTokenCount].append(1, c);
        }

        tokens[nTokenCount].pop_back();

        int stride = 1;
        if (!texs.empty())
          stride++;
        if (!norms.empty())
          stride++;

        if (!texs.empty()) {
          drz::GFX3D::triangle tri;
          tri.p[0] = verts[stoi(tokens[0 * stride]) - 1];
          tri.p[1] = verts[stoi(tokens[1 * stride]) - 1];
          tri.p[2] = verts[stoi(tokens[2 * stride]) - 1];

          tri.t[0] = texs[stoi(tokens[0 * stride + 1]) - 1];
          tri.t[1] = texs[stoi(tokens[1 * stride + 1]) - 1];
          tri.t[2] = texs[stoi(tokens[2 * stride + 1]) - 1];

          tri.col[0] = drz::WHITE;
          tri.col[1] = drz::WHITE;
          tri.col[2] = drz::WHITE;

          model.tris.push_back(tri);
          /*
                    model.tris.push_back({
                      verts[stoi(tokens[0 * stride]) - 1],
                      verts[stoi(tokens[1 * stride]) - 1],
                      verts[stoi(tokens[2 * stride]) - 1],
                      texs[stoi(tokens[0 * stride + 1]) - 1],
                      texs[stoi(tokens[1 * stride + 1]) - 1],
                      texs[stoi(tokens[2 * stride + 1]) - 1],
                      drz::WHITE, drz::WHITE, drz::WHITE});
          */
        } else {
          drz::GFX3D::triangle tri;
          tri.p[0] = verts[stoi(tokens[0 * stride]) - 1];
          tri.p[1] = verts[stoi(tokens[1 * stride]) - 1];
          tri.p[2] = verts[stoi(tokens[2 * stride]) - 1];

          tri.t[0] = drz::GFX3D::vec2d{0, 0, 0};
          tri.t[1] = drz::GFX3D::vec2d{0, 0, 0};
          tri.t[2] = drz::GFX3D::vec2d{0, 0, 0};

          tri.col[0] = drz::WHITE;
          tri.col[1] = drz::WHITE;
          tri.col[2] = drz::WHITE;

          model.tris.push_back(tri);
          /*
                    model.tris.push_back({
                      verts[stoi(tokens[0 * stride]) - 1],
                      verts[stoi(tokens[1 * stride]) - 1],
                      verts[stoi(tokens[2 * stride]) - 1],
                      drz::GFX3D::vec2d{0,0,0},
                      drz::GFX3D::vec2d{0,0,0},
                      drz::GFX3D::vec2d{0,0,0},
                      drz::WHITE, drz::WHITE, drz::WHITE });
          */
        }
      }
    }
  }
  return true;
}

bool SaveLBJHFile(GFX3D::mesh &model, std::string sFilename) {
  // open filename + ".h"
  std::ofstream f(sFilename + ".h");
  if (!f.is_open())
    return false;

  f << "#ifndef MODEL_" << sFilename << "_H" << std::endl;
  f << "#define MODEL_" << sFilename << "_H" << std::endl;
  f << std::endl;
  f << "#include <vector>" << std::endl;
  f << "#include <tuple>" << std::endl;
  f << std::endl;
  f << "const std::vector<std::tuple<float, float, float>> tris = {"
    << std::endl;
  for (const auto &v : model.tris) {
    f << "    {" << v.p[0].x << ", " << v.p[0].y << ", " << v.p[0].z << "},"
      << std::endl;
    f << "    {" << v.p[1].x << ", " << v.p[1].y << ", " << v.p[1].z << "},"
      << std::endl;
    f << "    {" << v.p[2].x << ", " << v.p[2].y << ", " << v.p[2].z << "},"
      << std::endl;
  }
  f << "};" << std::endl;
  f << std::endl;
  f << "#endif // MODEL_" << sFilename << "_H" << std::endl;
  f.close();

  return true;
}

int main() {
  GFX3D::mesh model;
  LoadOBJFile(model, "J7.obj", false);
  SaveLBJHFile(model, "J7");
  return 0;
}
