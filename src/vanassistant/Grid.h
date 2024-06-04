#pragma once

#include "../../include/IDrzEngine.h"

class Grid {
  public:
  
    Grid(IDrzEngine* engine, float depth, float height, int size) : engine(engine), gridDepth(depth), gridHeight(height), gridSize(size) {
      // Constructor
    }
 
    void Update(float elapsedTime) {
      //TODO
    }

    void Render() {
      //TODO
    }

  private:
    float gridDepth = 1000.0f; // Depth of the grid in the Z direction
    float gridHeight = 120.0f; // Height of the grid in the Y direction
    int gridSize = 12; // Number of grid squares in the X and Z directions
    IDrzEngine* engine;
};
