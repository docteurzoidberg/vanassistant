#include <DrzEngine.h>
#include <DrzEngine_PGE.h>
#include <DrzGraphics.h>
#include <gfx3d.h>

#include <cstdint>
#include <iostream>
#include <string>

#include <vector>

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
        }
      }
    }
  }
  return true;
}

class ViewerApp : public IDrzEngineApp {
public:
  ViewerApp(std::string modelFilename) : modelFilename(modelFilename) {
    gfx = DrzGraphics::Get();
  }

  ~ViewerApp() {}

  void Setup() {

    std::cout << "Loading model: " << modelFilename << std::endl;

    if (!LoadOBJFile(model, modelFilename, false)) {
      std::cerr << "Failed to load model: " << modelFilename << std::endl;
      return;
    }

    std::cout << "Model loaded with " << model.tris.size() << " triangles"
              << std::endl;

    drz::GFX3D::ConfigureDisplay(gfx);
    renderer.SetProjection(
        90.0f, (float)gfx->GetScreenHeight() / (float)gfx->GetScreenWidth(),
        0.1f, 1000.0f, 0.0f, 0.0f, gfx->GetScreenWidth(),
        gfx->GetScreenHeight());
  }

  void Update(float elapsedTime) {

    fTheta += elapsedTime * 1.0f;

    // Start of 3D Stuff
    drz::GFX3D::mat4x4 matRotateY = drz::GFX3D::Math::Mat_MakeRotationY(fTheta);

    drz::GFX3D::mat4x4 matTrans;
    matTrans = drz::GFX3D::Math::Mat_MakeTranslation(
        0.0f, 0.0f, 0.0f); // no need translating for now

    drz::GFX3D::mat4x4 matWorld;
    matWorld = drz::GFX3D::Math::Mat_MakeIdentity(); // Form World Matrix
    matWorld = drz::GFX3D::Math::Mat_MultiplyMatrix(
        matWorld, matRotateY); // Transform by rotation
    matWorld = drz::GFX3D::Math::Mat_MultiplyMatrix(
        matWorld, matTrans); // Transform by translation

    // Create a "Point At"
    drz::GFX3D::vec3d vTarget = {0, 0, 1};
    drz::GFX3D::mat4x4 matCameraRot = drz::GFX3D::Math::Mat_MakeRotationY(fYaw);
    vLookDir = drz::GFX3D::Math::Mat_MultiplyVector(matCameraRot, vTarget);
    vTarget = drz::GFX3D::Math::Vec_Add(vCamera, vLookDir);
    drz::GFX3D::mat4x4 matCamera =
        drz::GFX3D::Math::Mat_PointAt(vCamera, vTarget, vUp);
    renderer.SetCamera(vCamera, vTarget, vUp);

    drz::GFX3D::vec3d vForward =
        drz::GFX3D::Math::Vec_Mul(vLookDir, 8.0f * elapsedTime);

    gfx->Clear(drz::DARK_CYAN);

    drz::GFX3D::ClearDepth();
    renderer.SetTransform(matWorld);
    renderer.SetLightSource(1, drz::GFX3D::LIGHTS::LIGHT_DIRECTIONAL,
                            drz::WHITE, vSun);
    renderer.Render(model.tris, drz::GFX3D::RENDERFLAGS::RENDER_FLAT |
                                    drz::GFX3D::RENDERFLAGS::RENDER_DEPTH |
                                    drz::GFX3D::RENDERFLAGS::RENDER_LIGHTS);
  }

  bool Command(const std::string &command) {
    return false; // Command not handled
  }

  bool Data(const uint8_t *data, size_t size) {
    return false; // Data not handled
  }

  std::string modelFilename;
  IDrzGraphics *gfx;
  drz::GFX3D::mesh model;
  drz::GFX3D::PipeLine renderer;
  drz::GFX3D::vec3d vUp = {0, 1, 0};
  drz::GFX3D::vec3d vCamera = {0, 0, -3};
  drz::GFX3D::vec3d vLookDir = {0, 0, -1};
  drz::GFX3D::vec3d vSun = {0, 0, -1};
  float fYaw = 0.0f;   // FPS Camera rotation in XZ plane
  float fTheta = 0.0f; // Spins World transform
};

int main(int argc, char *argv[]) {

  // model to load is argument 1
  if (argc < 2) {
    std::cout << "Usage: obj-viewer <model.obj>" << std::endl;
    return 1;
  }
  std::string modelFilename = argv[1];
  if (modelFilename.substr(modelFilename.find_last_of(".") + 1) != "obj") {
    std::cout << "Error: Model file must be a .obj file." << std::endl;
    return 1;
  }

  IDrzEngine *drzenginepge = new DrzEngine_PGE(320, 240, 2);
  ViewerApp *viewerapp = new ViewerApp(modelFilename);
  DrzEngine::UseApp(viewerapp);
  DrzEngine::Setup();
  DrzEngine::Start();
  return 0;
}
