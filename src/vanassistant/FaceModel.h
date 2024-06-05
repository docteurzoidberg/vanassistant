#pragma once

#include "../../include/IDrzEngine.h"

#include "Model.h"
#include "Animated3DObject.h"
#include <cmath>
#include <iostream>

//Mouth part animations
class MouthPart : public Animated3DObject {
  public:

    //Animation keys
    enum KEY_FRAME {
      OPEN,
      CLOSE
    };

    MouthPart(IDrzEngine* engine, Model* model) : Animated3DObject(engine, model) { 
      //associate the vertices of the mouth with the corresponding triangles + point indexes
      // clang-format off
      tps = {
        {7,1},  //=v[17]
        {9,1},  //=v[16]
        {9,2},  //=v[15]
        {12,0}, //=v[18]
        {12,2}, //=v[56]
        {35,0}, //=v[46] 
        {36,0}, //=v[45]
        {33,1}, //=v[44]
        {47,1}, //=v[43]
      };
      //keyframe => vector<points>
      kf_points = {
        //TODO: real points for each frame
        //KEY_OPEN
        {
          {-0.547272, -0.677873, 0.391896},   //=v[17]
          {-0.310748, -0.546415, 0.579291},   //=v[16]
          {-0.082378, -0.539592, 0.691661},   //=v[15]
          {-0.20804, -0.78771, 0.587208},     //=v[18]
          {0.004478, -0.790754, 0.641731},    //=v[56]
          {0.216484, -0.78771, 0.587208},     //=v[46]
          {0.555716, -0.677873, 0.391896},    //=v[45]
          {0.319192, -0.546415, 0.579291},    //=v[44]
          {0.090822, -0.539592, 0.691661},    //=v[43]
        },
        //KEY_CLOSE
        {
          {-0.547272,-0.677873,0.391896},     //=v[17]
          {-0.310748,-0.6632,0.579291},       //=v[16]
          {-0.082378,-0.6447,0.691661},       //=v[15]
          {-0.20804,-0.72056,0.587208},       //=v[18]
          {0.004478,-0.71485,0.641731},       //=v[56]
          {0.216484,-0.72056,0.587208},       //=v[46]
          {0.555716,-0.677873,0.391896},      //=v[45]
          {0.319192,-0.66319,0.579291},       //=v[44]
          {0.090822,-0.6476,0.691661},        //=v[43]
        },
      }; 
      // clang-format on
      //loads tps into refs to the actual vertices
      load(); 
    }
};
 
