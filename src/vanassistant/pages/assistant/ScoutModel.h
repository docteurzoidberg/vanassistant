#pragma once

#include <IDrzEngine.h>

#include "IModel.h"

#include "Scene.h"

#include <cmath>
#include <random>

class ScoutEyeModel : public IModel {
  public:
    // clang-format off 
    //Hardcoded face model vertices
    std::vector<vec3d> verts = {
      {0.0001015, -0.078582, -0.167958},
      {0.0001015, -0.094591, -0.114096},
      {-0.0653975, -0.056014, -0.167958},
      {-0.079002, -0.0673365, -0.114096},
      {-0.111769, -0.001526, -0.114096},
      {0.0001015, 0.0915355, -0.114096},
      {0.111976, -0.001526, -0.114096},
      {0.065604, -0.056014, -0.167958},
      {0.0792085, -0.0673365, -0.114096},
      {0.0001015, -0.118528, -0.103334},
      {0.0001015, -0.118528, 0.115517},
      {0.0995575, -0.0842625, -0.103334},
      {0.0995575, -0.0842625, 0.115521},
      {0.140752, -0.001526, 0.115517},
      {0.0995575, 0.081207, 0.115521},
      {0.0001015, 0.115475, -0.103334},
      {0.0001015, 0.115475, 0.115521},
      {-0.099351, 0.081207, -0.103334},
      {-0.099351, 0.081207, 0.115521},
      {-0.140546, -0.001526, 0.115517},
      {-0.099351, -0.0842625, -0.103334},
      {-0.099351, -0.0842625, 0.115521},
      {-0.140546, -0.001526, -0.103334},
      {-0.079002, 0.0642775, -0.114096},
      {0.0995575, 0.081207, -0.103334},
      {0.0792085, 0.0642775, -0.114096},
      {0.140752, -0.001526, -0.103334},
      {-0.092526, -0.001526, -0.167958},
      {-0.044653, 0.0357035, -0.157948},
      {-0.0653975, 0.0529585, -0.167958},
      {-0.0631925, -0.001526, -0.157948},
      {-0.044653, -0.0387555, -0.157948},
      {0.0448595, -0.0387555, -0.157948},
      {0.0001015, 0.07553, -0.167958},
      {0.0001015, -0.0541835, -0.157948},
      {0.0927325, -0.001526, -0.167958},
      {0.063399, -0.001526, -0.157948},
      {0.065604, 0.0529585, -0.167958},
      {0.0448595, 0.0357035, -0.157948},
      {0.0001015, 0.0511245, -0.157948},
    };
    std::vector<face> faces = {
      {19, 16, 18},
      {33, 32, 29},
      {25, 14, 27},
      {36, 9, 8},
      {14, 12, 27},
      {38, 6, 26},
      {25, 7, 26},
      {12, 7, 27},
      {2, 8, 9},
      {33, 1, 35},
      {5, 21, 23},
      {8, 37, 36},
      {32, 1, 3},
      {5, 18, 24},
      {28, 4, 5},
      {30, 31, 28},
      {12, 11, 10},
      {10, 4, 2},
      {31, 3, 28},
      {11, 21, 10},
      {2, 3, 1},
      {30, 6, 34},
      {9, 10, 2},
      {16, 24, 18},
      {28, 24, 30},
      {39, 34, 38},
      {16, 26, 6},
      {21, 20, 23},
      {20, 18, 23},
      {26, 36, 38},
      {38, 37, 39},
      {16, 15, 25},
      {34, 29, 30},
      {19, 17, 16},
      {29, 40, 39},
      {39, 37, 33},
      {33, 35, 32},
      {32, 31, 29},
      {29, 39, 33},
      {25, 15, 14},
      {36, 7, 9},
      {14, 13, 12},
      {38, 34, 6},
      {25, 27, 7},
      {12, 9, 7},
      {2, 1, 8},
      {33, 8, 1},
      {5, 4, 21},
      {8, 33, 37},
      {32, 35, 1},
      {5, 23, 18},
      {28, 3, 4},
      {30, 29, 31},
      {12, 13, 11},
      {10, 21, 4},
      {31, 32, 3},
      {11, 22, 21},
      {2, 4, 3},
      {30, 24, 6},
      {9, 12, 10},
      {16, 6, 24},
      {28, 5, 24},
      {39, 40, 34},
      {16, 25, 26},
      {21, 22, 20},
      {20, 19, 18},
      {26, 7, 36},
      {38, 36, 37},
      {16, 17, 15},
      {34, 40, 29},
    };
    // clang-format on

