#ifndef ISOMETRICGRID3DRENDERER_H
#define ISOMETRICGRID3DRENDERER_H

#include "IDrzEngine.h"
#include <algorithm>
#include <tuple>
#include <vector>
#include <cmath>
#include <limits>

using namespace drz;

enum RenderMode {
    RENDER_WIREFRAME,
    RENDER_SOLID
};

class IsoGrid3dRenderer {
public: 
  RenderMode renderMode = RENDER_SOLID;
  IsoGrid3dRenderer(IDrzEngine* engine, int screenX, int screenY, int width, int height, int gridSize=5, color backgroundColor=BLACK) :
    engine(engine), screenX(screenX), screenY(screenY), width(width), height(height), gridSize(gridSize), backgroundColor(backgroundColor) {
  }

  void LoadModel(const std::vector<std::tuple<float, float, float>>& vertices, const std::vector<std::tuple<int, int, int>>& faces) {
    this->vertices = vertices;
    this->faces = faces;
    CalculateModelParameters();
  }

  void TranslateGrid(float x, float y, float z) {
    gridTranslationX = x;
    gridTranslationY = y;
    gridTranslationZ = z;
  }

  void RotateGrid(float x, float y, float z) {
    gridRotationX = x;
    gridRotationY = y;
    gridRotationZ = z;
  }

  void TranslateModel(float x, float y, float z) {
    modelTranslationX = x;
    modelTranslationY = y;
    modelTranslationZ = z;
  }

  void RotateModel(float x, float y, float z) {
    modelRotationX = x;
    modelRotationY = y;
    modelRotationZ = z;
  }

  void ScaleModel(float scale) {
    modelScale = scale;
  }

  float GetGridCellSize() const {
    return width / (float)(gridSize + 1); // Assuming width represents the widget width
  }

  void setMaxFPS(int maxFPS) {
    this->maxFPS = maxFPS;
  }

  // Update frame. elapsed time in seconds
  void Update(float fElapsedTime) {
    UpdateModelTransformations();
  }

  // Render is relative to screenX, screenY offset and using own width and height instead of screen width and height
  void Render() {
    static float accumulatedTime = 0.0f;
    accumulatedTime += rotation_speed;
    if (accumulatedTime < 1.0f / maxFPS) return;
    accumulatedTime -= 1.0f / maxFPS;

    engine->FillRect(screenX, screenY, width, height, backgroundColor);
    DrawIsoGrid();
    DrawModel();
  }

  void ToggleRenderMode() {
    renderMode = (renderMode == RENDER_SOLID) ? RENDER_WIREFRAME : RENDER_SOLID;
  }


protected:

  void CalculateModelParameters() {
    for (const auto& vertex : vertices) {
      min_x = std::min(min_x, std::get<0>(vertex));
      max_x = std::max(max_x, std::get<0>(vertex));
      min_y = std::min(min_y, std::get<1>(vertex));
      max_y = std::max(max_y, std::get<1>(vertex));
      min_z = std::min(min_z, std::get<2>(vertex));
      max_z = std::max(max_z, std::get<2>(vertex));
    }
    //model_center_x = (max_x + min_x) / 2.0f;
    //model_center_y = (max_y + min_y) / 2.0f;
    //model_center_z = (max_z + min_z) / 2.0f;
    scale_factor = desired_width / (max_x - min_x);
  }

  void UpdateModelTransformations() {
    transformed_vertices.clear();
    for (const auto& vertex : vertices) {
      float x = (std::get<0>(vertex)) * scale_factor * modelScale + modelTranslationX;
      float y = (std::get<1>(vertex)) * scale_factor * modelScale + modelTranslationY;
      float z = (std::get<2>(vertex)) * scale_factor * modelScale + modelTranslationZ;
      RotateVertex(x, y, z, modelRotationX, modelRotationY, modelRotationZ);
      transformed_vertices.emplace_back(x, y, z);
    }
  }

