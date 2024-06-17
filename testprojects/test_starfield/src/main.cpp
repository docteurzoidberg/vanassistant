#include "../../../src/Drz_PGE_Engine.h"
#include "../../../src/vanassistant/Starfield.h"

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class TestStarfield : public Drz_PGE_Engine {
  public:
    TestStarfield() : Drz_PGE_Engine(this) {
      startfield = new Starfield(this, 2000);
      sAppName = "TestGrid";
    }

    bool OnUserCreate() override {
      ConsoleCaptureStdOut(true);
      return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
      startfield->Update(fElapsedTime);
      Clear(BLACK);
      startfield->Render();
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