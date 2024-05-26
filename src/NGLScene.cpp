#include "NGLScene.h"
#include <QGuiApplication>
#include <QApplication>

NGLScene::NGLScene()
{
    setTitle("first-person POV PAC-MAN");
    //m_animate = true;
}

NGLScene::~NGLScene()
{
    std::cout << "Shutting down NGL, removing VAO's and Shaders\n";
}

void NGLScene::updateCameraPosition()
{
    float radYaw = ngl::radians(m_cameraYaw);
    m_cameraForward = ngl::Vec3(cos(radYaw), 0, sin(radYaw));

    m_view = ngl::lookAt(m_cameraPosition, m_cameraPosition + m_cameraForward, ngl::Vec3(0,1,0));
    while(m_cameraYaw >= 360.0f)
        m_cameraYaw -= 360.0f;
    while(m_cameraYaw < 0.0f)
        m_cameraYaw += 360.0f;
    std::cout << "camera yaw" << m_cameraYaw << std::endl;
    loadMatricesToShader();
}

void NGLScene::updateMazeGrid()
{
    mazeGrid = m_maze->getMazeGrid();
}

void NGLScene::updateCameraPositionInGrid()
{
    cameraGridX = m_maze->getCameraGridX();
    cameraGridY = m_maze->getCameraGridY();
}

void NGLScene::updateGhostPositionInGRid()
{
    int ghostX = m_maze->getCameraGridX();
    int ghostY = m_maze->getCameraGridY();
    m_ghost->setPosition(ghostX, ghostY);
}

void NGLScene::resizeGL(int _w, int _h)
{
    m_project = ngl::perspective(45.0f, static_cast<float>(_w) / _h, 0.05f, 350.0f);
    m_win.width = static_cast<int>(_w * devicePixelRatio());
    m_win.height = static_cast<int>(_h * devicePixelRatio());
}

constexpr auto shaderProgram = "PBR";
void NGLScene::initializeGL()
{
    // we need to initialise the NGL lib which will load all of the OpenGL functions, this must
    // be done once we have a valid GL context but before we call any GL commands. If we dont do
    // this everything will crash
    ngl::NGLInit::initialize();
    m_cameraPosition = ngl::Vec3(0.0f,0.0f,0.0f); //starting camera position
    m_cameraYaw = 0.0f; //facing forward
    updateCameraPosition();
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f); // Grey Background
    // enable depth testing for drawing
    glEnable(GL_DEPTH_TEST);
    m_lightAngle = 0.0;
    m_lightPos.set(sinf(m_lightAngle), 2, cosf(m_lightAngle));
    // now to load the shader and set the values
    // we are creating a shader called PBR to save typos
    // in the code create some constexpr
    constexpr auto vertexShader = "PBRVertex";
    constexpr auto fragShader = "PBRFragment";
    // create the shader program
    ngl::ShaderLib::createShaderProgram(shaderProgram);
    // now we are going to create empty shaders for Frag and Vert
    ngl::ShaderLib::attachShader(vertexShader, ngl::ShaderType::VERTEX);
    ngl::ShaderLib::attachShader(fragShader, ngl::ShaderType::FRAGMENT);
    // attach the source
    ngl::ShaderLib::loadShaderSource(vertexShader, "shaders/PBRVertex.glsl");
    ngl::ShaderLib::loadShaderSource(fragShader, "shaders/PBRFragment.glsl");
    // compile the shaders
    ngl::ShaderLib::compileShader(vertexShader);
    ngl::ShaderLib::compileShader(fragShader);
    // add them to the program
    ngl::ShaderLib::attachShaderToProgram(shaderProgram, vertexShader);
    ngl::ShaderLib::attachShaderToProgram(shaderProgram, fragShader);
    // now we have associated that data we can link the shader
    ngl::ShaderLib::linkProgramObject(shaderProgram);
    // and make it active ready to load values
    ngl::ShaderLib::use(shaderProgram);
    // We now create our view matrix for a static camera
    ngl::Vec3 from(5.0f, 5.0f, 5.0f);
    ngl::Vec3 to(0.0f, 0.0f, 0.0f);
    ngl::Vec3 up(0.0f, 2.0f, 0.0f);
    // now load to our new camera
    m_view = ngl::lookAt(from, to, up);
    m_project = ngl::perspective(45.0f, 1024.0f/720.0f, 0.05f, 350.0f);

    ngl::ShaderLib::setUniform("camPos", from);
    // now a light
    m_lightPos.set(0.0, 2.0f, 2.0f, 1.0f);
    // setup the default shader material and light porerties
    // these are "uniform" so will retain their values
    ngl::ShaderLib::setUniform("lightPosition", m_lightPos.toVec3());
    ngl::ShaderLib::setUniform("lightColor", 400.0f, 400.0f, 400.0f);
    ngl::ShaderLib::setUniform("exposure", 2.2f);
    ngl::ShaderLib::setUniform("albedo", 0.6f, 0.368f, 0.77f);

    ngl::ShaderLib::setUniform("metallic", 0.0f);
    ngl::ShaderLib::setUniform("roughness", 0.5f);
    ngl::ShaderLib::setUniform("ao", 0.2f);

    ngl::VAOPrimitives::createSphere("sphere", 0.5f, 50);
    ngl::VAOPrimitives::createCylinder("cylinder", 0.095f, 0.1, 40, 10);
    ngl::VAOPrimitives::createTrianglePlane("plane", 7.5, 7.5, 80, 80, ngl::Vec3(0, 1, 0));
    m_lightTimer = startTimer(40);
    m_maze = std::make_unique<Maze>(15);
    m_maze->loadMaze("image/Maze.png");
    m_ghost = std::make_unique<Ghost>(0.0f, 3.5f, 0.5f, -0.5f, *m_maze);
    std::pair<int, int> ghostCoords = m_ghost->findPathCorners();
    std::cout<<"ghostCoords.first"<<ghostCoords.first<<"ghostCoords.second"<<ghostCoords.second<<std::endl;
    updateMazeGrid();
    mazeGrid[ghostCoords.first][ghostCoords.second] = 3;
    m_maze->setMazeGrid(mazeGrid);
    m_maze->printMazeGrid();
    m_ghost->setGameOverCallback([this]() {this->GameOver();});
}

