#pragma once

//Base class for model animated parts

#include <iostream>
#include <queue>
#include <chrono>

#include "../../include/IDrzEngine.h"

#include "Model.h"

struct animkeyduration {
  uint16_t index;
  float duration;
};

class AnimatedObject {
  public:
    AnimatedObject(IDrzEngine* engine, Model* model)
      : engine(engine), model(model), currentAnimationIndex(0), isAnimating(false) {
      }

    void SetupAnimatedObject() {
      for (auto b : tps) {
        verts.push_back(model->tris[b.triIndex].p[b.pointIndex]);
      }
    }

    void Update() {
      if (isAnimating) {
        _updateAnimation();
      } else {
        if (!animQueue.empty()) {
          _startAnimation();
        } 
      }
    }

    void QueueAnimation(uint16_t index, float duration) {
      animQueue.push({index,duration});
    }
    
    //associate the vertices with the corresponding triangles + point indexes
    std::vector<trianglepointindex> tps;

    //store the vertices references
    std::vector<vec3d*> verts;

    //store the keyframes and their corresponding verts
    std::vector<std::vector<vec3d>> kf_points;

  private:
    IDrzEngine* engine;
    Model* model;
    bool isAnimating;
    uint16_t currentAnimationIndex;     //index of the current animation keyframe
    float currentAnimationDuration;     //in seconds
    double currentAnimationStartTime;   //in ms

    std::queue<animkeyduration> animQueue;

    // In your class, add a vector to store the initial positions of vertices
    std::vector<vec3d> lastVertPositions;

    //move the vertices according toward current animation keyframe using speed as a factor
    void _updateAnimation() {

      auto now = engine->Now();
      double elapsedTime = (now - currentAnimationStartTime)/1000.0f; //in seconds
      double t = elapsedTime/currentAnimationDuration;
      
      //if animation reached duration, stop it
      if (t >= 1.0f) {
        t = 1.0f;
        isAnimating = false;
      }

      //move vertices
      for (size_t i = 0; i < verts.size(); ++i) {
        vec3d* v = verts[i];
        if(isAnimating) {
          const vec3d initial = lastVertPositions[i];
          const vec3d* target = &kf_points[currentAnimationIndex][i];
          // Linear interpolation based on t
          v->x = (1.0f - t) * initial.x + t * target->x;
          v->y = (1.0f - t) * initial.y + t * target->y;
          v->z = (1.0f - t) * initial.z + t * target->z;
        } else {
          // Ensure final position is exactly the target position
          const vec3d& target = kf_points[currentAnimationIndex][i];
          *v = target;
        }
      }
    }

    void _startAnimation() {

      //pop the next animation from the queue
      animkeyduration akd = animQueue.front();
      animQueue.pop();

      // Set the current animation index and duration
      currentAnimationIndex = akd.index;
      currentAnimationDuration = akd.duration;
      currentAnimationStartTime = engine->Now();

      // Save the initial positions of vertices
      lastVertPositions.clear();
      for (vec3d* v : verts) {
        lastVertPositions.push_back(*v);
      }

      isAnimating = true;
    }
    
};