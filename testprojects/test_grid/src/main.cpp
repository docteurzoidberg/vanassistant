#include "../../../src/Drz_PGE_Engine.h"
#include "../../../src/vanassistant/Road.h"
#include "../../../src/fonts/Solid_Mono8pt7b.h"
#include "IDrzEngine.h"
#include <string>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class TestGrid : public Drz_PGE_Engine {
  public:
    TestGrid() : Drz_PGE_Engine(this) {
      road = new Road(this);
      sAppName = "TestGrid";
      //Load font
      LoadFont("solidmono8", &Solid_Mono8pt7b);
      SetFont("solidmono8");
    }

    bool OnUserCreate() override {
      ConsoleCaptureStdOut(true);
      return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
      
      // F1: show console
      if(GetKey(olc::Key::F1).bPressed) {
        ConsoleShow(olc::Key::ESCAPE, false);
      }

      road->Update(fElapsedTime);
      Clear(BLACK);
      road->Render(); 
      DrawText(std::to_string(GetFPS()), 4, 14, YELLOW);
      return true;
    }

    bool OnConsoleCommand(const std::string& text) override {
      return true;
    }

  private:
   Road* road;
};

int main() {
  TestGrid app;
  if (app.Construct(256, 240, 2, 2)) {
    app.Start();
  }
  return 0;
}