    ScoutEyeModel(vec3d offset) : offset(offset), IModel(&verts, &faces){ 
      SetupModel();
    }

    void Update(float elapsedTime) override {
      //matrix rotation from yaw and pitch
      Matrix4x4 matRotX = Matrix4x4::CreateRotationMatrixX(pitch);
      Matrix4x4 matRotY = Matrix4x4::CreateRotationMatrixY(yaw);
      rotationMatrix = matRotX * matRotY;

      //combine translations with actual translation matrix
      translationMatrix[0][3] += offset.x;
      translationMatrix[1][3] += offset.y;
      translationMatrix[2][3] += offset.z;
    }

    void SetPitch(float pitch) {
      this->pitch = pitch;
    }
    void SetYaw(float yaw) {
      this->yaw = yaw;
    }

  private:
    float pitch = 0.1;
    float yaw = 0.1f;   
    vec3d offset;
};

class ScoutMicModel : public IModel {
  public:
    // clang-format off 
    //Hardcoded face model vertices
    std::vector<vec3d> verts = {
      {0.507854, 0.840213, -1.35998},
      {1.36524, 0.122381, -0.215765},
      {1.36524, 0.23939, -0.0001015},
      {1.36524, -0.228631, -0.0001015},
      {1.36524, 0.122381, 0.215561},
      {1.36524, -0.111622, 0.215561},
      {1.14942, 0.122381, 0.215561},
      {1.14942, -0.111622, 0.215561},
      {1.14942, -0.228631, -0.0001015},
      {1.14942, -0.111622, -0.215765},
      {1.36524, -0.111622, -0.215765},
      {1.14942, 0.122381, -0.215765},
      {1.14942, 0.23939, -0.0001015},
      {1.15506, 0.144676, -0.118975},
      {1.14957, 0.361088, -0.786205},
      {0.976444, 0.559685, -1.14073},
      {0.969969, 0.67794, -1.07591},
      {0.524748, 0.739399, -1.45636},
      {0.524748, 0.857497, -1.39139},
      {1.15505, 0.026537, -0.18397},
      {1.1054, 0.357934, -0.779761},
      {0.944818, 0.546297, -1.11607},
      {0.507854, 0.72212, -1.42494},
      {1.10984, 0.408419, -0.149173},
      {1.10984, 0.196255, -0.406077},
      {1.10984, 0.138481, 0.42496},
      {1.10984, -0.37373, -0.195258},
      {1.10984, -0.185497, 0.405874},
      {1.11055, 0.0268555, -0.183771},
      {1.15435, -0.047348, -0.176323},
      {1.10984, -0.0473445, -0.176319},
      {1.15435, -0.148995, -0.0850325},
      {1.10984, -0.148995, -0.0850325},
      {1.15435, -0.160213, 0.056007},
      {1.10984, -0.0744275, 0.164178},
      {1.10984, -0.160209, 0.056007},
      {1.15435, 0.0581035, 0.176116},
      {1.15435, -0.074438, 0.164178},
      {1.10984, 0.0581035, 0.176116},
      {1.15435, 0.159747, 0.084826},
      {1.10984, 0.159754, 0.0848295},
      {1.10984, 0.170968, -0.05621},
      {1.15435, 0.170968, -0.05621},
      {1.10984, -0.397663, 0.14897},
      {1.06036, 0.153573, 0.473144},
      {1.10984, 0.384489, 0.195055},
      {1.06036, 0.427462, 0.217175},
      {1.06036, 0.454104, -0.166072},
      {1.10984, -0.127722, -0.425166},
      {1.06036, -0.416703, -0.217378},
      {1.06036, -0.20713, 0.451892},
      {0.62238, -0.214239, 0.467029},
      {0.62238, 0.15854, 0.488996},
      {0.62238, 0.441609, 0.224452},
      {0.62238, 0.469143, -0.171629},
      {0.62238, 0.225015, -0.467233},
      {1.06036, 0.217889, -0.452095},
      {0.62238, -0.147763, -0.489199},
      {1.06036, -0.14281, -0.47335},
      {0.62238, -0.430833, -0.224654},
      {1.06036, -0.443345, 0.165869},
      {0.62238, -0.458371, 0.171427},
      {1.11055, 0.145002, -0.118772},
      {1.15356, 0.479707, -0.722197},
      {0.938343, 0.664549, -1.05125},
      {1.10939, 0.476557, -0.71575},
    };
    std::vector<face> faces = {
      {65, 64, 17},
      {3, 11, 2},
      {3, 4, 11},
      {3, 6, 4},
      {3, 5, 6},
      {5, 8, 6},
      {5, 7, 8},
      {6, 9, 4},
      {6, 8, 9},
      {4, 10, 11},
      {4, 9, 10},
      {11, 12, 2},
      {11, 10, 12},
      {2, 13, 3},
      {2, 12, 13},
      {3, 7, 5},
      {3, 13, 7},
      {15, 14, 20},
      {15, 64, 14},
      {1, 17, 19},
      {23, 65, 1},
      {21, 20, 29},
      {21, 15, 20},
      {64, 16, 17},
      {55, 57, 56},
      {29, 20, 30},
      {29, 30, 31},
      {30, 33, 31},
      {30, 32, 33},
      {34, 33, 32},
      {34, 36, 33},
      {38, 36, 34},
      {38, 35, 36},
      {37, 35, 38},
      {37, 39, 35},
      {39, 37, 40},
      {39, 40, 41},
      {40, 42, 41},
      {40, 43, 42},
      {43, 63, 42},
      {43, 14, 63},
      {53, 51, 45},
      {60, 59, 50},
      {58, 57, 59},
      {45, 28, 26},
      {54, 48, 55},
      {49, 50, 59},
      {61, 60, 50},
      {26, 47, 45},
      {61, 28, 51},
      {47, 24, 48},
      {51, 62, 61},
      {48, 25, 57},
      {50, 44, 61},
      {25, 59, 57},
      {45, 54, 53},
      {14, 66, 63},
      {14, 64, 66},
      {23, 16, 22},
      {18, 1, 19},
      {63, 21, 29},
      {63, 66, 21},
      {66, 22, 21},
      {18, 17, 16},
      {15, 22, 16},
      {44, 49, 24},
      {65, 66, 64},
      {1, 65, 17},
      {23, 22, 65},
      {64, 15, 16},
      {55, 48, 57},
      {53, 52, 51},
      {60, 58, 59},
      {58, 56, 57},
      {45, 51, 28},
      {54, 47, 48},
      {49, 27, 50},
      {61, 62, 60},
      {26, 46, 47},
      {61, 44, 28},
      {47, 46, 24},
      {51, 52, 62},
      {48, 24, 25},
      {50, 27, 44},
      {25, 49, 59},
      {45, 47, 54},
      {23, 18, 16},
      {18, 23, 1},
      {66, 65, 22},
      {18, 19, 17},
      {15, 21, 22},
      {24, 46, 26},
      {26, 28, 44},
      {44, 27, 49},
      {49, 25, 24},
      {24, 26, 44},
    };
    // clang-format on

