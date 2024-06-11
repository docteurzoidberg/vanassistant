#pragma once

#include <algorithm>
#include <iostream>
#include <list>
#include <vector>

#include "Model.h"

enum RenderMode {
  RENDER_WIREFRAME,
  RENDER_SOLID
};

class Scene {

  public:

    vec3d vCamera;
    mat4x4 matProj;

    float fFov = 90.0f;
    float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * M_PI);
    float fNear = 0.1f;
    float fFar = 1000.0f;
    float fAspectRatio = 1.0f;
    //float fTheta =  2* 3.14159f;

    RenderMode renderMode = RENDER_SOLID;

    Scene(IDrzEngine* engine) : engine(engine) {
      
      fAspectRatio = (float)engine->GetScreenWidth() / (float)engine->GetScreenHeight();

      //setup projection matrix
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

    void MultiplyMatrixVector(vec3d &i, vec3d &o, mat4x4 &m) {
      o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
      o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
      o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
      float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];
      if (w != 0.0f)
      {
        o.x /= w; o.y /= w; o.z /= w;
      }
    }
    
    void MultiplyMatrixVector(vec3d &i, vec3d &o, Matrix4x4 &m) {
      o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
      o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
      o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
      float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];
      if (w != 0.0f)
      {
        o.x /= w; o.y /= w; o.z /= w;
      }
    }

    void Update(float fElapsedTime) {

      vecTrianglesToRaster.clear();

      for (auto model : models) {

        //Apply model rotations
        model->Update(fElapsedTime);

        for (auto tri : model->tris) {

          //trianglec triProjected, triTranslated, triRotatedZ, triRotatedZX;
          trianglec triProjected, triTranslated, triRotated;

          // Rotate in Z-Axis
          //MultiplyMatrixVector(*tri.p[0], triRotatedZ.p[0], model->matRotZ);
         // MultiplyMatrixVector(*tri.p[1], triRotatedZ.p[1], model->matRotZ);
          //MultiplyMatrixVector(*tri.p[2], triRotatedZ.p[2], model->matRotZ);

          // Rotate in X-Axis
          //MultiplyMatrixVector(triRotatedZ.p[0], triRotatedZX.p[0], model->matRotX);
          //MultiplyMatrixVector(triRotatedZ.p[1], triRotatedZX.p[1], model->matRotX);
          //MultiplyMatrixVector(triRotatedZ.p[2], triRotatedZX.p[2], model->matRotX);

          //refacto: Apply rotationMatrix instead of individual rotations
          auto rotationMatrix = model->rotationMatrix;
          MultiplyMatrixVector(*tri.p[0], triRotated.p[0], rotationMatrix);
          MultiplyMatrixVector(*tri.p[1], triRotated.p[1], rotationMatrix);
          MultiplyMatrixVector(*tri.p[2], triRotated.p[2], rotationMatrix);

          //refacto: Apply translationMatrix instead of individual translations
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
          //triTranslated = triRotatedZX;
          //triTranslated.p[0].z = triRotatedZX.p[0].z + 3.0f;
          //triTranslated.p[1].z = triRotatedZX.p[1].z + 3.0f;
          //triTranslated.p[2].z = triRotatedZX.p[2].z + 3.0f;

          //Offset into the screen
          triTranslated.p[0].z = triRotated.p[0].z + 3.0f;
          triTranslated.p[1].z = triRotated.p[1].z + 3.0f;
          triTranslated.p[2].z = triRotated.p[2].z + 3.0f;


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

          // It's normally normal to normalise the normal
          float l = sqrtf(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
          normal.x /= l; normal.y /= l; normal.z /= l;

          //if (normal.z < 0)
          if(normal.x * (triTranslated.p[0].x - vCamera.x) + 
            normal.y * (triTranslated.p[0].y - vCamera.y) +
            normal.z * (triTranslated.p[0].z - vCamera.z) < 0.0f)
          {
            // Illumination
            vec3d light_direction = { 0.0f, 0.0f, -1.0f };
            float l = sqrtf(light_direction.x*light_direction.x + light_direction.y*light_direction.y + light_direction.z*light_direction.z);
            light_direction.x /= l; light_direction.y /= l; light_direction.z /= l;

            // How similar is normal to light direction
            float dp = normal.x * light_direction.x + normal.y * light_direction.y + normal.z * light_direction.z;

            // Choose console colours as required (much easier with RGB)
            triTranslated.col = color(dp * 255, dp * 255, dp * 255);

            // Project triangles from 3D --> 2D
            MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
            MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProj);
            MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProj);
            triProjected.col = triTranslated.col;
            
            // Scale into view
            triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
            triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
            triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;
            triProjected.p[0].x *= 0.5f * (float)engine->GetScreenWidth();
            triProjected.p[0].y *= 0.5f * (float)engine->GetScreenHeight();
            triProjected.p[1].x *= 0.5f * (float)engine->GetScreenWidth();
            triProjected.p[1].y *= 0.5f * (float)engine->GetScreenHeight();
            triProjected.p[2].x *= 0.5f * (float)engine->GetScreenWidth();
            triProjected.p[2].y *= 0.5f * (float)engine->GetScreenHeight();

            // Store triangle for sorting
            vecTrianglesToRaster.push_back(triProjected);
          }
        }
      }

      // Sort triangles from back to front
      sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](trianglec &t1, trianglec &t2)
      {
        float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
        float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
        return z1 > z2;
      });

      //std::cout << "Scene updated, triangles: " << vecTrianglesToRaster.size() << std::endl;
      //std::cout << "Yaw: " << fYaw << " Camera: {" << vCamera.x << ", " << vCamera.y << ", " << vCamera.z << "} LookDir: {" << vLookDir.x << ", " << vLookDir.y << ", " << vLookDir.z << "}" << std::endl;
    }

    void Render() {
     	// Loop through all transformed, viewed, projected, and sorted triangles
      for (auto &t : vecTrianglesToRaster) {
        // Draw the transformed, viewed, clipped, projected, sorted, clipped triangles
        if (renderMode == RENDER_WIREFRAME) {
          engine->DrawLine(t.p[0].x, t.p[0].y, t.p[1].x, t.p[1].y, t.col);
          engine->DrawLine(t.p[1].x, t.p[1].y, t.p[2].x, t.p[2].y, t.col);
          engine->DrawLine(t.p[2].x, t.p[2].y, t.p[0].x, t.p[0].y, t.col);
        } else {
          engine->FillTriangle({t.p[0].x, t.p[0].y}, {t.p[1].x, t.p[1].y}, {t.p[2].x, t.p[2].y}, t.col);
        }
      }
    }

    void Clear() {
      models.clear();
    }     

    /// Toggle debug mode
    void ToggleDebugTriangles() {
      bDebugTriangles = !bDebugTriangles;
    }

    void ToggleRenderMode() {
      if (renderMode == RENDER_SOLID) {
        renderMode = RENDER_WIREFRAME;
      } else {
        renderMode = RENDER_SOLID;
      }
    }

    /// Show next triangle (Debug)
    void DbgNextTriangle() {
      iDbgTriangleIndex++;
      if (iDbgTriangleIndex >= iSceneTriangleCount) {
        iDbgTriangleIndex = 0;
      }
    }
    /// Show previous triangle (Debug)
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
    bool iSceneTriangleCount = 0;
    bool bDebugTriangles = false;
    int iDbgTriangleIndex = 0;
};