#include <DrzEngine.h>
#include <DrzEngine_PGE.h>
#include <DrzGraphics.h>
#include <gfx3d.h>

#include <cstdint>
#include <string>
#include <iostream>

#include <J7.h> // Assuming j7model.h contains the model data
#include <vector>

using namespace drz;

//Base class to load a 3d model
class ModelA {
  public:
    ModelA() {}

    virtual void Update(float fElapsedTime) = 0;

    void SetupModel(const std::vector<std::tuple<GFX3D::vec3d, GFX3D::vec3d, GFX3D::vec3d>> tuples) {
      GFX3D::triangle tri;
      for (const auto& t : tuples) {
        tri.p[0] = std::get<0>(t);
        tri.p[1] = std::get<1>(t);
        tri.p[2] = std::get<2>(t);
        tri.t[0] = GFX3D::vec2d{0, 0, 0};
        tri.t[1] = GFX3D::vec2d{0, 0, 0};
        tri.t[2] = GFX3D::vec2d{0, 0, 0};
        tri.col[0] = drz::WHITE;
        tri.col[1] = drz::WHITE;
        tri.col[2] = drz::WHITE;
        this->tris.push_back(tri);
      }
      //std::cout << "Loading model" << std::endl;
      //std::cout << "Model loaded with " << tris.size() << " triangles" << std::endl;
    }

    std::vector<GFX3D::triangle> tris;
    GFX3D::mat4x4 rotationMatrix;
    GFX3D::mat4x4 translationMatrix;

  private:
};

class J7Model : public ModelA {
  public:
    J7Model() : ModelA() {
      // Convert j7model_tris (vector<tuple<float, float, float>>) to vector<tuple<GFX3D::vec3d, GFX3D::vec3d, GFX3D::vec3d>>
      std::vector<std::tuple<GFX3D::vec3d, GFX3D::vec3d, GFX3D::vec3d>> converted_tris;
      for (size_t i = 0; i + 2 < j7model_tris.size(); i += 3) {
        GFX3D::vec3d v0{std::get<0>(j7model_tris[i]), std::get<1>(j7model_tris[i]), std::get<2>(j7model_tris[i])};
        GFX3D::vec3d v1{std::get<0>(j7model_tris[i+1]), std::get<1>(j7model_tris[i+1]), std::get<2>(j7model_tris[i+1])};
        GFX3D::vec3d v2{std::get<0>(j7model_tris[i+2]), std::get<1>(j7model_tris[i+2]), std::get<2>(j7model_tris[i+2])};
        converted_tris.emplace_back(v0, v1, v2);
      }
      SetupModel(converted_tris);
    }

    void Update(float fElapsedTime) override {
      //TODO: rotate the model
    }
};

