#pragma once

#include <IDrzEngine.h>
#include "Scene.h"

#include "models/ScoutModel.h"

#include <random>

class Scout {
  public: 
    Scout() {

      rotationDistribution = std::uniform_real_distribution<float>(-1.0f, 1.0f); // Small random rotations in degrees
      translationDistribution = std::uniform_real_distribution<float>(-0.1f, 0.1f); // Small random translations
      InitializeRandomGenerator();
      _startAnimation();

      head = new ScoutHeadModel();
      jaw = new ScoutJawModel();
      mic = new ScoutMicModel();

      leftEye = new ScoutEyeModel({-0.32f, -0.3f, -0.9f});
      rightEye = new ScoutEyeModel({0.32f, -0.3f, -0.9f});
    }

    void AddToScene(Scene* scene) {
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