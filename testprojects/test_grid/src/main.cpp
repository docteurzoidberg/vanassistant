#include "../../../src/Drz_PGE_Engine.h"
#include "../../../src/vanassistant/Road.h"

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class TestGrid : public Drz_PGE_Engine {
  public:
    TestGrid() : Drz_PGE_Engine(this) {
      road = new Road(this);
      sAppName = "TestGrid";
    }

    bool OnUserCreate() override {
      ConsoleCaptureStdOut(true);
      return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
      road->Update(fElapsedTime);
      Clear(BLACK);
      road->Render();
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