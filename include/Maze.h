#ifndef MAZE_H_
#define MAZE_H_

#include <QVector>
#include <QImage>
#include <QString>
#include <QDebug>
#include <cmath>
#include <iostream>
#include <ngl/ShaderLib.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/Transformation.h>
#include <QPoint>
#include <ngl/Vec4.h>
#include "Coins.h"
#include <QPoint>

class Maze
{
public:
    explicit Maze(int size = 15);
    void loadMaze(const QString& imagePath);
    void printMazeGrid();
    const QVector<QVector<int>>& getMazeGrid() const;
    void drawMaze(const ngl::Mat4& m_projection, const ngl::Mat4& m_view);
    void processArray();
    void rotateMatrixLeft();
    void rotateMatrixRight();
    int getCameraGridX() const;
    int getCameraGridY() const;
    void setCameraGridX(int x);
    void setCameraGridY(int y);
    int getGhostGridX() const;
    int getGhostGridY() const;
    void setGhostGridX(int x);
    void setGhostGridY(int y);
    std::vector<ngl::Transformation> cubeTransformations;
    Coins m_coins;
    std::pair<int, int> getCameraGridInitialPosition() const { return{cameraGridX, cameraGridY}; }
    void setMazeGrid(const QVector<QVector<int>>& grid);
    QPoint getSelectedPosition() const;
    void removeCoins(float x, float z);

private:
    QVector<QVector<int>> mazeGrid;
    int mazeSize;
    QPoint cameraPosition;
    QPoint selectedPosition;
    std::vector<ngl::Vec3> coinPositions;
    float xSpacing = 0.5f;
    float zSpacing = -0.5f;
    float baseX = 0.0f;
    float baseZ = 3.5f;
    int cameraGridX, cameraGridY;
    int ghostGridX, ghostGridY;
};

#endif