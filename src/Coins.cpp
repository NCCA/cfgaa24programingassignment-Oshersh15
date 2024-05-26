//#include "Maze.h"
#include "Coins.h"

Coins::Coins(float baseX, float baseZ, float xSpacing, float zSpacing)
        : m_baseX(baseX), m_baseZ(baseZ), m_xSpacing(xSpacing), m_zSpacing(zSpacing) {}

void Coins::placeCoins(float x, float z)
{
    ngl::Transformation transform;
    transform.setPosition(x, 0.0f, z);
    transform.setScale(0.3f,0.3f,0.3f);
    transform.setRotation(90.0f,0.0f,0.0f);
    coinTransformations.push_back(transform);
}

void Coins::removeCoins(float x, float z)
{
    for(auto it = coinTransformations.begin(); it != coinTransformations.end(); ++it)
    {
        if(it->getPosition().m_x == x && it->getPosition().m_z == z)
        {
            it = coinTransformations.erase(it);
        }
    }
}
