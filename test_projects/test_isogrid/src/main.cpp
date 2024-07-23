#include <Drz_Engine_PGE.h>
#include "../../../src/fonts/Solid_Mono8pt7b.h"
#include <IDrzEngine.h>

#include <cmath>
#include <string>

#include "ModelCube.h"
#include "ModelSpaceship.h"
#include "IsometricGrid3dRenderer.h"

class TestIsoGrid3dRenderer : public Drz_PGE_Engine {
public:
  TestIsoGrid3dRenderer() : Drz_PGE_Engine(this) {
    widgetCube = new IsoGrid3dRenderer(this, 0, 0, 60, 60, 3, BLACK);
    widgetSpaceShip = new IsoGrid3dRenderer(this, 60, 0, 60, 60, 3, BLACK);
    sAppName = "TestIsoGridRenderer";
    // Load font
    LoadFont("solidmono8", &Solid_Mono8pt7b);
    SetFont("solidmono8");
  }

  bool OnUserCreate() override {

    // Scale cube model to fit exactly one grid cell
    //float gridCellSize = widgetCube->GetGridCellSize(); // Assuming GetGridCellSize() returns the size of a grid cell

    // Calculate scale for the cube to fit exactly one cell
    //float cubeScale = CalculateModelScale(cubeVertices, gridCellSize);

    // Calculate scale for the spaceship to fit within three cells
    //float spaceshipScale = CalculateModelScale(spaceshipVertices, 3.0f * gridCellSize);


    widgetCube->LoadModel(cubeVertices, cubeFaces);
    widgetCube->ScaleModel(0.5f);

    widgetSpaceShip->LoadModel(spaceshipVertices, spaceshipFaces);
    widgetSpaceShip->ScaleModel(1.5f);

    ConsoleCaptureStdOut(true);
    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override {
    // Read Inputs
    // F1: show console
    if (GetKey(olc::Key::F1).bPressed) {
      ConsoleShow(olc::Key::ESCAPE, false);
    }
    // F: toggle show fps
    if (GetKey(olc::Key::F).bPressed) {
      drawFPS = !drawFPS;
    }

    // Apply rotations and translations to the models and grid
    static float theta = 0.0f;
    theta += fElapsedTime; // Increase theta based on elapsed time

    //widgetCube->RotateModel(theta, theta, theta);
    widgetCube->RotateGrid(M_PI/3, theta , M_PI/3);

    widgetSpaceShip->RotateModel(theta, theta, theta);
    //widgetSpaceShip->RotateGrid(theta , theta, theta);

    // Update widgets
    widgetCube->Update(fElapsedTime);
    widgetSpaceShip->Update(fElapsedTime);

    // Clear background
    Clear(BLACK);

    // Render widgets
    widgetCube->Render();
    widgetSpaceShip->Render();

    // Draw fps
    if (drawFPS) {
      DrawFPS();
    }
    return true;
  }

  bool OnConsoleCommand(const std::string& text) override {
    return true;
  }

  void DrawFPS() {
    auto text = std::to_string(GetFPS());
    auto size = GetTextBounds(text, 0, 0);
    DrawText(text, GetScreenWidth() - size.w - 4, 14, YELLOW);
  }

private:
  IsoGrid3dRenderer* widgetCube;
  IsoGrid3dRenderer* widgetSpaceShip;
  bool drawFPS = true;

  // Function to calculate the scale factor for a model to fit within a given size
  float CalculateModelScale(const std::vector<std::tuple<float, float, float>>& vertices, float maxSize) {
    float min_x = std::numeric_limits<float>::max();
    float max_x = std::numeric_limits<float>::min();
    float min_y = std::numeric_limits<float>::max();
    float max_y = std::numeric_limits<float>::min();
    float min_z = std::numeric_limits<float>::max();
    float max_z = std::numeric_limits<float>::min();

    for (const auto& vertex : vertices) {
      min_x = std::min(min_x, std::get<0>(vertex));
      max_x = std::max(max_x, std::get<0>(vertex));
      min_y = std::min(min_y, std::get<1>(vertex));
      max_y = std::max(max_y, std::get<1>(vertex));
      min_z = std::min(min_z, std::get<2>(vertex));
      max_z = std::max(max_z, std::get<2>(vertex));
    }

    float modelWidth = max_x - min_x;
    float modelHeight = max_y - min_y;
    float modelDepth = max_z - min_z;

    float maxDimension = std::max({modelWidth, modelHeight, modelDepth});
    return maxSize / maxDimension;
  }
};

int main() {
  TestIsoGrid3dRenderer app;
  //480x240 = two widgets of 240x240
  if (app.Construct(480, 240, 2, 2)) {
    app.Start();
  }
  return 0;
}
