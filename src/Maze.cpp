#include "Maze.h"


Maze::Maze(int size) : mazeSize(size), m_coins(baseX, baseZ, xSpacing, zSpacing)
{
    mazeGrid.resize(mazeSize);
    for(int i = 0; i < mazeSize; ++i)
    {
        mazeGrid[i].resize(mazeSize);
        std::fill(mazeGrid[i].begin(), mazeGrid[i].end(), 0);
    }
}

const QVector<QVector<int>>& Maze::getMazeGrid() const
{
    return mazeGrid;
}

int Maze::getCameraGridX() const
{
    return cameraGridX;
}

int Maze::getCameraGridY() const
{
    return cameraGridY;
}

void Maze::setCameraGridX(int x)
{
    cameraGridX = x;
}

void Maze::setCameraGridY(int y)
{
    cameraGridY = y;
}

int Maze::getGhostGridX() const
{
    return ghostGridX;
}

int Maze::getGhostGridY() const
{
    return  ghostGridY;
}

void Maze::setGhostGridX(int x)
{
    ghostGridX = x;
}

void Maze::setGhostGridY(int y)
{
    ghostGridY = y;
}

void Maze::loadMaze(const QString& imagePath)
{
    QImage image(imagePath);
    if(image.isNull())
    {
        qDebug() << "Failed to load maze image from:" << imagePath;
        mazeGrid = QVector<QVector<int>>();
        return;
    }
    image = image.convertToFormat(QImage::Format_RGB32);

    int cellWidth = image.width() / mazeSize;
    int cellHeight = image.height() / mazeSize;

    int midIndex = mazeSize / 2; //centre index for the grid
    bool foundStart = false;

    for (int gridY = 0; gridY < mazeSize; ++gridY)
    {
        for (int gridX = 0; gridX < mazeSize; ++gridX)
        {
            // Calculate the average color of the cell.
            int blackPixelCount = 0;
            for (int y = 0; y < cellHeight; ++y)
            {
                for (int x = 0; x < cellWidth; ++x)
                {
                    QRgb pixel = image.pixel(gridX * cellWidth + x, (mazeSize - 1 - gridY) * cellHeight + y);
                    if (qRed(pixel) == 0)  // Black pixel
                    {
                        blackPixelCount++;
                    }
                }
            }
            int totalPixels = cellWidth * cellHeight;
            if (blackPixelCount > totalPixels / 2)
            {
                mazeGrid[gridY][gridX] = 1;  // Wall
            }
            else
            {
                mazeGrid[gridY][gridX] = 0;  // Path
                if (!foundStart && abs(gridX - midIndex) <= 1 && abs(gridY-midIndex) <= 1)
                {
                    mazeGrid[gridY][gridX] = 2; //mark the start position
                    foundStart = true;
                }
            }
        }
    }
}

void Maze::setMazeGrid(const QVector<QVector<int>> &grid)
{
    mazeGrid = grid;
}

void Maze::processArray()
{
    float BaseX = 0.0f;
    float BaseZ = 3.5f;

    for (int a = 0; a < mazeGrid.size(); ++a)
    {
        for (int b = 0; b < mazeGrid[a].size(); ++b)
        {
            ngl::Transformation transform;
            float xPosition = BaseX + (float) a * xSpacing;
            float zPosition = BaseZ + (float) b * zSpacing;
            transform.setPosition(xPosition, 0.0f, zPosition);
            transform.setScale(0.5f, 0.5f, 0.5f);
            if (mazeGrid[a][b] == 1) {
                cubeTransformations.push_back(transform);
            }// and before a pop
            else if (mazeGrid[a][b] == 2)
            {
                cameraGridX = a;
                cameraGridY = b;
            }
            else
            {
                float coinXPosition = baseX + (float)a * xSpacing;
                float coinZPosition = baseZ + (float)b * zSpacing;
                m_coins.placeCoins(coinXPosition, coinZPosition);
            }
        }
    }
}

void Maze::rotateMatrixRight()
{
    QVector<QVector<int>> newGrid(mazeSize, QVector<int>(mazeSize, 0));
    for(int y = 0; y < mazeSize; ++y)
    {
        for(int x = 0; x < mazeSize; ++x)
        {
            newGrid[x][mazeSize - 1 - y] = mazeGrid[y][x];
        }
    }
    mazeGrid = newGrid;
    int temp = cameraGridX;
    cameraGridX = cameraGridY;
    cameraGridY = mazeSize - 1 - temp;

    temp = ghostGridX;
    ghostGridX = ghostGridY;
    ghostGridY = mazeSize - 1 - temp;
}

void Maze::rotateMatrixLeft()
{
    QVector<QVector<int>> newGrid(mazeSize, QVector<int>(mazeSize, 0));
    for(int y = 0; y < mazeSize; ++y)
    {
        for(int x = 0; x < mazeSize; ++x)
        {
            newGrid[mazeSize - 1 - x][y] = mazeGrid[y][x];
        }
    }
    mazeGrid = newGrid;
    int temp = cameraGridY;
    cameraGridY = cameraGridX;
    cameraGridX = mazeSize - 1 - temp;

    temp = ghostGridY;
    ghostGridY = ghostGridX;
    ghostGridX = mazeSize - 1 - temp;
}

void Maze::removeCoins(float x, float z)
{
    m_coins.removeCoins(x, z);
}

void Maze::printMazeGrid()
{
    qDebug() << "Maze Grid:";
    for (int a = 0; a < mazeGrid.size(); a++) {
        for (int b = 0; b < mazeGrid[a].size(); b++) {
            std::cout << mazeGrid[a][b] << " ";
        }
        std::cout << std::endl;
    }
}