#ifndef COINS_H_
#define COINS_H_

#include <QVector>
#include <ngl/Transformation.h>

class Coins
{
public:
    Coins(float baseX, float baseZ, float xSpacing, float zSpacing);
    void placeCoins(float x, float z);
    std::vector<ngl::Transformation> coinTransformations;
    void removeCoins(float x, float z);

private:
    float m_baseX;
    float m_baseZ;
    float m_xSpacing;
    float m_zSpacing;
};
#endif