//3d face model including animated subparts
class FaceModel : public Model {
  public:
    // clang-format off 
    //Hardcoded face model vertices
    std::vector<vec3d> verts = {
      {-0.123703, 0.001472, 0.952351},
      {-0.12059, -0.184893, 0.858286},
      {-0.096589, 0.851554, 0.378855},
      {-0.308867, 0.098008, 0.460455},
      {-0.313299, 1.02628, 0.411342},
      {-0.226048, 0.6784, 0.218026},
      {-1.03587, 0.990863, 0.116014},
      {-1.11066, 0.105018, 0.048242},
      {-0.080185, -0.31292, 0.566291},
      {-0.378635, -0.048315, 0.421444},
      {-0.304947, -0.225835, 0.473946},
      {-1.16352, 0.643507, -0.243753},
      {-1.08676, 1.641, -0.353627},
      {-0.490549, 1.88597, 0.038541},
      {-0.903636, -0.562408, -0.039296},

      //kp: 9-2. mouth [2]
      {-0.082378, -0.539592, 0.691661},

      //kp: 9-1. mouth [1]
      {-0.310748, -0.546415, 0.579291},
      
      //kp: 7-1. mouth [0]
      {-0.547272, -0.677873, 0.391896},

      //kp: 12-0 mouth [3]
      {-0.20804, -0.78771, 0.587208},

      {-0.272745, -0.967215, 0.46947},
      {-0.294976, -1.24988, 0.572406},
      {-0.530438, 0.564415, 0.182007},
      {-0.514957, 0.818628, 0.145485},
      {-0.355678, 0.667006, 0.118644},
      {-0.806416, 0.803185, 0.085337},
      {-0.793271, 0.56203, 0.112307},
      {-0.893837, 0.679656, 0.023895},
      {-0.563347, -1.1456, 0.273962},
      {0.132147, 0.001472, 0.952351},
      {0.129034, -0.184893, 0.858286},
      {0.105033, 0.851554, 0.378855},
      {0.317311, 0.098008, 0.460455},
      {0.321743, 1.02628, 0.411342},
      {0.234492, 0.6784, 0.218026},
      {1.04431, 0.990863, 0.116014},
      {1.11911, 0.105018, 0.048242},
      {0.088628, -0.31292, 0.566291},
      {0.387079, -0.048315, 0.421444},
      {0.313391, -0.225835, 0.473946},
      {1.17196, 0.643507, -0.243753},
      {1.09161, 1.63217, -0.347511},
      {0.498993, 1.88597, 0.038541},
      {0.91208, -0.562408, -0.039296},

      //kp: 47-1 mouth[8]
      {0.090822, -0.539592, 0.691661},

      //kp:33-1 mouth [7]
      {0.319192, -0.546415, 0.579291},

      //kp:36-0 mouth [6]
      {0.555716, -0.677873, 0.391896},

      //kp:35-0 mouth [5]
      {0.216484, -0.78771, 0.587208},

      {0.281189, -0.967215, 0.46947},
      {0.30342, -1.24988, 0.572406},
      {0.538881, 0.564415, 0.182007},
      {0.523401, 0.818628, 0.145485},
      {0.364122, 0.667006, 0.118644},
      {0.81486, 0.803185, 0.085337},
      {0.801715, 0.56203, 0.112307},
      {0.903642, 0.679152, 0.021932},
      {0.571791, -1.1456, 0.273962},

      //kp: 12-2 mouth [4]
      {0.004478, -0.790754, 0.641731},
    };
    //Hardcoded face model faces
    std::vector<face> faces = {
      {6, 3, 5},
      {27, 8, 26},
      {7, 14, 13},
      {6, 22, 4},
      {7, 13, 12},
      {9, 2, 11},
      {8, 10, 4},
      {15, 18, 10},
      {10, 2, 1},
      {9, 17, 16},
      {11, 18, 17},
      {18, 15, 28},
      {19, 20, 57},
      {18, 20, 19},
      {18, 11, 10},
      {7, 23, 5},
      {7, 27, 25},
      {26, 4, 22},
      {23, 6, 5},
      {20, 18, 28},
      {21, 20, 28},
      {1, 4, 10},
      {3, 4, 1},
      {34, 33, 31},
      {36, 55, 54},
      {35, 42, 33},
      {50, 34, 32},
      {35, 40, 41},
      {37, 39, 30},
      {38, 36, 32},
      {43, 38, 46},
      {30, 38, 29},
      {37, 45, 39},
      {39, 45, 46},
      {46, 56, 43},
      {47, 57, 48},
      {46, 47, 48},
      {20, 48, 57},
      {46, 38, 39},
      {51, 35, 33},
      {35, 55, 40},
      {32, 54, 50},
      {51, 34, 52},
      {48, 56, 46},
      {49, 56, 48},
      {29, 38, 32},
      {32, 31, 29},
      {9, 44, 37},
      {2, 37, 30},
      {30, 1, 2},
      {29, 3, 1},
      {3, 33, 5},
      {33, 14, 5},
      {20, 49, 48},
      {27, 12, 8},
      {7, 5, 14},
      {6, 24, 22},
      {8, 15, 10},
      {10, 11, 2},
      {9, 11, 17},
      {7, 25, 23},
      {7, 12, 27},
      {26, 8, 4},
      {23, 24, 6},
      {3, 6, 4},
      {36, 40, 55},
      {35, 41, 42},
      {50, 52, 34},
      {38, 43, 36},
      {30, 39, 38},
      {37, 44, 45},
      {51, 53, 35},
      {35, 53, 55},
      {32, 36, 54},
      {51, 33, 34},
      {32, 34, 31},
      {9, 16, 44},
      {2, 9, 37},
      {30, 29, 1},
      {29, 31, 3},
      {3, 31, 33},
      {33, 42, 14},
      {20, 21, 49},
    };
    // clang-format on

    MouthPart* mouth;

    FaceModel(IDrzEngine* engine) : engine(engine), Model(&verts, &faces)  {

      fAspectRatio = (float)engine->GetScreenWidth() / (float)engine->GetScreenHeight();
      LoadModel();

      //Animated parts
      mouth = new MouthPart(engine, this);

      //TODO
      //lefteye=...
      //righteye=...

      // Projection Matrix
      float fNear = 0.1f;
      float fFar = 1000.0f;
      float fFov = 90.0f;
      
      //fAspectRatio = (float)engine->ScreenHeight() / (float)ScreenWidth();
      float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

      matProj.m[0][0] = fAspectRatio * fFovRad;
      matProj.m[1][1] = fFovRad;
      matProj.m[2][2] = fFar / (fFar - fNear);
      matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
      matProj.m[2][3] = 1.0f;
      matProj.m[3][3] = 0.0f;
    }

