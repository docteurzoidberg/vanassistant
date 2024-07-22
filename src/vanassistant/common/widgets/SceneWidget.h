#pragma once

#include <IDrzEngine.h>

#include "../../Model.h"
#include "../../Widget.h"

#include <gfx3d.h>
#include <algorithm>
#include <vector>

using namespace drz;

enum RenderMode {
  RENDER_WIREFRAME,
  RENDER_SOLID
};

class SceneWidget : public Widget {
public:
  vec3d vCamera;
  Matrix4x4 matProj;

  float fFov = 90.0f;
  float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * M_PI);
  float fNear = 0.1f;
  float fFar = 1000.0f;
  float fAspectRatio = 1.0f; 

  RenderMode renderMode = RENDER_SOLID;

  SceneWidget(int x, int y, int width, int height) 
    : Widget(x, y, width, height), engine(DisplayPageManager::GetEngine()) {
    fAspectRatio = (float)width / (float)height;

    // Setup projection matrix
    matProj = Matrix4x4::Identity();
    matProj.m[0][0] = fAspectRatio * fFovRad;
    matProj.m[1][1] = fFovRad;
    matProj.m[2][2] = fFar / (fFar - fNear);
    matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
    matProj.m[2][3] = 1.0f;
    matProj.m[3][3] = 0.0f;
  }

  void AddModel(Model* model) {
    iSceneTriangleCount += model->tris.size();
    models.push_back(model);
  }

  void Update(float fElapsedTime) override{
    vecTrianglesToRaster.clear();

    for (auto model : models) {
      // Apply model rotations
      model->Update(fElapsedTime);

      for (auto tri : model->tris) {
        trianglec triProjected, triTranslated, triRotated;

        // Apply rotationMatrix instead of individual rotations
        auto rotationMatrix = model->rotationMatrix;
        Matrix4x4::MultiplyVector(rotationMatrix, *tri.p[0], triRotated.p[0]);
        Matrix4x4::MultiplyVector(rotationMatrix, *tri.p[1], triRotated.p[1]);
        Matrix4x4::MultiplyVector(rotationMatrix, *tri.p[2], triRotated.p[2]);

        // Apply translationMatrix instead of individual translations
        auto translationMatrix = model->translationMatrix;
        triTranslated = triRotated;

        triTranslated.p[0].x += translationMatrix.m[0][3];
        triTranslated.p[0].y += translationMatrix.m[1][3];
        triTranslated.p[0].z += translationMatrix.m[2][3];

        triTranslated.p[1].x += translationMatrix.m[0][3];
        triTranslated.p[1].y += translationMatrix.m[1][3];
        triTranslated.p[1].z += translationMatrix.m[2][3];

        triTranslated.p[2].x += translationMatrix.m[0][3];
        triTranslated.p[2].y += translationMatrix.m[1][3];
        triTranslated.p[2].z += translationMatrix.m[2][3];

        // Offset into the screen
        triTranslated.p[0].z = triTranslated.p[0].z + 3.0f;
        triTranslated.p[1].z = triTranslated.p[1].z + 3.0f;
        triTranslated.p[2].z = triTranslated.p[2].z + 3.0f;

        // Use Cross-Product to get surface normal
        vec3d normal, line1, line2;
        line1.x = triTranslated.p[1].x - triTranslated.p[0].x;
        line1.y = triTranslated.p[1].y - triTranslated.p[0].y;
        line1.z = triTranslated.p[1].z - triTranslated.p[0].z;

        line2.x = triTranslated.p[2].x - triTranslated.p[0].x;
        line2.y = triTranslated.p[2].y - triTranslated.p[0].y;
        line2.z = triTranslated.p[2].z - triTranslated.p[0].z;

        normal.x = line1.y * line2.z - line1.z * line2.y;
        normal.y = line1.z * line2.x - line1.x * line2.z;
        normal.z = line1.x * line2.y - line1.y * line2.x;

        // Normalize the normal
        float l = sqrtf(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
        normal.x /= l; normal.y /= l; normal.z /= l;

        if (normal.x * (triTranslated.p[0].x - vCamera.x) + 
            normal.y * (triTranslated.p[0].y - vCamera.y) +
            normal.z * (triTranslated.p[0].z - vCamera.z) < 0.0f) {
          // Illumination
          vec3d light_direction = { 0.0f, 0.0f, -1.0f };
          float l = sqrtf(light_direction.x*light_direction.x + light_direction.y*light_direction.y + light_direction.z*light_direction.z);
          light_direction.x /= l; light_direction.y /= l; light_direction.z /= l;

          // How similar is normal to light direction
          float dp = normal.x * light_direction.x + normal.y * light_direction.y + normal.z * light_direction.z;

          // Choose colors as required
          triTranslated.col = color(dp * 255, dp * 255, dp * 255);

          // Project triangles from 3D --> 2D
          Matrix4x4::MultiplyVector(matProj, triTranslated.p[0], triProjected.p[0]);
          Matrix4x4::MultiplyVector(matProj, triTranslated.p[1], triProjected.p[1]);
          Matrix4x4::MultiplyVector(matProj, triTranslated.p[2], triProjected.p[2]);
          triProjected.col = triTranslated.col;
          
          // Scale into view
          triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
          triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
          triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;
          triProjected.p[0].x *= 0.5f * (float)width;
          triProjected.p[0].y *= 0.5f * (float)height;
          triProjected.p[1].x *= 0.5f * (float)width;
          triProjected.p[1].y *= 0.5f * (float)height;
          triProjected.p[2].x *= 0.5f * (float)width;
          triProjected.p[2].y *= 0.5f * (float)height;

          // Store triangle for sorting
          vecTrianglesToRaster.push_back(triProjected);
        }
      }
    }

    // Sort triangles from back to front
    sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](trianglec &t1, trianglec &t2) {
      float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
      float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
      return z1 > z2;
    });
  }

  void Render() override {
    // Loop through all transformed, viewed, projected, and sorted triangles
    for (auto &t : vecTrianglesToRaster) {
      // Clip triangles to the render area
      ClipAndDrawTriangle(t);
    }
  }

  void Clear() {
    models.clear();
  }     

  // Toggle debug mode
  void ToggleDebugTriangles() {
    bDebugTriangles = !bDebugTriangles;
  }

  void ToggleRenderMode() {
    renderMode = (renderMode == RENDER_SOLID) ? RENDER_WIREFRAME : RENDER_SOLID;
  }

  // Show next triangle (Debug)
  void DbgNextTriangle() {
    iDbgTriangleIndex++;
    if (iDbgTriangleIndex >= iSceneTriangleCount) {
      iDbgTriangleIndex = 0;
    }
  }

  // Show previous triangle (Debug)
  void DbgPrevTriangle() {
    iDbgTriangleIndex--;
    if (iDbgTriangleIndex < 0) {
      iDbgTriangleIndex = iSceneTriangleCount - 1;
    }
  }

