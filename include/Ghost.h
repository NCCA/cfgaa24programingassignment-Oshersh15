#ifndef GHOST_H_
#define GHOST_H_

#include <vector>
#include <ngl/Transformation.h>
#include <QVector>
#include "Maze.h"

class Ghost
{
public:
    Ghost(float baseX, float baseZ, float xSpacing, float zSpacing, const Maze& maze);
    std::vector<ngl::Transformation> sphereTransformations;
    void placeSphere(float x, float y, float z);
    float sphereInitialxPosition;
    float sphereInitialzPosition;
    float sphereLastxPosition;
    float sphereLastzPosition;
    std::pair<int, int> findPathCorners();
    std::pair<std::pair<int, int>, std::pair<int, int>> findShortestPath(QVector<QVector<int>>& mazeGrid, int cameraGridX, int cameraGridY, float m_cameraYaw);
    using GameOverCallback = std::function<void()>;
    void setGameOverCallback(GameOverCallback callback);
    void setPosition(int x, int y);

private:
    float m_baseX;
    float m_baseZ;
    float m_xSpacing;
    float m_zSpacing;

    const Maze* m_maze;
    float shortest = std::numeric_limits<float>::max();
    int currentShortestX;
    int currentShortestY;
    float xSpacing = 0.5f;  // Horizontal spacing between cubes
    float zSpacing = -0.5f;
    GameOverCallback gameOverCallback;
    int selectedX, selectedY;
};

#endif