  void RotateVertex(float& x, float& y, float& z, float rotationX, float rotationY, float rotationZ) {
    float cosX = cos(rotationX), sinX = sin(rotationX);
    float cosY = cos(rotationY), sinY = sin(rotationY);
    float cosZ = cos(rotationZ), sinZ = sin(rotationZ);

    float y1 = y * cosX - z * sinX;
    float z1 = y * sinX + z * cosX;
    y = y1;
    z = z1;

    float x1 = x * cosY + z * sinY;
    z1 = -x * sinY + z * cosY;
    x = x1;
    z = z1;

    x1 = x * cosZ - y * sinZ;
    y1 = x * sinZ + y * cosZ;
    x = x1;
    y = y1;
  }

float CalculateFaceDepth(const std::tuple<int, int, int>& face) {
    float depth = 0.0f;
    depth += std::get<2>(transformed_vertices[std::get<0>(face)]);
    depth += std::get<2>(transformed_vertices[std::get<1>(face)]);
    depth += std::get<2>(transformed_vertices[std::get<2>(face)]);
    return depth / 3.0f;
}

color CalculateLighting(const vec3d& normal) {
    vec3d light_direction = { 0.0f, 0.0f, -1.0f };
    float l = sqrtf(light_direction.x * light_direction.x + light_direction.y * light_direction.y + light_direction.z * light_direction.z);
    light_direction.x /= l;
    light_direction.y /= l;
    light_direction.z /= l;

    float dp = normal.x * light_direction.x + normal.y * light_direction.y + normal.z * light_direction.z;
    return color(dp * 255, dp * 255, dp * 255);
}


