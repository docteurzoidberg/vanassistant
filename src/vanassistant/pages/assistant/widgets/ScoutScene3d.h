#pragma once

#include "../../../common/widgets/SceneWidget.h"

#include "../Scout.h"

#include <memory>

class WidgetScoutScene3d : public SceneWidget {
  public:
    std::unique_ptr<Scout> scout;

    WidgetScoutScene3d(int screenX, int screenY, int width, int height) : SceneWidget(screenX, screenY, width, height) {
      this->screenX = screenX;
      this->screenY = screenY;
      this->width = width;
      this->height = height;
      scout = std::make_unique<Scout>();
      Load();
    }

    void Load() override {
      for(auto model : scout->GetModels()) {
        AddModel(model);
      }
      scout->SetJawOpening(0.5f);
    }

    void Update(float fElapsedTime) override {
      scout->Update(fElapsedTime);
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

    void SetJawOpening(float opening) {
      scout->SetJawOpening(opening);
    }
};