void NGLScene::loadMatricesToShader()
{
    ngl::ShaderLib::use("PBR");
    struct transform
    {
        ngl::Mat4 MVP;
        ngl::Mat4 normalMatrix;
        ngl::Mat4 M;
    };
    transform t;
    t.M = m_view * m_mouseGlobalTX * m_transform.getMatrix();
    t.MVP = m_project * t.M;
    t.normalMatrix = t.M;
    t.normalMatrix.inverse().transpose();
    ngl::ShaderLib::setUniformBuffer("TransformUBO", sizeof(transform), &t.MVP.m_00);
    ngl::ShaderLib::setUniform("lightPosition", (m_mouseGlobalTX * m_lightPos).toVec3());
}

void NGLScene::renderMaze() {
    for (const auto& transform : m_maze->cubeTransformations) {
        m_transform = transform;
        ngl::ShaderLib::setUniform("albedo", 0.6f, 0.368f, 0.77f);
        loadMatricesToShader();
        ngl::VAOPrimitives::draw("cube");
    }
    for (const auto& transform : m_ghost->sphereTransformations)
    {
        m_transform = transform;
        loadMatricesToShader();
        ngl::ShaderLib::setUniform("albedo", 0.96f, 0.18f, 0.8f);
        ngl::VAOPrimitives::draw("sphere");
    }
    for (const auto& coinTransform : m_maze->m_coins.coinTransformations)
    {
        m_transform = coinTransform;
        ngl::ShaderLib::setUniform("albedo", 0.49f, 0.4f, 0.03f);
        loadMatricesToShader();
        ngl::VAOPrimitives::draw("cylinder");
    }
}