    ScoutMicModel() : IModel(&verts, &faces){ 
      SetupModel();
    }

    void Update(float elapsedTime) override {
      //if animation is not active, set a new target angle
      if(animate) {
        currentAngle += incrementSpeed * animationDirection * elapsedTime;
        if(animationDirection == 1) {
          if(currentAngle >= targetAngle) {
            animate = false;
            animationDirection = -1;
          }
        } else {
          if(currentAngle <= targetAngle) {
            animate = false;
            animationDirection = 1;
          }
        }
      } else {
        _setNewAngleTarget();
        animate = true;
      }

      Matrix4x4 matRotX = Matrix4x4::CreateRotationMatrixX(currentAngle);
      rotationMatrix = rotationMatrix * matRotX;
    }

  private:
    int animationDirection = 1;
    float currentAngle = 0.0f;
    float targetAngle = 0.0f;
    float minAngle = -0.2f;
    float maxAngle = 0.2f; 
    float incrementSpeed = 1.0f;
    bool animate = false;

    void _setNewAngleTarget() {
      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_real_distribution<> dis(minAngle, maxAngle);
      targetAngle = dis(gen);
    }
};

class ScoutJawModel : public IModel {
  public:
    // clang-format off 
    //Hardcoded face model vertices
    std::vector<vec3d> verts = {
      {0.498453, 0.609942, -0.121895},
      {-0.490175, 0.99133, -0.220153},
      {-0.480137, 0.609942, -0.121895},
      {0.508491, 0.99133, -0.220153},
      {0.30827, 1.0936, -1.03093},
      {0.0091595, 1.09374, -1.08808},
      {0.328136, 0.562328, -1.11536},
      {0.0091595, 0.562328, -1.17249},
      {-0.289954, 1.0936, -1.03093},
      {-0.30982, 0.562328, -1.11536},
    };
    std::vector<face> faces = {
      {1, 4, 2},
      {1, 2, 3},
      {4, 5, 6},
      {9, 2, 6},
      {7, 4, 1},
      {7, 5, 4},
      {6, 5, 7},
      {6, 7, 8},
      {10, 6, 8},
      {10, 9, 6},
      {2, 9, 10},
      {2, 10, 3},
      {4, 6, 2},
    };
    // clang-format on