  void DrawIsoGrid() {
    const int originX = screenX + width / 2;
    const int originY = screenY + height / 2;
    const float gridSpacingX = width / (float)(gridSize + 1);
    const float gridSpacingY = height / (float)(gridSize + 1);

    auto isometric_projection = [originX, originY, gridSpacingX, gridSpacingY](float x, float y, float z) {
      int screenX = originX + (x - y) * gridSpacingX / 2;
      int screenY = originY + (x + y) * (gridSpacingY / 4) - z * gridSpacingY / 2;
      return std::make_pair(screenX, screenY);
    };

    auto rotate = [this](float& x, float& y, float& z) {
      float cosTheta = cos(gridRotationY);
      float sinTheta = sin(gridRotationY);

      x -= gridSize / 2.0f;
      y -= gridSize / 2.0f;
      z -= gridSize / 2.0f;

      float x_new = x * cosTheta - z * sinTheta;
      float z_new = x * sinTheta + z * cosTheta;
      x = x_new;
      z = z_new;

      float y_new1 = y * cosTheta - z * sinTheta;
      float z_new1 = y * sinTheta + z * cosTheta;
      y = y_new1;
      z = z_new1;

      float x_new2 = x * cosTheta - y * sinTheta;
      float y_new2 = x * sinTheta + y * cosTheta;
      x = x_new2;
      y = y_new2;

      x += gridSize / 2.0f;
      y += gridSize / 2.0f;
      z += gridSize / 2.0f;
    };

    for (int x = 0; x <= gridSize; x++) {
      for (int y = 0; y <= gridSize; y++) {
        float sx = x, sy = y, sz = 0;
        rotate(sx, sy, sz);
        auto [startX, startY] = isometric_projection(sx, sy, sz);
        if (x < gridSize) {
          float ex = x + 1, ey = y, ez = 0;
          rotate(ex, ey, ez);
          auto [endX1, endY1] = isometric_projection(ex, ey, ez);
          engine->DrawLine(startX, startY, endX1, endY1, DARK_GREEN);
        }
        if (y < gridSize) {
          float ex = x, ey = y + 1, ez = 0;
          rotate(ex, ey, ez);
          auto [endX2, endY2] = isometric_projection(ex, ey, ez);
          engine->DrawLine(startX, startY, endX2, endY2, DARK_GREEN);
        }
      }
    }

    for (int x = 0; x <= gridSize; x++) {
      for (int z = 0; z <= gridSize; z++) {
        float sx = x, sy = 0, sz = z;
        rotate(sx, sy, sz);
        auto [startX, startY] = isometric_projection(sx, sy, sz);
        if (x < gridSize) {
          float ex = x + 1, ey = 0, ez = z;
          rotate(ex, ey, ez);
          auto [endX1, endY1] = isometric_projection(ex, ey, ez);
          engine->DrawLine(startX, startY, endX1, endY1, DARK_RED);
        }
        if (z < gridSize) {
          float ex = x, ey = 0, ez = z + 1;
          rotate(ex, ey, ez);
          auto [endX2, endY2] = isometric_projection(ex, ey, ez);
          engine->DrawLine(startX, startY, endX2, endY2, DARK_RED);
        }
      }
    }

    for (int y = 0; y <= gridSize; y++) {
      for (int z = 0; z <= gridSize; z++) {
        float sx = 0, sy = y, sz = z;
        rotate(sx, sy, sz);
        auto [startX, startY] = isometric_projection(sx, sy, sz);
        if (y < gridSize) {
          float ex = 0, ey = y + 1, ez = z;
          rotate(ex, ey, ez);
          auto [endX1, endY1] = isometric_projection(ex, ey, ez);
          engine->DrawLine(startX, startY, endX1, endY1, DARK_BLUE);
        }
        if (z < gridSize) {
          float ex = 0, ey = y, ez = z + 1;
          rotate(ex, ey, ez);
          auto [endX2, endY2] = isometric_projection(ex, ey, ez);
          engine->DrawLine(startX, startY, endX2, endY2, DARK_BLUE);
        }
      }
    }

    float ax = 0, ay = 0, az = 0;
    rotate(ax, ay, az);
    auto [sx1, sy1] = isometric_projection(ax, ay, az);

    float bx = gridSize, by = 0, bz = 0;
    rotate(bx, by, bz);
    auto [sx2, sy2] = isometric_projection(bx, by, bz);

    float cx = 0, cy = gridSize, cz = 0;
    rotate(cx, cy, cz);
    auto [sx3, sy3] = isometric_projection(cx, cy, cz);

    float dx = 0, dy = 0, dz = gridSize;
    rotate(dx, dy, dz);
    auto [sx4, sy4] = isometric_projection(dx, dy, dz);

    engine->DrawLine(sx1, sy1, sx2, sy2, RED);
    engine->DrawLine(sx1, sy1, sx3, sy3, GREEN);
    engine->DrawLine(sx1, sy1, sx4, sy4, BLUE);
  }

void DrawModel() {
    const int originX = screenX + width / 2;
    const int originY = screenY + height / 2;
    const float gridSpacingX = width / (float)(gridSize + 1);
    const float gridSpacingY = height / (float)(gridSize + 1);

    auto isometric_projection = [originX, originY, gridSpacingX, gridSpacingY](float x, float y, float z) {
        int screenX = originX + (x - y) * gridSpacingX / 2;
        int screenY = originY + (x + y) * (gridSpacingY / 4) - z * gridSpacingY / 2;
        return std::make_pair(screenX, screenY);
    };

    // Sort faces by depth
    std::vector<std::tuple<int, int, int>> sortedFaces = faces;
    std::sort(sortedFaces.begin(), sortedFaces.end(), [this](const auto& face1, const auto& face2) {
        return CalculateFaceDepth(face1) > CalculateFaceDepth(face2);
    });

    for (const auto& face : sortedFaces) {
        float sx1 = std::get<0>(transformed_vertices[std::get<0>(face)]);
        float sy1 = std::get<1>(transformed_vertices[std::get<0>(face)]);
        float sz1 = std::get<2>(transformed_vertices[std::get<0>(face)]);
        auto [startX, startY] = isometric_projection(sx1, sy1, sz1);

        float sx2 = std::get<0>(transformed_vertices[std::get<1>(face)]);
        float sy2 = std::get<1>(transformed_vertices[std::get<1>(face)]);
        float sz2 = std::get<2>(transformed_vertices[std::get<1>(face)]);
        auto [endX, endY] = isometric_projection(sx2, sy2, sz2);

        float sx3 = std::get<0>(transformed_vertices[std::get<2>(face)]);
        float sy3 = std::get<1>(transformed_vertices[std::get<2>(face)]);
        float sz3 = std::get<2>(transformed_vertices[std::get<2>(face)]);
        auto [endX2, endY2] = isometric_projection(sx3, sy3, sz3);

        if (renderMode == RENDER_WIREFRAME) {
            engine->DrawLine(startX, startY, endX, endY, WHITE);
            engine->DrawLine(endX, endY, endX2, endY2, WHITE);
            engine->DrawLine(endX2, endY2, startX, startY, WHITE);
        } else if (renderMode == RENDER_SOLID) {
            vec3d normal, line1, line2;
            line1.x = sx2 - sx1;
            line1.y = sy2 - sy1;
            line1.z = sz2 - sz1;

            line2.x = sx3 - sx1;
            line2.y = sy3 - sy1;
            line2.z = sz3 - sz1;

            normal.x = line1.y * line2.z - line1.z * line2.y;
            normal.y = line1.z * line2.x - line1.x * line2.z;
            normal.z = line1.x * line2.y - line1.y * line2.x;

            float l = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
            normal.x /= l;
            normal.y /= l;
            normal.z /= l;

            color shade = CalculateLighting(normal);
            engine->FillTriangle(startX, startY, endX, endY, endX2, endY2, shade);
        }
    }
}


