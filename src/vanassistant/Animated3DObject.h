#pragma once

//Base class for model animated parts

#include <queue>
#include <chrono>

#include "../../include/IDrzEngine.h"

#include "Model.h"

struct animkeyduration {
  uint16_t index;
  float duration;
};

class Animated3DObject {
  public:
    Animated3DObject(IDrzEngine* engine, Model* model)
      : engine(engine), model(model), currentAnimationIndex(0), isAnimating(false) {
        load();
      }

    void QueueAnimation(uint16_t index, float duration) {
      animQueue.push({index,duration});
    }

    void Update() {
      if (isAnimating) {
        _updateAnimation();
      } else {
        if (!animQueue.empty()) {
          auto kd = animQueue.front();
          currentAnimationIndex = kd.index;
          currentAnimationDuration = kd.duration;
          //TODO
          //std::cout << "Starting animation " << currentAnimationIndex << " at speed " << currentAnimationDuration << std::endl;
          animQueue.pop();
          isAnimating = true;
          animationStartTime = engine->Now();
        } 
      }
    }
    //associate the vertices with the corresponding triangles + point indexes
    std::vector<trianglepointindex> tps;

    //store the vertices references
    std::vector<vec3d*> verts;

    //store the keyframes and their corresponding verts
    std::vector<std::vector<vec3d>> kf_points;

    void load() {
      for (auto b : tps) {
        //TODO
        //std::cout << "triIndex: " << b.triIndex << " pointIndex: " << b.pointIndex << std::endl;
        verts.push_back(model->tris[b.triIndex].p[b.pointIndex]);
      }
    }

  private:
    IDrzEngine* engine;
    Model* model;
    bool isAnimating;
    uint16_t currentAnimationIndex;
    float currentAnimationDuration; //in seconds
    std::queue<animkeyduration> animQueue;
    uint32_t animationStartTime; //in ms

    //move the vertices according toward current animation keyframe using speed as a factor
    void _updateAnimation() {
      
      auto now = engine->Now();
 
      float elapsedTime = (now - animationStartTime)/1000.0f;

      float t = elapsedTime/(currentAnimationDuration*1000);

      if (t >= 1.0f) {
        t = 1.0f;
        isAnimating = false;
      }
      for (size_t i = 0; i < verts.size(); ++i) {
        vec3d* v = verts[i];
        const vec3d& start = *v;
        const vec3d& target = kf_points[currentAnimationIndex][i];
        v->x = start.x + (target.x - start.x) * t;
        v->y = start.y + (target.y - start.y) * t;
        v->z = start.z + (target.z - start.z) * t;
      }
       if (!isAnimating) {
        //TODO: add debug message
        //std::cout << "Animation " << currentAnimationIndex << " finished" << std::endl;

        // Ensure final position is exactly the target position
        for (size_t i = 0; i < verts.size(); ++i) {
          vec3d* v = verts[i];
          const vec3d& target = kf_points[currentAnimationIndex][i];
          *v = target;
        }
      }
    }
};