    ScoutJawModel() : IModel(&verts, &faces){ 
      SetupModel();
    }

    //Set the current jaw angle
    //map jaw opening to min and max angles
    void SetOpening(float opening) {
      if(opening < 0.0f) {
        opening = 0.0f;
      } else if(opening > 1.0f) {
        opening = 1.0f;
      }
      this->opening = opening;
      angle = opening * (maxAngle - minAngle) + minAngle;
      SetAngle(angle);
    }

    void Update(float elapsedTime) override {

      // Rotate the model
      if(direction == 1) {
        angle += 0.001f;
      } else {
        angle -= 0.001f;
      }
      
      if(angle> maxAngle) {
        direction = -1;
      } else if (angle <= minAngle) {
        direction = 1;
      }

      Matrix4x4 matRotX = Matrix4x4::CreateRotationMatrixX(angle); 
      rotationMatrix = rotationMatrix * matRotX;
    }

  private:
    int direction = 1;  //1 for increasing angle, -1 for decreasing angle
    float opening = 0.5f;
    float angle = M_PI;
    float minAngle = -0.2f;
    float maxAngle = 0.0f;

    void SetAngle(float angle) {
      if(angle > maxAngle) {
        this->angle =  maxAngle;
      } else if (angle < minAngle) {
        this->angle = minAngle;
      } else {
        this->angle = angle;
      }
    }
};
 
