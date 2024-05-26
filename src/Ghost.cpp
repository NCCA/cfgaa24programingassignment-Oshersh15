#include "Ghost.h"
#include <random>
#include <iostream>

Ghost::Ghost(float baseX, float baseZ, float xSpacing, float zSpacing, const Maze& maze)
        : m_baseX(baseX), m_baseZ(baseZ), m_xSpacing(xSpacing), m_zSpacing(zSpacing), m_maze(&maze) {}

void Ghost::setPosition(int x, int y)
{
    selectedX = x;
    selectedY = y;
}

std::pair<int, int> Ghost::findPathCorners()
{
    if (!m_maze) return {-1, -1};

    const QVector<QVector<int>>& mazeGrid = m_maze->getMazeGrid();
    std::cout << "finding path corners" << std::endl;
    QVector<QVector<int>> corners;
    int n = mazeGrid.size();
    int layer = 0;
    bool found = false;

    while(layer < n / 2 && !found)
    {
        for(int i = layer; i < n-layer; ++i)
        {
            if(mazeGrid[layer][i] == 0)
            {
                corners.push_back(QVector<int>{layer, i});
                found = true;
            }
            if(mazeGrid[n-layer-1][i] == 0)
            {
                corners.push_back(QVector<int>{n-layer-1, i});
                found = true;
            }
            if(mazeGrid[i][layer] == 0)
            {
                corners.push_back(QVector<int>{i, layer});
                found = true;
            }
            if(mazeGrid[i][n-layer-1] == 0)
            {
                corners.push_back(QVector<int>{i, n-layer-1});
                found = true;
            }
        }
        layer++;
    }
    std::pair<int, int> selectedCoords = {-1, -1};
    if(!corners.isEmpty())
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, corners.size() - 1);
        int index = distrib(gen);
        selectedCoords = {corners[index][0], corners[index][1]};
        sphereInitialxPosition = m_baseX + (float)selectedCoords.first * m_xSpacing;
        sphereInitialzPosition = m_baseZ + (float)selectedCoords.second * m_zSpacing;
        selectedX = selectedCoords.first;
        selectedY = selectedCoords.second;
        ngl::Transformation transform;
        transform.setPosition(sphereInitialxPosition, 0.0f, sphereInitialzPosition);
        transform.setScale(0.3f,0.3f,0.3f);
        sphereTransformations.push_back(transform);
        sphereLastxPosition = sphereInitialxPosition;
        sphereLastzPosition = sphereInitialzPosition;
    }
    return selectedCoords;
}

void Ghost::placeSphere(float x, float y, float z)
{
    sphereTransformations.clear();
    ngl::Transformation transform;
    transform.setPosition(x, y, z);
    transform.setScale(0.3f,0.3f,0.3f);
    sphereTransformations.push_back(transform);
}