void NGLScene::drawScene(const std::string &_shader) {
    ngl::ShaderLib::use(_shader);
    // Rotation based on the mouse position for our global transform
    ngl::Mat4 rotX = ngl::Mat4::rotateX(m_win.spinXFace);
    ngl::Mat4 rotY = ngl::Mat4::rotateY(m_win.spinYFace);
    // multiply the rotations
    m_mouseGlobalTX = rotY * rotX;
    // add the translations
    m_mouseGlobalTX.m_m[3][0] = m_modelPos.m_x;
    m_mouseGlobalTX.m_m[3][1] = m_modelPos.m_y;
    m_mouseGlobalTX.m_m[3][2] = m_modelPos.m_z;
    if (!run)
    {
        m_maze->processArray();
        auto[x,y] = m_maze->getCameraGridInitialPosition();
        cameraGridX = x;
        cameraGridY = y;
        float xPosition = 0.0f + (float)x * xSpacing;
        float zPosition = 3.5f + (float)y * zSpacing;
        m_cameraPosition = ngl::Vec3(xPosition, 0.2, zPosition);
        m_cameraForward = ngl::Vec3(1,0,0);
        updateCameraPosition();
        run = true;
    }
    renderMaze();
    m_transform.reset();
    {
        m_transform.setPosition(3.5f, -0.25f, 0.0f);
        ngl::ShaderLib::setUniform("albedo", 0.6f, 0.368f, 0.77f);
        loadMatricesToShader();
        ngl::VAOPrimitives::draw("plane");
    } // and before a pop
}