bool LoadOBJFile(GFX3D::mesh &model, std::string sFilename, bool bHasTexture) {
  std::ifstream f(sFilename);
  if (!f.is_open()) return false;

  // Local cache of verts
  std::vector<drz::GFX3D::vec3d> verts;
  std::vector<drz::GFX3D::vec3d> norms;
  std::vector<drz::GFX3D::vec2d> texs;

  while (!f.eof())
  {
    char line[128];
    f.getline(line, 128);

    std::stringstream s;
    s << line;

    std::cout << line << std::endl;

    char junk;

    if (line[0] == 'v')
    {
      if (line[1] == 't')
      {
        drz::GFX3D::vec2d v;
        s >> junk >> junk >> v.x >> v.y;
        //v.x = 1.0f - v.x;
        v.y = 1.0f - v.y;
        texs.push_back(v);
      }
      else if (line[1] == 'n')
      {
        drz::GFX3D::vec3d v;
        s >> junk >> junk >> v.x >> v.y >> v.z;										
        norms.push_back(v);
      }
      else
      {
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
      if (line[0] == 'f')
      {
        s >> junk;

        std::string tokens[9];
        int nTokenCount = 0;
        while (!s.eof())
        {
          char c = s.get();
          if (c == ' ' || c == '/')
          {
            if (tokens[nTokenCount].size() > 0)
            {
              nTokenCount++;
            }
          }
          else
            tokens[nTokenCount].append(1, c);
        }

        tokens[nTokenCount].pop_back();

        int stride = 1;
        if (!texs.empty()) stride++;
        if (!norms.empty()) stride++;

        if (!texs.empty())
        {
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
        }
        else
        {
          drz::GFX3D::triangle tri;
          tri.p[0] = verts[stoi(tokens[0 * stride]) - 1];
          tri.p[1] = verts[stoi(tokens[1 * stride]) - 1];
          tri.p[2] = verts[stoi(tokens[2 * stride]) - 1];

          tri.t[0] = drz::GFX3D::vec2d{0,0,0};
          tri.t[1] = drz::GFX3D::vec2d{0,0,0};
          tri.t[2] = drz::GFX3D::vec2d{0,0,0};

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
  if (!f.is_open()) return false;


  f << "#ifndef MODEL_" << sFilename << "_H" << std::endl;
  f << "#define MODEL_" << sFilename << "_H" << std::endl;
  f << std::endl;
  f << "#include <vector>" << std::endl;
  f << "#include <tuple>" << std::endl;
  f << std::endl;
  f << "const std::vector<std::tuple<float, float, float>> tris = {" << std::endl;
  for (const auto& v : model.tris)
  {
    f << "    {" << v.p[0].x << ", " << v.p[0].y << ", " << v.p[0].z << "}," << std::endl;
    f << "    {" << v.p[1].x << ", " << v.p[1].y << ", " << v.p[1].z << "}," << std::endl;
    f << "    {" << v.p[2].x << ", " << v.p[2].y << ", " << v.p[2].z << "}," << std::endl;
  }
  f << "};" << std::endl;
  f << std::endl;
  f << "#endif // MODEL_" << sFilename << "_H" << std::endl;
  f.close();  

  return true;
}

class TestApp: public IDrzEngineApp {
  public:

    TestApp() {
      std::cout << "TestApp constructor called" << std::endl;
      gfx = DrzGraphics::Get();
    }

    ~TestApp(){
      std::cout << "TestApp destructor called" << std::endl;
    } 

    void Setup() {
      std::cout << "TestApp::Setup called" << std::endl;

      j7model = new J7Model();

      //LoadOBJFile(model, "J7.obj", false);
      //SaveLBJHFile(model, "J7");

      for (auto &tri : j7model->tris)
      {
          std::swap(tri.p[0], tri.p[2]);
          //std::swap(tri.t[0], tri.t[2]);
      }
      drz::GFX3D::ConfigureDisplay(gfx);
      renderer.SetProjection(90.0f, (float)gfx->GetScreenHeight() / (float)gfx->GetScreenWidth(), 0.1f, 1000.0f, 0.0f, 0.0f, gfx->GetScreenWidth(), gfx->GetScreenHeight());
    }

    void Update(float elapsedTime) {
      //std::cout << "TestApp::Update called with elapsed time: " << elapsedTime << std::endl;
      
      fTheta+=elapsedTime*1.0f;

      // Start of 3D Stuff
      drz::GFX3D::mat4x4 matRotateY = drz::GFX3D::Math::Mat_MakeRotationY(fTheta);

      drz::GFX3D::mat4x4 matTrans;
      matTrans = drz::GFX3D::Math::Mat_MakeTranslation(0.0f, 0.0f, 0.0f); //no need translating for now

      drz::GFX3D::mat4x4 matWorld;
      matWorld = drz::GFX3D::Math::Mat_MakeIdentity();	// Form World Matrix
      matWorld = drz::GFX3D::Math::Mat_MultiplyMatrix(matWorld, matRotateY); // Transform by rotation
      matWorld = drz::GFX3D::Math::Mat_MultiplyMatrix(matWorld, matTrans); // Transform by translation

      // Create a "Point At"
      drz::GFX3D::vec3d vTarget = { 0,0,1 };
      drz::GFX3D::mat4x4 matCameraRot = drz::GFX3D::Math::Mat_MakeRotationY(fYaw);
      vLookDir = drz::GFX3D::Math::Mat_MultiplyVector(matCameraRot, vTarget);
      vTarget = drz::GFX3D::Math::Vec_Add(vCamera, vLookDir);
      drz::GFX3D::mat4x4 matCamera = drz::GFX3D::Math::Mat_PointAt(vCamera, vTarget, vUp);
      renderer.SetCamera(vCamera, vTarget, vUp);

      drz::GFX3D::vec3d vForward = drz::GFX3D::Math::Vec_Mul(vLookDir, 8.0f * elapsedTime);

      gfx->Clear(drz::DARK_CYAN);

      drz::GFX3D::ClearDepth(gfx);
      renderer.SetTransform(matWorld);
      renderer.SetLightSource(1, drz::GFX3D::LIGHTS::LIGHT_DIRECTIONAL, drz::WHITE, vSun);

      j7model->rotationMatrix = matRotateY;

      renderer.Render(j7model->tris, drz::GFX3D::RENDERFLAGS::RENDER_FLAT|drz::GFX3D::RENDERFLAGS::RENDER_DEPTH|drz::GFX3D::RENDERFLAGS::RENDER_LIGHTS);
        //renderer.Render(model.tris, olc::GFX3D::RENDERFLAGS::RENDER_WIRE|olc::GFX3D::RENDERFLAGS::RENDER_DEPTH|olc::GFX3D::RENDER_CULL_CW);

    }

    bool Command(const std::string& command) {
      std::cout << "Command: " << command << std::endl;
      return false; // Command not handled
    }

    bool Data(const uint8_t* data, size_t size) {
      std::cout << "TestApp::Data called with size: " << size << std::endl;
      // Process data here
      return false; // Data not handled
    }

    J7Model* j7model;
    IDrzGraphics* gfx;
    drz::GFX3D::mesh model;
    drz::GFX3D::PipeLine renderer;
    drz::GFX3D::vec3d vUp = { 0, 1, 0 };
    drz::GFX3D::vec3d vCamera = { 0, 0, -3 };
    drz::GFX3D::vec3d vLookDir = { 0, 0,-1 };
    drz::GFX3D::vec3d vSun = {0, 0, -1};
    float fYaw = 0.0f;		// FPS Camera rotation in XZ plane
	  float fTheta = 0.0f;	// Spins World transform
};

int main(){
  IDrzEngine* drzenginepge = new DrzEngine_PGE(320, 240, 2);
  TestApp* testapp = new TestApp();
  DrzEngine::UseApp(testapp);
  DrzEngine::Setup();  
  DrzEngine::Start();
  return 0;
}
