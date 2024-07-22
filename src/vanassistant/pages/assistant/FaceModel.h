#pragma once

#include <IDrzEngine.h>

#include "AnimatedObject.h"
#include "../../Model.h"

#include <cmath>

using namespace drz;

//Mouth part animations
class MouthPart : public AnimatedObject {
  public:

    //Animation keys
    enum KEY_FRAME {
      OPEN,
      CLOSE
    };

    MouthPart(IDrzEngine* engine, Model* model) : AnimatedObject(engine, model) { 
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
      SetupAnimatedObject();
      // clang-format on
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

      //Load model tri and faces
      SetupModel();

      //Animated parts
      mouth = new MouthPart(engine, this);
    }

    void Update(float elapsedTime) {
      //sub parts update
      mouth->Update();
    }  

  private:
    IDrzEngine* engine;
};
