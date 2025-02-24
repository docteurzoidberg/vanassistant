#pragma once

#include "../../common/widgets/SceneWidget.h"

#include "../J7.h"

#include <memory>

class WidgetJ7Scene3d : public SceneWidget {
  public:
    std::unique_ptr<J7> j7;

    WidgetJ7Scene3d(int screenX, int screenY, int width, int height) : SceneWidget(screenX, screenY, width, height) {
      this->screenX = screenX;
      this->screenY = screenY;
      this->width = width;
      this->height = height;
      j7 = std::make_unique<J7>();
      Load();
    }

    void Load() override {
      for(auto model : j7->GetModels()) {
        AddModel(model);
      }
    }

    void Update(float fElapsedTime) override {
      j7->Update(fElapsedTime);
      SceneWidget::Update(fElapsedTime);
    }

    void Render() override {
      SceneWidget::Render();
    }

    void ToggleRenderMode() {
      if(renderMode == RENDER_WIREFRAME) {
        renderMode = RENDER_SOLID;
      } else {
        renderMode = RENDER_WIREFRAME;
      }
    }

    void ToggleDebugTriangles() {
      SceneWidget::ToggleDebugTriangles();
    }

    void DbgNextTriangle() {
      SceneWidget::DbgNextTriangle();
    }

    void DbgPrevTriangle() {
      SceneWidget::DbgPrevTriangle();
    }
};