std::pair<std::pair<int, int>, std::pair<int,int>> Ghost::findShortestPath(QVector<QVector<int>> &mazeGrid, int cameraGridX, int cameraGridY, float m_cameraYaw)
{
    float distance;
    int chosen;
    int forwardX, forwardY, backwardX, backwardY, leftX, leftY, rightX, rightY;
    switch((int)m_cameraYaw)
    {
        case(90):
            forwardX = -1; forwardY = 0;
            backwardX = 1; backwardY = 0;
            leftX = 0; leftY = 1;
            rightX = 0; rightY = -1;
            break;
        case(180):
            forwardX = 0; forwardY = -1;
            backwardX = 0; backwardY = 1;
            leftX = -1; leftY = 0;
            rightX = 1; rightY = 0;
            break;
        case(270):
            forwardX = 1; forwardY = 0;
            backwardX = -1; backwardY = 0;
            leftX = 0; leftY = -1;
            rightX = 0; rightY = 1;
            break;
        default:
            forwardX = 0; forwardY = 1;
            backwardX = 0; backwardY = -1;
            leftX = 1; leftY = 0;
            rightX = -1; rightY = 0;
            break;
    }
    if (mazeGrid[selectedX + leftX][selectedY + leftY] == 0 || mazeGrid[selectedX + leftX][selectedY + leftY] == 2) {
        if (mazeGrid[selectedX + leftX][selectedY + leftY] == 2)
        {
            if(gameOverCallback) gameOverCallback();
        }
        distance = sqrt(pow(((selectedX + leftX) - cameraGridX), 2) + pow((selectedY + leftY - cameraGridY), 2));
        if (shortest > distance) {
            shortest = distance;
            currentShortestX = selectedX + leftX;
            currentShortestY = selectedY + leftY;
            chosen = 1;
        }
    }
    if (mazeGrid[selectedX + rightX][selectedY + rightY] == 0 || mazeGrid[selectedX + rightX][selectedY + rightY] == 2) {
        if (mazeGrid[selectedX + rightX][selectedY + rightY] == 2)
        {
            if(gameOverCallback) gameOverCallback();
        }
        distance = sqrt(pow(((selectedX + rightX) - cameraGridX), 2) + pow((selectedY + rightY - cameraGridY), 2));
        if (shortest > distance) {
            shortest = distance;
            currentShortestX = selectedX + rightX;
            currentShortestY = selectedY + rightY;
            chosen = 2;
        }
    }
    if (mazeGrid[selectedX + forwardX][selectedY + forwardY] == 0 || mazeGrid[selectedX + forwardX][selectedY + forwardY] == 2) {
        if (mazeGrid[selectedX + forwardX][selectedY + forwardY] == 2)
        {
            if(gameOverCallback) gameOverCallback();
        }
        distance = sqrt(pow((selectedX +forwardX - cameraGridX), 2) + pow(((selectedY + forwardY) - cameraGridY), 2));
        if (shortest > distance) {
            shortest = distance;
            currentShortestX = selectedX + forwardX;
            currentShortestY = selectedY + forwardY;
            chosen = 3;
        }
    }
    if (mazeGrid[selectedX + backwardX][selectedY + backwardY] == 0 || mazeGrid[selectedX + backwardX][selectedY + backwardY] == 2) {
        if (mazeGrid[selectedX + backwardX][selectedY + backwardY] == 2)
        {
            if(gameOverCallback) gameOverCallback();
        }
        distance = sqrt(pow((selectedX + backwardX - cameraGridX), 2) + pow(((selectedY + backwardY) - cameraGridY), 2));
        if (shortest > distance) {
            shortest = distance;
            currentShortestX = selectedX + backwardX;
            currentShortestY = selectedY + backwardY;
            chosen = 4;
        }
    }
    float xPosition;
    float zPosition;
    switch(chosen)
    {
        case(1):
            xPosition = sphereLastxPosition + xSpacing;
            zPosition = sphereLastzPosition;
            break;
        case(2):
            xPosition = sphereLastxPosition - xSpacing;
            zPosition = sphereLastzPosition;
            break;
        case(3):
            xPosition = sphereLastxPosition;
            zPosition = sphereLastzPosition + zSpacing;
            break;
        case(4):
            xPosition = sphereLastxPosition;
            zPosition = sphereLastzPosition - zSpacing;
            break;
        default:
            break;
    }
    std::cout << "chosen: " << chosen << std::endl;
    placeSphere(xPosition, 0.0f, zPosition);
    sphereLastxPosition = xPosition;
    sphereLastzPosition = zPosition;
    int prevX = selectedX;
    int prevY = selectedY;
    selectedX = currentShortestX;
    selectedY = currentShortestY;
    shortest = std::numeric_limits<float>::max();
    return {{selectedX, selectedY}, {prevX, prevY}};
}

void Ghost::setGameOverCallback(Ghost::GameOverCallback callback)
{
    gameOverCallback = callback;
}


