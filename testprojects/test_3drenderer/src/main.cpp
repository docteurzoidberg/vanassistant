#include "../../../src/Drz_PGE_Engine.h"
#include "../../../src/fonts/Solid_Mono8pt7b.h"

#include "../include/Scene.h"
#include "../include/J7Model.h"

#define OLC_PGE_APPLICATION
#include <olcPixelGameEngine.h>

#define OLC_PGEX_GRAPHICS3D
#include <gfx3d.h> 

class Test3DRenderer : public Drz_PGE_Engine {
  public:
    Test3DRenderer() : Drz_PGE_Engine(this) {
      sAppName = "3D renderer";
      //Load font
      LoadFont("solidmono8", &Solid_Mono8pt7b);
    }

    bool OnUserCreate() override {
      ConsoleCaptureStdOut(true);
      scene = new Scene(this);
      model = new J7(scene);
      return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
      // F1: show console
      if(GetKey(olc::Key::F1).bPressed) {
        ConsoleShow(olc::Key::ESCAPE, false);
      }

      Clear(drz::BLACK);
      model->Update(fElapsedTime);
      //scene->Update(fElapsedTime);
      //scene->Render();
      return true;
    }

    bool OnConsoleCommand(const std::string& text) override {
      return true;
    }

  private:
    Scene* scene;
    J7* model;
};

int main() {
  Test3DRenderer app;
  if (app.Construct(256, 240, 2, 2)) {
    app.Start();
  }
  return 0;
}