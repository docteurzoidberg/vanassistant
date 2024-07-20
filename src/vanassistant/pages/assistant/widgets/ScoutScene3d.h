#pragma once

#include <IDrzEngine.h>
#include <memory>
#include "../Scout.h"
#include "../../../IWidget.h"

class WidgetScoutScene3d : public IWidget {
  public:
    std::unique_ptr<Scene> scene;
    std::unique_ptr<Scout> scout;

    WidgetScoutScene3d(int screenX, int screenY, int width, int height) : IWidget(screenX, screenY, width, height) {
      scene = std::make_unique<Scene>(width, height, screenX, screenY);
      scout = std::make_unique<Scout>();
      scout->AddToScene(scene.get());
    }

    void Load() override {
      scout->SetJawOpening(0.5f);
    }

    void Update(float fElapsedTime) override {
      scout->Update(fElapsedTime);
      scene->Update(fElapsedTime);
    }

    void Render() override {
      scene->Render();
    }

    void ToggleRenderMode() {
      if(scene->renderMode == RENDER_WIREFRAME) {
        scene->renderMode = RENDER_SOLID;
      } else {
        scene->renderMode = RENDER_WIREFRAME;
      }
    }

    void ToggleDebugTriangles() {
      scene->ToggleDebugTriangles();
    }

    void DbgNextTriangle() {
      scene->DbgNextTriangle();
    }

    void DbgPrevTriangle() {
      scene->DbgPrevTriangle();
    }

    void SetJawOpening(float opening) {
      scout->SetJawOpening(opening);
    }
};