//3d face model including animated subparts
class ScoutHeadModel : public IModel {
  public:
    // clang-format off 
    //Hardcoded face model vertices
    std::vector<vec3d> verts = {
      {0.283549, 0.614877, 0.035609},
      {0.283413, 0.09786, 0.257093},
      {-0.265097, 0.09786, 0.257093},
      {-0.288851, 0.539788, 0.45276},
      {0.321808, 0.041412, 0.688398},
      {0.605178, 0.557032, 0.356825},
      {0.307167, 0.539788, 0.45276},
      {0.0091595, -0.538135, 1.05631},
      {0.0091595, 0.02576, 0.751503},
      {-0.303492, 0.041412, 0.688398},
      {-0.764729, 0.057071, 0.562114},
      {-0.232834, -0.561995, -1.03409},
      {-0.763357, -0.321198, -1.53264},
      {-0.240215, -0.56336, -1.79283},
      {0.25115, -0.561995, -1.03409},
      {0.258531, -0.56336, -1.79283},
      {-0.562446, 0.689965, -0.373201},
      {-0.265234, 0.614877, 0.035609},
      {0.355159, 0.561375, -1.18116},
      {-0.336843, 0.561375, -1.18116},
      {0.887309, -0.423203, -0.662599},
      {-0.860675, -0.423203, -1.53264},
      {-0.980514, -0.479192, 0.174849},
      {-0.782418, 0.083419, -0.0749455},
      {-0.586863, 0.557032, 0.356825},
      {0.580762, 0.689965, -0.373201},
      {0.50323, 0.118153, -0.924126},
      {0.778442, -0.325073, -0.667086},
      {0.800734, 0.083419, -0.0749455},
      {0.783045, 0.057071, 0.562114},
      {0.998833, -0.479192, 0.174849},
      {0.0091595, -1.39108, 0.97769},
      {-0.81488, -1.32745, 0.0261905},
      {-0.724689, -1.27072, -0.822125},
      {0.29449, -1.24655, -1.18357},
      {-0.276171, -1.24655, -1.18357},
      {0.743005, -1.27072, -0.822125},
      {0.833196, -1.32745, 0.0261905},
      {0.722256, -1.3801, 0.804174},
      {-0.244668, -0.687883, -1.06225},
      {-0.868994, -0.423203, -0.662599},
      {0.262983, -0.687883, -1.06225},
      {-0.703941, -1.3801, 0.804174},
      {0.85238, -0.526788, 0.876894},
      {0.781672, -0.321198, -1.53264},
      {-0.308123, -0.205496, -0.799645},
      {-0.361946, -0.2144, -0.799648},
      {-0.407578, -0.239764, -0.799652},
      {-0.438067, -0.277725, -0.799652},
      {-0.44877, -0.3225, -0.799655},
      {-0.438064, -0.367272, -0.799652},
      {-0.407575, -0.405234, -0.799652},
      {-0.361942, -0.430595, -0.799648},
      {-0.254292, -0.430591, -0.799641},
      {-0.308119, -0.439498, -0.799645},
      {-0.178174, -0.367269, -0.799638},
      {-0.208663, -0.405234, -0.799641},
      {-0.167468, -0.322497, -0.799638},
      {-0.178178, -0.277721, -0.799638},
      {-0.208666, -0.239761, -0.799641},
      {-0.254296, -0.2144, -0.799641},
      {0.26088, -0.68649, -1.79459},
      {0.878994, -0.423203, -1.53264},
      {-0.242564, -0.68649, -1.79459},
      {-0.834061, -0.526788, 0.876894},
      {-0.298557, -0.272135, -0.923811},
      {-0.760126, -0.325073, -0.667086},
      {-0.22282, 0.14462, -1.06073},
      {-0.484915, 0.118153, -0.924126},
      {-0.163842, 0.561375, -1.22921},
      {0.182158, 0.561375, -1.22921},
      {0.241136, 0.14462, -1.06073},
      {0.316873, -0.272135, -0.923811},
    };
    std::vector<face> faces = {
      {1, 3, 2},
      {1, 18, 3},
      {7, 2, 5},
      {7, 1, 2},
      {3, 4, 10},
      {3, 18, 4},
      {5, 10, 9},
      {10, 5, 2},
      {10, 2, 3},
      {7, 30, 6},
      {7, 5, 30},
      {5, 44, 30},
      {8, 44, 5},
      {8, 5, 9},
      {10, 8, 9},
      {10, 65, 8},
      {65, 10, 11},
      {11, 10, 4},
      {11, 4, 25},
      {12, 13, 67},
      {12, 14, 13},
      {14, 12, 15},
      {14, 15, 16},
      {45, 16, 15},
      {45, 15, 28},
      {17, 18, 1},
      {18, 17, 25},
      {18, 25, 4},
      {6, 1, 7},
      {6, 26, 1},
      {17, 1, 26},
      {20, 17, 26},
      {20, 26, 19},
      {20, 19, 71},
      {20, 71, 70},
      {39, 31, 44},
      {39, 38, 31},
      {21, 31, 38},
      {21, 38, 37},
      {23, 67, 41},
      {67, 22, 41},
      {67, 13, 22},
      {43, 8, 65},
      {43, 32, 8},
      {8, 32, 39},
      {8, 39, 44},
      {69, 17, 20},
      {11, 23, 65},
      {11, 24, 23},
      {24, 67, 23},
      {24, 69, 67},
      {69, 24, 17},
      {25, 17, 24},
      {25, 24, 11},
      {26, 27, 19},
      {29, 26, 6},
      {26, 29, 27},
      {29, 28, 27},
      {29, 31, 28},
      {30, 31, 29},
      {30, 44, 31},
      {29, 6, 30},
      {37, 38, 33},
      {36, 35, 37},
      {33, 34, 37},
      {34, 36, 37},
      {32, 43, 33},
      {38, 39, 32},
      {40, 34, 41},
      {40, 36, 34},
      {42, 36, 40},
      {42, 35, 36},
      {37, 35, 42},
      {37, 42, 21},
      {61, 56, 53},
      {23, 43, 65},
      {23, 33, 43},
      {41, 33, 23},
      {41, 34, 33},
      {31, 21, 28},
      {63, 28, 21},
      {63, 45, 28},
      {22, 40, 41},
      {22, 64, 40},
      {40, 64, 62},
      {40, 62, 42},
      {42, 62, 63},
      {42, 63, 21},
      {63, 16, 45},
      {63, 62, 16},
      {64, 16, 62},
      {64, 14, 16},
      {22, 14, 64},
      {22, 13, 14},
      {73, 27, 28},
      {15, 73, 28},
      {12, 73, 15},
      {12, 66, 73},
      {66, 12, 67},
      {69, 66, 67},
      {69, 68, 66},
      {20, 68, 69},
      {20, 70, 68},
      {68, 70, 71},
      {68, 71, 72},
      {72, 71, 19},
      {72, 19, 27},
      {73, 72, 27},
      {72, 73, 66},
      {72, 66, 68},
      {38, 32, 33},
      {53, 52, 51},
      {51, 50, 49},
      {49, 48, 47},
      {47, 46, 61},
      {61, 60, 59},
      {59, 58, 56},
      {56, 57, 54},
      {54, 55, 53},
      {53, 51, 49},
      {49, 47, 61},
      {61, 59, 56},
      {56, 54, 53},
      {53, 49, 61},
    };
    // clang-format on

