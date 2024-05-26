#ifndef NGLSCENE_H_
#define NGLSCENE_H_

#include <ngl/Transformation.h>
#include "WindowParams.h"
#include <QOpenGLWindow>
#include <QLabel>
#include <QDebug>
#include <QMouseEvent>
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include <QDir>
#include <cmath>
#include <random>
#include <algorithm>
#include "Maze.h"
#include "Coins.h"
#include "Ghost.h"

//----------------------------------------------------------------------------------------------------------------------
/// @file NGLScene.h
/// @brief this class inherits from the Qt OpenGLWindow and allows us to use NGL to draw OpenGL
/// @author Jonathan Macey
/// @version 1.0
/// @date 10/9/13
/// Revision History :
/// This is an initial version used for the new NGL6 / Qt 5 demos
/// @class NGLScene
/// @brief our main glwindow widget for NGL applications all drawing elements are
/// put in this file
//----------------------------------------------------------------------------------------------------------------------

class NGLScene : public QOpenGLWindow
{
Q_OBJECT


public:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief ctor for our NGL drawing class
    /// @param [in] parent the parent window to the class
    //----------------------------------------------------------------------------------------------------------------------
    NGLScene();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief dtor must close down ngl and release OpenGL resources
    //----------------------------------------------------------------------------------------------------------------------
    ~NGLScene() override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the initialize class is called once when the window is created and we have a valid GL context
    /// use this to setup any default GL stuff
    //----------------------------------------------------------------------------------------------------------------------
    void initializeGL() override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this is called everytime we want to draw the scene
    //----------------------------------------------------------------------------------------------------------------------
    void paintGL() override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this is called everytime we resize the window
    //----------------------------------------------------------------------------------------------------------------------
    void resizeGL(int _w, int _h) override;

    void renderMaze();
    bool isFullScreen = false;
    void GameOver();
    void updateMazeGrid();
    void updateCameraPositionInGrid();
    void updateGhostPositionInGRid();
    void updateCameraPosition();

private:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the windows params such as mouse and rotations etc
    //----------------------------------------------------------------------------------------------------------------------
    WinParams m_win;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief used to store the global mouse transforms
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Mat4 m_mouseGlobalTX;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Our Camera
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Mat4 m_view;
    ngl::Mat4 m_project;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief transformation stack for the gl transformations etc
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Transformation m_transform;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the model position for mouse movement
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Vec3 m_modelPos;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a simple light use to illuminate the screen
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Vec4 m_lightPos;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief timer id for the animation timer
    //----------------------------------------------------------------------------------------------------------------------
    int m_lightTimer;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief flag to indicate if were animating the light
    //----------------------------------------------------------------------------------------------------------------------
    bool m_animate;
    //----------------------------------------------------------------------------------------------------------------------
    /// the angle of the light updated when the timer triggers to make it rotate
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Real m_lightAngle;

    //----------------------------------------------------------------------------------------------------------------------
    /// @brief method to load transform matrices to the shader
    //----------------------------------------------------------------------------------------------------------------------
    void loadMatricesToShader();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Qt Event called when a key is pressed
    /// @param [in] _event the Qt event to query for size etc
    //----------------------------------------------------------------------------------------------------------------------
    void keyPressEvent(QKeyEvent *_event) override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called every time a mouse is moved
    /// @param _event the Qt Event structure
    //----------------------------------------------------------------------------------------------------------------------
    void mouseMoveEvent (QMouseEvent * _event ) override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called everytime the mouse button is pressed
    /// inherited from QObject and overridden here.
    /// @param _event the Qt Event structure
    //----------------------------------------------------------------------------------------------------------------------
    void mousePressEvent ( QMouseEvent *_event) override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called everytime the mouse button is released
    /// inherited from QObject and overridden here.
    /// @param _event the Qt Event structure
    //----------------------------------------------------------------------------------------------------------------------
    void mouseReleaseEvent ( QMouseEvent *_event ) override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called everytime the mouse wheel is moved
    /// inherited from QObject and overridden here.
    /// @param _event the Qt Event structure
    //----------------------------------------------------------------------------------------------------------------------
    void wheelEvent( QWheelEvent *_event) override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief called when the timer is triggered
    //----------------------------------------------------------------------------------------------------------------------
    void timerEvent(QTimerEvent *_event) override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a method to update the light
    //----------------------------------------------------------------------------------------------------------------------
    void updateLight();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief draw our scene passing in the shader to use
    /// @param[in] _shader the name of the shader to use when drawing
    //----------------------------------------------------------------------------------------------------------------------
    void drawScene(const std::string &_shader);

    std::unique_ptr<Maze> m_maze;
    std::unique_ptr<Coins> m_coins;
    std::unique_ptr<Ghost> m_ghost;
    bool run = false;

    ngl::Vec3 m_cameraPosition;
    ngl::Vec3 m_cameraForward;
    ngl::Vec3 m_cameraRight;
    float m_cameraYaw;
    QVector<QVector<int>> m_mazeMatrix;
    QVector<QVector<int>> mazeGrid;
    float baseX = 0.0f;
    float baseZ = 3.5f;
    float lastxPosition;
    float lastzPosition;
    float xSpacing = 0.5f;  // Horizontal spacing between cubes
    float zSpacing = -0.5f;
    int lastRotation = 0;
    bool hasRun = false;
    int cameraGridX;
    int cameraGridY;

};





#endif