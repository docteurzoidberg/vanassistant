#ifndef MODEL_CUBE_H
#define MODEL_CUBE_H

#include <vector>
#include <tuple>

const std::vector<std::tuple<float, float, float>> cubeVertices = {
    {-0.5, -0.5, -0.5},  // 0
    {0.5, -0.5, -0.5},   // 1
    {0.5, 0.5, -0.5},    // 2
    {-0.5, 0.5, -0.5},   // 3
    {-0.5, -0.5, 0.5},   // 4
    {0.5, -0.5, 0.5},    // 5
    {0.5, 0.5, 0.5},     // 6
    {-0.5, 0.5, 0.5}     // 7
};

const std::vector<std::tuple<int, int, int>> cubeFaces = {
    {0, 1, 2}, {0, 2, 3}, // Front face
    {4, 5, 6}, {4, 6, 7}, // Back face
    {0, 1, 5}, {0, 5, 4}, // Bottom face
    {1, 2, 6}, {1, 6, 5}, // Right face
    {2, 3, 7}, {2, 7, 6}, // Top face
    {3, 0, 4}, {3, 4, 7}  // Left face
};

#endif // MODEL_CUBE_H