private:
  IDrzEngine* engine;
  std::vector<Model*> models;
  std::vector<trianglec> vecTrianglesToRaster;
  int iSceneTriangleCount = 0;
  bool bDebugTriangles = false;
  int iDbgTriangleIndex = 0;

  void ClipAndDrawTriangle(const trianglec& t) {
    if (renderMode == RENDER_WIREFRAME) {
      ClipAndDrawLine(t.p[0].x, t.p[0].y, t.p[1].x, t.p[1].y, WHITE);
      ClipAndDrawLine(t.p[1].x, t.p[1].y, t.p[2].x, t.p[2].y, WHITE);
      ClipAndDrawLine(t.p[2].x, t.p[2].y, t.p[0].x, t.p[0].y, WHITE);
    } else {
      ClipAndFillTriangle(t.p[0].x, t.p[0].y, t.p[1].x, t.p[1].y, t.p[2].x, t.p[2].y, t.col);
    }
  }

  void ClipAndDrawLine(int x1, int y1, int x2, int y2, color col) {
    // Clip and draw line within the render area
    if (ClipLine(x1, y1, x2, y2)) {
      engine->DrawLine(x1 + screenX, y1 + screenY, x2 + screenX, y2 + screenY, col);
    }
  }

  bool ClipLine(int& x1, int& y1, int& x2, int& y2) {
    // Liang-Barsky line clipping algorithm
    float t0 = 0.0f, t1 = 1.0f;
    int dx = x2 - x1, dy = y2 - y1;
    float p[4], q[4];
    p[0] = -dx; q[0] = x1 - screenX;
    p[1] = dx;  q[1] = screenX + width - x1;
    p[2] = -dy; q[2] = y1 - screenY;
    p[3] = dy;  q[3] = screenY + height - y1;

    for (int i = 0; i < 4; i++) {
      if (p[i] == 0 && q[i] < 0) return false;
      float r = q[i] / p[i];
      if (p[i] < 0) {
        if (r > t1) return false;
        if (r > t0) t0 = r;
      } else if (p[i] > 0) {
        if (r < t0) return false;
        if (r < t1) t1 = r;
      }
    }
    if (t0 > 0) { x1 += t0 * dx; y1 += t0 * dy; }
    if (t1 < 1) { x2 += t1 * dx; y2 += t1 * dy; }
    return true;
  }

  void ClipAndFillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, color col) {
    // Implement Sutherland-Hodgman polygon clipping algorithm
    std::vector<vec3d> vertices = { {float(x1), float(y1), 0}, {float(x2), float(y2), 0}, {float(x3), float(y3), 0} };
    std::vector<vec3d> clippedVertices;

    ClipPolygonToEdge(vertices, clippedVertices, 1, 0); // Left
    vertices = clippedVertices; clippedVertices.clear();
    ClipPolygonToEdge(vertices, clippedVertices, -1, width); // Right
    vertices = clippedVertices; clippedVertices.clear();
    ClipPolygonToEdge(vertices, clippedVertices, 1, 0); // Top
    vertices = clippedVertices; clippedVertices.clear();
    ClipPolygonToEdge(vertices, clippedVertices, -1, height); // Bottom

    if (clippedVertices.size() < 3) return;

    for (size_t i = 1; i < clippedVertices.size() - 1; i++) {
      engine->FillTriangle(
        clippedVertices[0].x + screenX, clippedVertices[0].y + screenY,
        clippedVertices[i].x + screenX, clippedVertices[i].y + screenY,
        clippedVertices[i + 1].x + screenX, clippedVertices[i + 1].y + screenY,
        col);
    }
  }

  void ClipPolygonToEdge(const std::vector<vec3d>& vertices, std::vector<vec3d>& clippedVertices, int edge, float boundary) {
    vec3d prevVertex = vertices.back();
    for (const auto& vertex : vertices) {
      if (Inside(vertex, edge, boundary)) {
        if (!Inside(prevVertex, edge, boundary)) {
          clippedVertices.push_back(Intersect(prevVertex, vertex, edge, boundary));
        }
        clippedVertices.push_back(vertex);
      } else if (Inside(prevVertex, edge, boundary)) {
        clippedVertices.push_back(Intersect(prevVertex, vertex, edge, boundary));
      }
      prevVertex = vertex;
    }
  }

  bool Inside(const vec3d& vertex, int edge, float boundary) {
    if (edge == 1) return vertex.x >= boundary;
    if (edge == -1) return vertex.x <= boundary;
    if (edge == 2) return vertex.y >= boundary;
    if (edge == -2) return vertex.y <= boundary;
    return false;
  }

  vec3d Intersect(const vec3d& v1, const vec3d& v2, int edge, float boundary) {
    float t;
    if (edge == 1 || edge == -1) {
      t = (boundary - v1.x) / (v2.x - v1.x);
      return { boundary, v1.y + t * (v2.y - v1.y), 0 };
    } else {
      t = (boundary - v1.y) / (v2.y - v1.y);
      return { v1.x + t * (v2.x - v1.x), boundary, 0 };
    }
  }
};