    void Update(float elapsedTime) {

      //sub parts update
      mouth->Update();

      fTheta += 0.001f; //1.0f * fElapsedTime;
    }

    void Render() {

      // Set up rotation matrices
      mat4x4 matRotZ, matRotX;

      //engine->DrawTriangle({0,0}, {0, (float)engine->GetScreenWidth()}, {(float)engine->GetScreenWidth(),(float)engine->GetScreenHeight()}, BLUE); 
      //

      if(bDebugTriangles) {
        auto text = "tri: " + std::to_string(iDbgTriangleIndex);
        //TODO
        //auto text_size   = pixelFont48->GetTextSizeProp( text );
        //auto text_centre      = text_size / 2.0f;
        //auto fScale                 = 1.0f;
        //pixelFont48->DrawStringPropDecal( {0,(float)SCREEN_H-text_size.y}, text, olc::MAGENTA, {fScale, fScale} );
      }

      // Rotation Z
      matRotZ.m[0][0] = cosf(fTheta);
      matRotZ.m[0][1] = sinf(fTheta);
      matRotZ.m[1][0] = -sinf(fTheta);
      matRotZ.m[1][1] = cosf(fTheta);
      matRotZ.m[2][2] = 1;
      matRotZ.m[3][3] = 1;

      // Rotation X
      matRotX.m[0][0] = 1;
      matRotX.m[1][1] = cosf(fTheta * 0.5f);
      matRotX.m[1][2] = sinf(fTheta * 0.5f);
      matRotX.m[2][1] = -sinf(fTheta * 0.5f);
      matRotX.m[2][2] = cosf(fTheta * 0.5f);
      matRotX.m[3][3] = 1;

      // Draw Triangles
      int index=0;
      for (triangleref tri : tris) {

        vec3d p0 = *tri.p[0];
        vec3d p1 = *tri.p[1];
        vec3d p2 = *tri.p[2];

        //std::cout << "i: " << index << "p0.x: " << p0.x << " p0.y: " << p0.y << " p0.z: " << p0.z << std::endl;
        //std::cout << "i: " << index << "p1.x: " << p1.x << " p1.y: " << p1.y << " p1.z: " << p1.z << std::endl;
        //std::cout << "i: " << index << "p2.x: " << p2.x << " p2.y: " << p2.y << " p2.z: " << p2.z << std::endl;

        triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

        // Rotate in Z-Axis
        MultiplyMatrixVector(p0, triRotatedZ.p[0], matRotZ);
        MultiplyMatrixVector(p1, triRotatedZ.p[1], matRotZ);
        MultiplyMatrixVector(p2, triRotatedZ.p[2], matRotZ);

        // Rotate in X-Axis
        MultiplyMatrixVector(triRotatedZ.p[0], triRotatedZX.p[0], matRotX);
        MultiplyMatrixVector(triRotatedZ.p[1], triRotatedZX.p[1], matRotX);
        MultiplyMatrixVector(triRotatedZ.p[2], triRotatedZX.p[2], matRotX);

        // Offset into the screen
        triTranslated = triRotatedZX;
        triTranslated.p[0].z = triRotatedZX.p[0].z + 3.0f;
        triTranslated.p[1].z = triRotatedZX.p[1].z + 3.0f;
        triTranslated.p[2].z = triRotatedZX.p[2].z + 3.0f;

        // Project triangles from 3D --> 2D
        MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
        MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProj);
        MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProj);

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

        //check if projected tri contains one or more left eye points
        //for(auto bidule: meshFace.leftEye.tps) {
        //  for(int i=0; i<3; i++) {
        //    vec3d trianglePoint = *tri.p[i];
        //    triangleref leftEyeTriangle = meshFace.tris[bidule.triIndex];
        //    vec3d leftEyeTrianglePoint =*leftEyeTriangle.p[bidule.pointIndex];
        //    if(trianglePoint.x == leftEyeTrianglePoint.x && trianglePoint.y == leftEyeTrianglePoint.y && trianglePoint.z == leftEyeTrianglePoint.z) {
        //      FillCircle(triProjected.p[i].x, triProjected.p[i].y, 2, olc::CYAN);
        //    }
        //  }
        // }

