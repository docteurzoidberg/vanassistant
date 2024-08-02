#pragma once

#include <IDrzEngine.h>

#include <vector>

#include "models/J7Model.h"

using namespace drz;


class J7 {
  public: 
    J7() {
      body = new J7BodyModel();
      wheelFL = new J7WheelModel(); //TODO : add wheel offset
      wheelFR = new J7WheelModel(); //TODO : add wheel offset
      wheelBL = new J7WheelModel(); //TODO : add wheel offset
      wheelBR = new J7WheelModel(); //TODO : add wheel offset
      //scene->AddModel(body);
      //scene->AddModel(wheelFL);
      //scene->AddModel(wheelFR);
      //scene->AddModel(wheelBL);
      //scene->AddModel(wheelBR);
    }

    //get models
    std::vector<Model*> GetModels() {
      return {body, wheelFL, wheelFR, wheelBL, wheelBR};
    }

    void Update(float elapsedTime) {

      //update whole model rotation
      angle += elapsedTime * 0.1f;

      //make wheel rotate at model speed
      wheelPitch += elapsedTime * speed;

      //global model matrices
      auto rotationMatrix = Matrix4x4::CreateRotationMatrix(0, angle, 0);
      auto translationMatrix = Matrix4x4::CreateTranslationMatrix(0, 0, 0);
      
      //front wheels can rotate in two axis (pitch.yaw)
      //back wheels can rotate in one axis (pitch)
      auto frontWheelsRotationMatrix = Matrix4x4::CreateRotationMatrix(wheelPitch, wheelYaw, 0);
      auto backWheelsRotationMatrix = Matrix4x4::CreateRotationMatrix(wheelPitch, 0, 0);

      body->rotationMatrix = rotationMatrix;
      body->translationMatrix = translationMatrix;

      wheelFL->rotationMatrix = rotationMatrix * frontWheelsRotationMatrix;
      wheelFL->translationMatrix = translationMatrix;

      wheelFR->rotationMatrix = rotationMatrix * frontWheelsRotationMatrix;
      wheelFR->translationMatrix = translationMatrix;

      wheelBL->rotationMatrix = rotationMatrix * backWheelsRotationMatrix;
      wheelBL->translationMatrix = translationMatrix;

      wheelBR->rotationMatrix = rotationMatrix * backWheelsRotationMatrix;
      wheelBR->translationMatrix = translationMatrix;
    }

    void SetSpeed(float speed) {
      if(speed < 0) {
        speed = 0;
      }
      if(speed > 0) {
        isMoving = true;
      } else {
        isMoving = false;
      }
      this->speed = speed;
    }

  private:
    float angle = 0.0f;
    float wheelPitch = 0.0f;
    float wheelYaw = 0.0f;
    float speed = 1.0f;

    bool isMoving = false;

    J7BodyModel* body;
    J7WheelModel* wheelFL;  //front left wheel
    J7WheelModel* wheelFR;  //front right wheel
    J7WheelModel* wheelBL;  //back left wheel
    J7WheelModel* wheelBR;  //back right wheel
};