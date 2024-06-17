//https://pgetinker.com/s/v4ObPyh4Brp

#include "../../../src/Drz_PGE_Engine.h"
#include "../../../src/fonts/Solid_Mono8pt7b.h"

#include <IDrzEngine.h>

#include "../include/Scene.h"
#include "../include/J7Model.h"

#define OLC_PGE_APPLICATION
#include <olcPixelGameEngine.h>

#define OLC_PGEX_GRAPHICS3D
#include <gfx3d.h>



class Example : public Drz_PGE_Engine {
  public:

    float fYaw = 0.0f;		// FPS Camera rotation in XZ plane
	  float fTheta = 0.0f;	// Spins World transform

    Example() : Drz_PGE_Engine(this) {
      sAppName = "Example 3D";
      GFX3D::SetEngine(this);
      //Load font
      LoadFont("solidmono8", &Solid_Mono8pt7b);
      SetFont("solidmono8");
    }

    bool OnUserCreate() override {
      //ConsoleCaptureStdOut(true);
      scene = new Scene(this, 0, 0, GetScreenWidth(), GetScreenHeight());
      model = new J7(scene);
      return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
      // F1: show console
      if(GetKey(olc::Key::F1).bPressed) {
        ConsoleShow(olc::Key::ESCAPE, false);
      }
          
      if(GetKey(olc::Key::R).bPressed) {
        scene->renderMode = (scene->renderMode == RENDER_SOLID) ? RENDER_WIREFRAME : RENDER_SOLID;
      }
      
      fTheta += 0.1f * fElapsedTime;	// Spin

      Clear(drz::BLACK);

      scene->Update(fElapsedTime, vCamera, fYaw, fTheta);
      scene->Render();

      //Draw FPS
      DrawText( std::to_string(GetFPS()), 4,14, drz::YELLOW);

      if (GetKey(olc::Key::UP).bHeld)
        vCamera.y += 8.0f * fElapsedTime;	// Travel Upwards
      
      if (GetKey(olc::Key::DOWN).bHeld)
        vCamera.y -= 8.0f * fElapsedTime;	// Travel Downwards

      // Dont use these two in FPS mode, it is confusing :P
      if (GetKey(olc::Key::LEFT).bHeld)
        vCamera.x -= 8.0f * fElapsedTime;	// Travel Along X-Axis

      if (GetKey(olc::Key::RIGHT).bHeld)
        vCamera.x += 8.0f * fElapsedTime;	// Travel Along X-Axis

          // Standard FPS Control scheme, but turn instead of strafe
      if (GetKey(olc::Key::W).bHeld)
        vCamera = GFX3D::Math::Vec_Add(vCamera, scene->vForward);

      if (GetKey(olc::Key::S).bHeld)
        vCamera = GFX3D::Math::Vec_Sub(vCamera, scene->vForward);

      if (GetKey(olc::Key::A).bHeld)
        fYaw -= 2.0f * fElapsedTime;

      if (GetKey(olc::Key::D).bHeld)
        fYaw += 2.0f * fElapsedTime;


      return true;
    }

    bool OnConsoleCommand(const std::string& text) override {
      return true;
    }

  private:
  	GFX3D::vec3d vCamera = { 0, 0, -3 };
    Scene* scene;
    J7* model;
};

int main() {
  Example app;
  if (app.Construct(360, 370, 2, 2)) {
    app.Start();
  }
  return 0;
}