        //check if projected tri contains one or more right eye points
        //for(auto bidule: meshFace.rightEye.tps) {
        //  for(int i=0; i<3; i++) {
        //    vec3d trianglePoint = *tri.p[i];
        //    triangleref rightEyeTriangle = meshFace.tris[bidule.triIndex];
        //    vec3d rightEyeTrianglePoint =*rightEyeTriangle.p[bidule.pointIndex];
        //    if(trianglePoint.x == rightEyeTrianglePoint.x && trianglePoint.y == rightEyeTrianglePoint.y && trianglePoint.z == rightEyeTrianglePoint.z) {
        //      FillCircle(triProjected.p[i].x, triProjected.p[i].y, 2, olc::CYAN);
        //    }
        //  }
        //}

        //check if projected tri contains one or more mouth points
        for(auto tp: mouth->tps) {
          for(int i=0; i<3; i++) {
            vec3d trianglePoint = *tri.p[i];
            triangleref mouthTriangle = tris[tp.triIndex];
            vec3d mouthTrianglePoint =*mouthTriangle.p[tp.pointIndex];
            if(trianglePoint.x == mouthTrianglePoint.x && trianglePoint.y == mouthTrianglePoint.y && trianglePoint.z == mouthTrianglePoint.z) {
              engine->FillCircle(triProjected.p[i].x, triProjected.p[i].y,2, CYAN);
            }
          }
        }

        // Rasterize triangle
        if(index == iDbgTriangleIndex && bDebugTriangles) {
          engine->FillTriangle(
            {triProjected.p[0].x, triProjected.p[0].y},
            {triProjected.p[1].x, triProjected.p[1].y},
            {triProjected.p[2].x, triProjected.p[2].y},
            YELLOW
          );
          engine->FillCircle(triProjected.p[0].x, triProjected.p[0].y, 4, RED);
          engine->FillCircle(triProjected.p[1].x, triProjected.p[1].y, 4, GREEN);
          engine->FillCircle(triProjected.p[2].x, triProjected.p[2].y, 4, BLUE);

        } else {
          //std::cout << "i:" << index << "p[0].x: " << triProjected.p[0].x << " p[0].y: " << triProjected.p[0].y << std::endl;
          //std::cout << "i:" << index << "p[1].x: " << triProjected.p[1].x << " p[1].y: " << triProjected.p[1].y << std::endl;
          //std::cout << "i:" << index << "p[2].x: " << triProjected.p[2].x << " p[2].y: " << triProjected.p[2].y << std::endl;
          
          engine->DrawTriangle(
            triProjected.p[0].x, triProjected.p[0].y,
            triProjected.p[1].x, triProjected.p[1].y,
            triProjected.p[2].x, triProjected.p[2].y, 
            RED
          );
        }
        index++;
      }
      //std::cout << "index: " << index << std::endl;
    }
    
    void ToggleDebugTriangle() {
      bDebugTriangles = !bDebugTriangles;
    }

    /// Show next triangle (Debug)
    void DbgNextTriangle() {
      iDbgTriangleIndex++;
      if (iDbgTriangleIndex >= tris.size()) {
        iDbgTriangleIndex = 0;
      }
    }
    /// Show previous triangle (Debug)
    void DbgPrevTriangle() {
      iDbgTriangleIndex--;
      if (iDbgTriangleIndex < 0) {
        iDbgTriangleIndex = tris.size() - 1;
      }
    }

  private:
    IDrzEngine* engine;
    
    const float fFov = 90.0f; // Field of view in degrees
    const float fFovRad = 1.0f / tan(fFov * 0.5f / 180.0f * 3.14159f);

    //3D rotation variables
    float fTheta = 2* 3.14159f;  
    float fAspectRatio = 1.0f;

    mat4x4 matProj;

    //debug
    int iDbgTriangleIndex = 0;
    bool bDebugTriangles = false;

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

    // Function to project 3D coordinates to 2D screen coordinates
    vec2d Project(float x, float y, float z) {
      float screenX = (x / (z * fAspectRatio)) * fFovRad * engine->GetScreenWidth() + (engine->GetScreenWidth() / 2.0f);
      float screenY = (y / z) * fFovRad * engine->GetScreenHeight() + (engine->GetScreenHeight() / 2.0f);
      std::cout << "x: " << screenX << " y: " << screenY << std::endl;
      return {screenX, screenY};
    }
};
