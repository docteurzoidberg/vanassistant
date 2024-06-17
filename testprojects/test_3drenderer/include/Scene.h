#pragma once

#include <IDrzEngine.h>

#define OLC_PGEX_GRAPHICS3D
#include <gfx3d.h> 

#include "Model.h"

#include <vector>

enum RenderMode {
  RENDER_WIREFRAME,
  RENDER_SOLID
};

using namespace drz;

class Scene {

  public:

    float fFovDegrees = 90.0f;
    
    float fNear = 0.1f;
    float fFar = 1000.0f;
    float fLeft = 0.0f;
    float fTop = 0.0f; 
    float fWidth;
    float fHeight;
    float fAspectRatio;

    GFX3D::PipeLine renderer;

    GFX3D::vec3d vUp = { 0, 1, 0 };
    GFX3D::vec3d vLookDir = { 0, 0,-1 };
    GFX3D::vec3d vSun = {0, 0, -1};
    GFX3D::vec3d vForward;

    RenderMode renderMode = RENDER_SOLID;

    Scene(IDrzEngine* engine, float left, float top, float width, float height) : 
      engine(engine),
      fLeft(left),
      fTop(top), 
      fWidth(width), 
      fHeight(height) 
    {
      fAspectRatio = fWidth / fHeight;
      GFX3D::ConfigureDisplay();
      renderer.SetProjection( fFovDegrees, fAspectRatio, fNear, fFar, fLeft, fTop, fWidth, fHeight);
    }

    void AddModel(Model* model) {
      iSceneTriangleCount += model->tris.size();
      models.push_back(model);
    }

    void Update(float fElapsedTime, GFX3D::vec3d vCamera, float fYaw, float fTheta) { 

      vForward = GFX3D::Math::Vec_Mul(vLookDir, 8.0f * fElapsedTime);

      for (auto model : models) {

        //Apply model rotations
        model->Update(fElapsedTime);

        GFX3D::mat4x4 matRotateY =GFX3D::Math::Mat_MakeRotationY(fTheta); //TODO if i want model rotation

        GFX3D::mat4x4 matTrans;
        matTrans = GFX3D::Math::Mat_MakeTranslation(0.0f, 0.0f, 0.0f); //no need translating for now

        GFX3D::mat4x4 matWorld;
        matWorld = GFX3D::Math::Mat_MakeIdentity();;	// Form World Matrix
        matWorld = GFX3D::Math::Mat_MultiplyMatrix(matWorld, matRotateY); // Transform by rotation
        matWorld = GFX3D::Math::Mat_MultiplyMatrix(matWorld, matTrans); // Transform by translation

        GFX3D::vec3d vTarget = { 0,0,1 };
        GFX3D::mat4x4 matCameraRot = GFX3D::Math::Mat_MakeRotationY(fYaw); //TODO if i want camera rotation
        vLookDir = GFX3D::Math::Mat_MultiplyVector(matCameraRot, vTarget);
        vTarget = GFX3D::Math::Vec_Add(vCamera, vLookDir);
        
        renderer.SetCamera(vCamera, vTarget, vUp);

        GFX3D::ClearDepth();
        renderer.SetTransform(matWorld);
        renderer.SetLightSource(1, GFX3D::LIGHTS::LIGHT_DIRECTIONAL, drz::WHITE, vSun);
        if(renderMode == RENDER_WIREFRAME) {
          renderer.Render(model->tris, GFX3D::RENDERFLAGS::RENDER_WIRE);
        }
        else if(renderMode == RENDER_SOLID) {
          renderer.Render(model->tris, GFX3D::RENDERFLAGS::RENDER_FLAT|GFX3D::RENDERFLAGS::RENDER_DEPTH|GFX3D::RENDERFLAGS::RENDER_LIGHTS);
        }
      }
      //std::cout << "Light Direction: " << vSun.x << ", " << vSun.y << ", " << vSun.z << std::endl;
      //std::cout << "Camera Position: " << vCamera.x << ", " << vCamera.y << ", " << vCamera.z << std::endl;
    }

    void Render() {
     

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

    bool iSceneTriangleCount = 0;
    bool bDebugTriangles = false;
    int iDbgTriangleIndex = 0;

    // Camera
    float fCameraYaw = 0.0f;
};