    ScoutHeadModel() : IModel(&verts, &faces)  {
      //Load model tri and faces
      SetupModel();
    }

    void Update(float elapsedTime) override {}
};

struct animframe {
  float tX, tY, tZ;
  float rX, rY, rZ;
};

class Scout {
  public: 
    Scout(Scene* scene) {

      rotationDistribution = std::uniform_real_distribution<float>(-1.0f, 1.0f); // Small random rotations in degrees
      translationDistribution = std::uniform_real_distribution<float>(-0.1f, 0.1f); // Small random translations
      InitializeRandomGenerator();
      _startAnimation();

      head = new ScoutHeadModel();
      jaw = new ScoutJawModel();
      mic = new ScoutMicModel();

      leftEye = new ScoutEyeModel({-0.32f, -0.3f, -0.9f});
      rightEye = new ScoutEyeModel({0.32f, -0.3f, -0.9f});

      scene->AddModel(head);
      scene->AddModel(jaw);
      scene->AddModel(mic);
      scene->AddModel(leftEye);
      scene->AddModel(rightEye);
    }

    void SetJawOpening(float opening) {
      if(opening < 0.0f) {
        opening = 0.0f;
      } else if(opening > 1.0f) {
        opening = 1.0f;
      }
      jawOpening = opening;
      jaw->SetOpening( opening);
    }

    void Update(float elapsedTime) {
    
      _updateAnimation(elapsedTime);

      //Set models matrices
      
      head->rotationMatrix = rotationMatrix;
      head->translationMatrix = translationMatrix;

      jaw->rotationMatrix = rotationMatrix;
      jaw->translationMatrix = translationMatrix;

      mic->rotationMatrix = rotationMatrix;
      mic->translationMatrix = translationMatrix;

      leftEye->rotationMatrix = rotationMatrix;
      leftEye->translationMatrix = translationMatrix;

      rightEye->rotationMatrix = rotationMatrix;
      rightEye->translationMatrix = translationMatrix;
    }
    