void NGLScene::paintGL()
{
    // clear the screen and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, m_win.width, m_win.height);
    drawScene("PBR");
}

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
    float rotateSpeed = 5.0f;
    int dx;
    int dy = 0;
    static bool move = false;
    float xPosition;
    float zPosition;

    if(!hasRun)
    {
        m_maze->loadMaze("image/Maze.png");
        updateMazeGrid();
        hasRun = true;
    }
    // this method is called every time the main window recives a key event.
    // we then switch on the key value and set the camera in the GLWindow
    switch (_event->key())
    {
        // escape key to quite
        case Qt::Key_Escape:
            QGuiApplication::exit(EXIT_SUCCESS);
            break;
            // turn on wirframe rendering
        case Qt::Key_Up:
            if((int)m_cameraYaw<45)
                m_cameraYaw = 0;
            else if((int)m_cameraYaw<135)
                m_cameraYaw = 90;
            else if((int)m_cameraYaw<225)
                m_cameraYaw = 180;
            else if((int)m_cameraYaw<315)
                m_cameraYaw = 270;
            else
                m_cameraYaw = 0;
            updateCameraPosition();
            m_maze->printMazeGrid();
            if((int)m_cameraYaw % 90 == 0)
            {
                move = true;
                dx = 1;
                if(lastRotation != (int)m_cameraYaw)
                {
                    int angleDifference = ((int)m_cameraYaw - lastRotation + 360) % 360;
                    bool clockwiseRotation = angleDifference <= 180;
                    if(clockwiseRotation)
                    {
                        m_maze->setMazeGrid(mazeGrid);
                        m_maze->setCameraGridX(cameraGridX);
                        m_maze->setCameraGridY(cameraGridY);
                        m_maze->rotateMatrixLeft();
                    }
                    else
                    {
                        m_maze->setMazeGrid(mazeGrid);
                        m_maze->setCameraGridX(cameraGridX);
                        m_maze->setCameraGridY(cameraGridY);
                        m_maze->rotateMatrixRight();
                        std::cout << "rotate matrix right" << std::endl;
                    }
                    updateGhostPositionInGRid();
                    updateCameraPositionInGrid();
                    m_ghost->setPosition(m_maze->getGhostGridX(), m_maze->getGhostGridY());
                    updateMazeGrid();
                }
                lastRotation = (int)m_cameraYaw;

                if (mazeGrid[cameraGridX + dx][cameraGridY + dy] != 1 && move)
                {
                    if(mazeGrid[cameraGridX + 2][cameraGridY + dy] == 3)
                    {
                        GameOver();
                    }
                    else
                    {
                        mazeGrid[cameraGridX][cameraGridY] = 0;
                        m_maze->setMazeGrid(mazeGrid);
                        cameraGridX += dx;
                        switch ((int) m_cameraYaw) {
                            case (90):
                                xPosition = lastxPosition;
                                zPosition = lastzPosition + xSpacing;
                                break;
                            case (180):
                                xPosition = lastxPosition + zSpacing;
                                zPosition = lastzPosition;
                                break;
                            case (270):
                                xPosition = lastxPosition;
                                zPosition = lastzPosition + zSpacing;
                                break;
                            default:
                                xPosition = baseX + (float) cameraGridX * xSpacing;
                                zPosition = baseZ + (float) cameraGridY * zSpacing;
                                break;
                        }
                        m_maze->removeCoins(lastxPosition, lastzPosition);
                        lastxPosition = xPosition;
                        lastzPosition = zPosition;
                        m_cameraPosition = ngl::Vec3(xPosition, 0.2, zPosition);

                        updateCameraPosition();
                        mazeGrid[cameraGridX][cameraGridY] = 2;
                        std::cout << cameraGridX << " " << cameraGridY << std::endl;
                        m_maze->setMazeGrid(mazeGrid);

                        auto [newGhostPos, prevGhostPos] = m_ghost->findShortestPath(mazeGrid, cameraGridX, cameraGridY, m_cameraYaw);
                        mazeGrid[prevGhostPos.first][prevGhostPos.second] = 0;
                        mazeGrid[newGhostPos.first][newGhostPos.second] = 3;
                        m_maze->setGhostGridX(newGhostPos.first);
                        m_maze->setGhostGridY(newGhostPos.second);
                        m_maze->setMazeGrid(mazeGrid);
                        m_maze->printMazeGrid();
                    }
                }
            }
            break;
        case Qt::Key_Left:
            m_cameraYaw -= rotateSpeed;
            updateCameraPosition();
            if((int)m_cameraYaw % 90 == 0 && lastRotation != (int)m_cameraYaw)
            {
                m_maze->setMazeGrid(mazeGrid);
                m_maze->setCameraGridX(cameraGridX);
                m_maze->setCameraGridY(cameraGridY);
                m_maze->rotateMatrixRight();
                updateGhostPositionInGRid();
                updateCameraPositionInGrid();
                std::cout << "reverse" << std::endl;
                m_ghost->setPosition(m_maze->getGhostGridX(), m_maze->getGhostGridY());
                updateMazeGrid();
                m_maze->printMazeGrid();
                lastRotation = (int)m_cameraYaw;

            }
            break;
        case Qt::Key_Right:
            m_cameraYaw += rotateSpeed;
            updateCameraPosition();
            if((int)m_cameraYaw % 90 == 0 && lastRotation != (int)m_cameraYaw)
            {
                m_maze->setMazeGrid(mazeGrid);
                m_maze->setCameraGridX(cameraGridX);
                m_maze->setCameraGridY(cameraGridY);
                m_maze->rotateMatrixLeft();
                updateCameraPosition();
                updateCameraPositionInGrid();
                std::cout << "reverse" << std::endl;
                m_ghost->setPosition(m_maze->getGhostGridX(), m_maze->getGhostGridY());
                updateMazeGrid();
                m_maze->printMazeGrid();
                lastRotation = (int)m_cameraYaw;
            }
            break;
            // show full screen
        case Qt::Key_F:
            if (isFullScreen) {
                showNormal();
                isFullScreen = !isFullScreen;
            }
            else {
                showFullScreen();
                isFullScreen = !isFullScreen;
            }
            break;
//  case Qt::Key_Space:
//    m_animate ^= true;
//    break;
        default:
            break;
    }
    update();
}

void NGLScene::GameOver()
{
    QPixmap pixmap("image/game.jpg");
    QLabel *label = new QLabel;
    label->setPixmap(pixmap);
    label->resize(pixmap.size());
    label->setScaledContents(true);
    label->resize(300, 300);
    label->move(500,500);
    label->setWindowTitle("");
    label->show();
    update();
}

void NGLScene::updateLight()
{
    // change the light angle
    m_lightAngle = 0.1f;

    // now set this value and load to the shader
    m_lightPos.set(ngl::Vec3(4.0f * cosf(m_lightAngle), 2.0f, 4.0f * sinf(m_lightAngle)));
}

void NGLScene::timerEvent(QTimerEvent *_event)
{
    // if the timer is the light timer call the update light method
    if (_event->timerId() == m_lightTimer && m_animate == true)
    {
        updateLight();
    }
    // re-draw GL
    update();
}
