#pragma once

#include <IDrzEngine.h>

#include <gfx3d.h> 

#include "Model.h"

#include <algorithm>
#include <vector>

enum RenderMode {
  RENDER_WIREFRAME,
  RENDER_SOLID
};

using namespace drz;

class Scene {

  public:

    vec3d vLookDir = { 0,0,1 };
    vec3d vCamera;
    Matrix4x4 matProj;

    float fFov = 90.0f;
    float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * M_PI);
    float fNear = 0.1f;
    float fFar = 1000.0f;
    float fAspectRatio = 1.0f;

    RenderMode renderMode = RENDER_SOLID;

    Scene(IDrzEngine* engine) : engine(engine) {
      
      fAspectRatio = (float)engine->GetScreenWidth() / (float)engine->GetScreenHeight();

      //setup projection matrix
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
    
    vec3d Vector_Add(vec3d &v1, vec3d &v2)
    {
      return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
    }

    void Update(float fElapsedTime) {

      vecTrianglesToRaster.clear();

      for (auto model : models) {

        //Apply model rotations
        model->Update(fElapsedTime);

        // Create World Matrix
        Matrix4x4 matWorld = Matrix4x4::Identity();  
        matWorld = model->rotationMatrix * model->translationMatrix;
      
        // Create "Point At" Matrix for camera
        vec3d vUp = { 0,1,0 };
        vec3d vTarget = { 0,0,1 };
        Matrix4x4 matCameraRot = Matrix4x4::CreateRotationMatrixY(fCameraYaw);
        
        Matrix4x4::MultiplyVector(matCameraRot, vTarget, vLookDir);
        vTarget = Vector_Add(vCamera, vLookDir);
        Matrix4x4 matCamera = Matrix4x4::PointAt(vCamera, vTarget, vUp);

        // Make view matrix from camera
        Matrix4x4 matView = Matrix4x4::QuickInverse(matCamera);

  
        // Draw Triangles
        for (auto tri : model->tris) {
          trianglec triProjected, triTransformed, triViewed;

          // World Matrix Transform
          Matrix4x4::MultiplyVector(matWorld, *tri.p[0],  triTransformed.p[0]);
          Matrix4x4::MultiplyVector(matWorld, *tri.p[1],  triTransformed.p[1]);
          Matrix4x4::MultiplyVector(matWorld, *tri.p[2],  triTransformed.p[2]);

          // Calculate triangle Normal
          vec3d normal, line1, line2;

          // Get lines either side of triangle
          line1 = Vector::Sub(triTransformed.p[1], triTransformed.p[0]);
          line2 = Vector::Sub(triTransformed.p[2], triTransformed.p[0]);

          // Take cross product of lines to get normal to triangle surface
          normal = Vector::CrossProduct(line1, line2);

          // You normally need to normalise a normal!
          normal = Vector::Normalise(normal);
          
          // Get Ray from triangle to camera
          vec3d vCameraRay = Vector::Sub(triTransformed.p[0], vCamera);

          // If ray is aligned with normal, then triangle is visible
          if (Vector::DotProduct(normal, vCameraRay) < 0.0f) {
            // Illumination
            vec3d light_direction = { 0.0f, 1.0f, -1.0f };
            light_direction = Vector::Normalise(light_direction);

            // How "aligned" are light direction and triangle surface normal?
            float dp = std::max(0.1f, Vector::DotProduct(light_direction, normal));

            // Choose console colours as required (much easier with RGB)
            color col = color(255 * dp, 255 * dp, 255 * dp);

            triTransformed.col = col;

            // Convert World Space --> View Space
            Matrix4x4::MultiplyVector(matView, triTransformed.p[0], triViewed.p[0]);
            Matrix4x4::MultiplyVector(matView, triTransformed.p[1], triViewed.p[1]);
            Matrix4x4::MultiplyVector(matView, triTransformed.p[2], triViewed.p[2]);
            triViewed.col = triTransformed.col;
            
            // Clip Viewed Triangle against near plane, this could form two additional
            // additional triangles. 
            int nClippedTriangles = 0;
            trianglec clipped[2];
            nClippedTriangles = Triangle::ClipAgainstPlane({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f }, triViewed, clipped[0], clipped[1]);

            // We may end up with multiple triangles form the clip, so project as
            // required
            for (int n = 0; n < nClippedTriangles; n++) {
              // Project triangles from 3D --> 2D
              Matrix4x4::MultiplyVector(matProj, clipped[n].p[0],triProjected.p[0]);
              Matrix4x4::MultiplyVector(matProj, clipped[n].p[1],triProjected.p[1]);
              Matrix4x4::MultiplyVector(matProj, clipped[n].p[2],triProjected.p[2]);
              triProjected.col = clipped[n].col;
              //triProjected.t[0] = clipped[n].t[0];
              //triProjected.t[1] = clipped[n].t[1];
              //t//riProjected.t[2] = clipped[n].t[2];


              //triProjected.t[0].u = triProjected.t[0].u / triProjected.p[0].w;
              //triProjected.t[1].u = triProjected.t[1].u / triProjected.p[1].w;
              //triProjected.t[2].u = triProjected.t[2].u / triProjected.p[2].w;

              //triProjected.t[0].v = triProjected.t[0].v / triProjected.p[0].w;
              //triProjected.t[1].v = triProjected.t[1].v / triProjected.p[1].w;
              //triProjected.t[2].v = triProjected.t[2].v / triProjected.p[2].w;

              //triProjected.t[0].w = 1.0f / triProjected.p[0].w;
              //triProjected.t[1].w = 1.0f / triProjected.p[1].w;
              //triProjected.t[2].w = 1.0f / triProjected.p[2].w;


              // Scale into view, we moved the normalising into cartesian space
              // out of the matrix.vector function from the previous videos, so
              // do this manually
              triProjected.p[0] = Vector::Div(triProjected.p[0], triProjected.p[0].w);
              triProjected.p[1] = Vector::Div(triProjected.p[1], triProjected.p[1].w);
              triProjected.p[2] = Vector::Div(triProjected.p[2], triProjected.p[2].w);

              // X/Y are inverted so put them back
              triProjected.p[0].x *= -1.0f;
              triProjected.p[1].x *= -1.0f;
              triProjected.p[2].x *= -1.0f;
              triProjected.p[0].y *= -1.0f;
              triProjected.p[1].y *= -1.0f;
              triProjected.p[2].y *= -1.0f;

              // Offset verts into visible normalised space
              vec3d vOffsetView = { 1,1,0 };
              triProjected.p[0] = Vector::Add(triProjected.p[0], vOffsetView);
              triProjected.p[1] = Vector::Add(triProjected.p[1], vOffsetView);
              triProjected.p[2] = Vector::Add(triProjected.p[2], vOffsetView);
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


      }


      // Sort triangles from back to front
      sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](trianglec &t1, trianglec &t2)
      {
        float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
        float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
        return z1 > z2;
      });

      std::cout << "Scene updated, triangles: " << vecTrianglesToRaster.size() << std::endl;
      std::cout << "Yaw: " << fCameraYaw << " Camera: {" << vCamera.x << ", " << vCamera.y << ", " << vCamera.z << "} LookDir: {" << vLookDir.x << ", " << vLookDir.y << ", " << vLookDir.z << "}" << std::endl;
    }

    void Render() {
     	// Loop through all transformed, viewed, projected, and sorted triangles
      for (auto &t : vecTrianglesToRaster) {
        // Draw the transformed, viewed, clipped, projected, sorted, clipped triangles
        if (renderMode == RENDER_WIREFRAME) {
          engine->DrawLine(t.p[0].x, t.p[0].y, t.p[1].x, t.p[1].y, drz::WHITE);
          engine->DrawLine(t.p[1].x, t.p[1].y, t.p[2].x, t.p[2].y, drz::WHITE);
          engine->DrawLine(t.p[2].x, t.p[2].y, t.p[0].x, t.p[0].y, drz::WHITE);
        } else {
          engine->FillTriangle(t.p[0].x, t.p[0].y, t.p[1].x, t.p[1].y, t.p[2].x, t.p[2].y, t.col);
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

    // Camera
    float fCameraYaw = 0.0f;

};