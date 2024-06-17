#include "../../../src/Drz_PGE_Engine.h"
#include "../../../src/vanassistant/Starfield.h"
#include "../../../src/fonts/Solid_Mono8pt7b.h"

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class TestStarfield : public Drz_PGE_Engine {
  public:
    TestStarfield() : Drz_PGE_Engine(this) {
      startfield = new Starfield(this, 2000);
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

      startfield->Update(fElapsedTime);
      Clear(BLACK);
      startfield->Render();
      DrawText(std::to_string(GetFPS()), 4, 14, YELLOW);
      return true;
    }

    bool OnConsoleCommand(const std::string& text) override {
      return true;
    }

  private:
   Starfield* startfield;
};

int main() {
  TestStarfield app;
  if (app.Construct(256, 240, 2, 2)) {
    app.Start();
  }
  return 0;
}