  void DrawModelOld() {
    const int originX = screenX + width / 2;
    const int originY = screenY + height / 2;
    const float gridSpacingX = width / (float)(gridSize + 1);
    const float gridSpacingY = height / (float)(gridSize + 1);

    auto isometric_projection = [originX, originY, gridSpacingX, gridSpacingY](float x, float y, float z) {
      int screenX = originX + (x - y) * gridSpacingX / 2;
      int screenY = originY + (x + y) * (gridSpacingY / 4) - z * gridSpacingY / 2;
      return std::make_pair(screenX, screenY);
    };

    for (const auto& face : faces) {
      for (size_t i = 0; i < faces.size(); i++) {
        float sx1 = std::get<0>(transformed_vertices[std::get<0>(face) - 1]);
        float sy1 = std::get<1>(transformed_vertices[std::get<0>(face) - 1]);
        float sz1 = std::get<2>(transformed_vertices[std::get<0>(face) - 1]);
        auto [startX, startY] = isometric_projection(sx1, sy1, sz1);

        float sx2 = std::get<0>(transformed_vertices[std::get<1>(face) - 1]);
        float sy2 = std::get<1>(transformed_vertices[std::get<1>(face) - 1]);
        float sz2 = std::get<2>(transformed_vertices[std::get<1>(face) - 1]);
        auto [endX, endY] = isometric_projection(sx2, sy2, sz2);

        engine->DrawLine(startX, startY, endX, endY, WHITE);
      }
    }

    float center_sx = modelTranslationX;
    float center_sy = modelTranslationY;
    float center_sz = modelTranslationZ;
    auto [centerX2D, centerY2D] = isometric_projection(center_sx, center_sy, center_sz);
    engine->FillCircle(centerX2D, centerY2D, 3, RED);
  }

private:
  IDrzEngine* engine;
  int screenX;
  int screenY;
  int width;
  int height;
  color backgroundColor;

  int maxFPS = 12;
  float rotation_speed = 0.1f;

  float gridRotationX = 0.0f;
  float gridRotationY = 0.0f;
  float gridRotationZ = 0.0f;
  float gridTranslationX = 0.0f;
  float gridTranslationY = 0.0f;
  float gridTranslationZ = 0.0f;
  int gridSize=5;

  float modelRotationX = 0.0f;
  float modelRotationY = 0.0f;
  float modelRotationZ = 0.0f;
  float modelTranslationX = 0.0f;
  float modelTranslationY = 0.0f;
  float modelTranslationZ = 0.0f;
  float modelScale = 1.0f;
  float scale_factor = 1.0f;  const float desired_width = 1.0f;
  
  float min_x = std::numeric_limits<float>::max();
  float max_x = std::numeric_limits<float>::min();
  float min_y = std::numeric_limits<float>::max();
  float max_y = std::numeric_limits<float>::min();
  float min_z = std::numeric_limits<float>::max();
  float max_z = std::numeric_limits<float>::min();

  std::vector<std::tuple<float, float, float>> vertices;
  std::vector<std::tuple<int, int, int>> faces;
  std::vector<std::tuple<float, float, float>> transformed_vertices;
};

#endif // ISOMETRICGRID3DRENDERER_H