  private:
    std::default_random_engine generator;
    std::uniform_real_distribution<float> rotationDistribution;
    std::uniform_real_distribution<float> translationDistribution;

    ScoutHeadModel* head;
    ScoutJawModel* jaw;
    ScoutMicModel* mic;
    ScoutEyeModel* leftEye;
    ScoutEyeModel* rightEye;

    Matrix4x4 rotationMatrix = Matrix4x4::Identity(); // Initialize with your current rotation matrix
    Matrix4x4 translationMatrix = Matrix4x4::Identity(); // Initialize with an identity matrix for no initial translation
    
    float jawOpening = 0.0f; //0.0f closed, 1.0f open
    float t = 0.0f;
    float currentAnimationDuration = 0.0f;

    animframe animTarget = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    animframe animCurrent = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    animframe animInitial = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    animframe animTargetDeltaMax = {0.01f, 0.01f, 0.01f, 0.0005f, 0.0005f, 0.0005f};

    void InitializeRandomGenerator() {
      std::random_device rd;
      generator.seed(rd());
    }

    void _clamp(float& target, float deltaMax) {
      if(target > deltaMax) {
        target = deltaMax;
      } else if (target < -deltaMax) {
        target = -deltaMax;
      }
    }

    void _clampTarget(animframe &target, animframe &deltaMax) {
      _clamp(target.tX, deltaMax.tX);
      _clamp(target.tY, deltaMax.tY);
      _clamp(target.tZ, deltaMax.tZ);
      _clamp(target.rX, deltaMax.rX);
      _clamp(target.rY, deltaMax.rY);
      _clamp(target.rZ, deltaMax.rZ);
    }

    void _calcNextTarget() {
      
      animInitial = animCurrent;
      animTarget = animCurrent;

      // Generate small random angles for rotation
      float deltaPitch = rotationDistribution(generator);
      float deltaYaw = rotationDistribution(generator);
      float deltaRoll = rotationDistribution(generator);
      
      // Convert angles to radians
      float pitchRad = deltaPitch * (M_PI / 180.0f);
      float yawRad = deltaYaw * (M_PI / 180.0f);
      float rollRad = deltaRoll * (M_PI / 180.0f);

      // Generate small random translations
      float deltaX = translationDistribution(generator);
      float deltaY = translationDistribution(generator);
      float deltaZ = translationDistribution(generator);

      // update target
      animTarget.tX += deltaX;
      animTarget.tY += deltaY;
      animTarget.tZ += deltaZ;
      animTarget.rX += pitchRad;
      animTarget.rY += yawRad;
      animTarget.rZ += rollRad;

      // Clamp target values
      _clampTarget(animTarget, animTargetDeltaMax);
    }

    void _startAnimation() {
      t = 0.0f;
      currentAnimationDuration = 0.5f;
      _calcNextTarget();
    }

    void _updateAnimation(float elapsedTime) {

      //not sure !
      t += elapsedTime / currentAnimationDuration;

      //interpolate
      animCurrent.tX = animInitial.tX * (1.0f - t) + animTarget.tX * t;
      animCurrent.tY = animInitial.tY * (1.0f - t) + animTarget.tY * t;
      animCurrent.tZ = animInitial.tZ * (1.0f - t) + animTarget.tZ * t;
      animCurrent.rX = animInitial.rX * (1.0f - t) + animTarget.rX * t;
      animCurrent.rY = animInitial.rY * (1.0f - t) + animTarget.rY * t;
      animCurrent.rZ = animInitial.rZ * (1.0f - t) + animTarget.rZ * t;

      rotationMatrix = Matrix4x4::CreateRotationMatrix(animCurrent.rX, animCurrent.rY, animCurrent.rZ);
      translationMatrix = Matrix4x4::CreateTranslationMatrix(animCurrent.tX, animCurrent.tY, animCurrent.tZ);

      //check if anim is done
      if(t >= 1.0f) {
        animCurrent = animTarget;
        _calcNextTarget();
        t = 0.0f;
        return;
      }
    }
};