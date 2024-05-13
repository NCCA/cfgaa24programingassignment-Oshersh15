#include "includes.h"
#include <QApplication>
#include <QMessageBox>
#include <QLabel>
#include <QPixmap>

NGLScene::NGLScene()
{
  setTitle("first-person POV PAC-MAN");
  //m_animate = true;
}

NGLScene::~NGLScene()
{
  std::cout << "Shutting down NGL, removing VAO's and Shaders\n";
}

bool hasRun = false;
void NGLScene::loadMaze()
{
    if (!hasRun) {
        QImage image("image/Maze.png");
        if (image.isNull()) {
            qDebug() << "Failed to load maze image.";
            mazeGrid = QVector<QVector<int>>();
            return;
        }
        image = image.convertToFormat(QImage::Format_RGB32);
        int mazeSize = 15;
        mazeGrid.resize(mazeSize);
        for(int i = 0; i < mazeSize; ++i)
        {
            mazeGrid[i].resize(mazeSize);
        }
        // Calculate the size of each cell in the original image.
        int cellWidth = image.width() / mazeSize;
        int cellHeight = image.height() / mazeSize;

        int midIndex = mazeSize / 2; //centre index for the grid
        bool foundStart = false;

        for (int gridY = 0; gridY < mazeSize; ++gridY) {
            QString row = "pixels: ";
            for (int gridX = 0; gridX < mazeSize; ++gridX) {
                // Calculate the average color of the cell.
                int blackPixelCount = 0;
                for (int y = 0; y < cellHeight; ++y) {
                    for (int x = 0; x < cellWidth; ++x) {
                        QRgb pixel = image.pixel(gridX * cellWidth + x, (mazeSize - 1 - gridY) * cellHeight + y);
                        if (qRed(pixel) == 0)  // Black pixel
                        {
                            blackPixelCount++;
                        }
                    }
                }
                // Determine if the cell is predominantly black or white.
                int totalPixels = cellWidth * cellHeight;
                if (blackPixelCount > totalPixels / 2)
                {
                    mazeGrid[gridY][gridX] = 1;  // Wall
                    row += "1 ";  // Majority black, cube should be placed.
                } else
                {
                    mazeGrid[gridY][gridX] = 0;  // Path
                    row += "0 "; // Majority white, cell is empty.
                    if (!foundStart && abs(gridX - midIndex) <= 1 && abs(gridY-midIndex) <= 1)
                    {
                        mazeGrid[gridY][gridX] = 2; //mark the start position
                        row += "2 ";
                        foundStart = true;
                    }
                }
            }
        }
        findPathCorners();
        hasRun = true;
    }
}

std::vector<ngl::Transformation> sphereTransformations;
std::vector<ngl::Transformation> coinTransformations;

void NGLScene::findPathCorners()
{
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
    if(!corners.isEmpty())
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, corners.size() - 1);
        int index = distrib(gen);
        selectedX = corners[index][0];
        selectedY = corners[index][1];
        mazeGrid[selectedX][selectedY] = 3;
        sphereInitialxPosition = baseX + selectedX * xSpacing;
        sphereInitialzPosition = baseZ + selectedY * zSpacing;
        std::cout<<"selectedX"<<selectedX<<"selectedY"<<selectedY<<std::endl;

        ngl::Transformation transform;
        transform.setPosition(sphereInitialxPosition, 0.0f, sphereInitialzPosition);
        transform.setScale(0.3f,0.3f,0.3f);
        sphereTransformations.push_back(transform);
        sphereLastxPosition = sphereInitialxPosition;
        sphereLastzPosition = sphereInitialzPosition;
        printMazeGrid();
    }
}

void NGLScene::placeSphere(float x, float y, float z)
{
    sphereTransformations.clear();
    ngl::Transformation transform;
    transform.setPosition(x, y, z);
    transform.setScale(0.3f,0.3f,0.3f);
    sphereTransformations.push_back(transform);
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
//  ngl::Texture texture("image/Maze.png");
//  m_textureName = texture.setTextureGL();
  // We now create our view matrix for a static camera
  ngl::Vec3 from(00.0f, 0.0f, 0.0f);
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

int cameraGridX;
int cameraGridY;
std::vector<ngl::Transformation> cubeTransformations;

void NGLScene::processArray() {
    if(!hasRun)
    {
        loadMaze();
    }
    float BaseX = 0.0f;
    float BaseZ = 3.5f;  // Starting Z position for the first row
    float coinxPosition;
    float coinzPosition;

    for (int a = 0; a < mazeGrid.size(); ++a)
    {
        for (int b = 0; b < mazeGrid[a].size(); ++b)
        {
            ngl::Transformation transform;
            float xPosition = BaseX + (float)a * xSpacing;
            float zPosition = BaseZ + (float)b * zSpacing;
            transform.setPosition(xPosition, 0.0f, zPosition);
            transform.setScale(0.5f, 0.5f, 0.5f);
            if (mazeGrid[a][b] == 1)
            {
                cubeTransformations.push_back(transform);
            }// and before a pop
            else if (mazeGrid[a][b] == 2)
            {
                m_cameraPosition = ngl::Vec3(xPosition, 0.2, zPosition);
                m_cameraForward = ngl::Vec3(1,0,0);
                updateCameraPosition();
                cameraGridX = a;
                cameraGridY = b;
            }
            else
            {
                coinxPosition = baseX + a * xSpacing;
                coinzPosition = baseZ + b * zSpacing;
                ngl::Transformation cointransform;
                cointransform.setPosition(coinxPosition, 0.0f, coinzPosition);
                cointransform.setScale(0.3f,0.3f,0.3f);
                cointransform.setRotation(90.0f,0.0f,0.0f);
                coinTransformations.push_back(cointransform);
                printMazeGrid();
            }
        }
    }
}

void NGLScene::renderMaze() {
    for (const auto& transform : cubeTransformations) {
        m_transform = transform;
        ngl::ShaderLib::setUniform("albedo", 0.6f, 0.368f, 0.77f);
        loadMatricesToShader();
        ngl::VAOPrimitives::draw("cube");
    }
    for (const auto& transform : sphereTransformations)
    {
        m_transform = transform;
        loadMatricesToShader();
        ngl::ShaderLib::setUniform("albedo", 0.96f, 0.18f, 0.8f);
        ngl::VAOPrimitives::draw("sphere");
    }
    for (const auto& cointransform : coinTransformations)
    {
        m_transform = cointransform;
//        glBindTexture(GL_TEXTURE_2D, m_textureName);
//        glPolygonMode(GL_FRONT_AND_BACK, m_polyMode);
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
        processArray();
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
//  loadMatricesToShader();
//  glBindTexture(GL_TEXTURE_2D, m_textureName);
//  glPolygonMode(GL_FRONT_AND_BACK, m_polyMode);
//  loadMatricesToShader();
  //ngl::VAOPrimitives::draw("cylinder");
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseMoveEvent(QMouseEvent *_event)
{
// note the method buttons() is the button state when event was called
// that is different from button() which is used to check which button was
// pressed when the mousePress/Release event is generated
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
  auto position = _event->position();
#else
  auto position = _event->pos();
#endif
  if (m_win.rotate && _event->buttons() == Qt::LeftButton)
  {
    int diffx = position.x() - m_win.origX;
    int diffy = position.y() - m_win.origY;
    m_win.spinXFace += static_cast<int>(0.5f * diffy);
    m_win.spinYFace += static_cast<int>(0.5f * diffx);
    m_win.origX = position.x();
    m_win.origY = position.y();
    update();
  }
  // right mouse translate code
  else if (m_win.translate && _event->buttons() == Qt::RightButton)
  {
    int diffX = static_cast<int>(position.x() - m_win.origXPos);
    int diffY = static_cast<int>(position.y() - m_win.origYPos);
    m_win.origXPos = position.x();
    m_win.origYPos = position.y();
    m_modelPos.m_x += INCREMENT * diffX;
    m_modelPos.m_y -= INCREMENT * diffY;
    update();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mousePressEvent(QMouseEvent *_event)
{
// that method is called when the mouse button is pressed in this case we
// store the value where the maouse was clicked (x,y) and set the Rotate flag to true
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
  auto position = _event->position();
#else
  auto position = _event->pos();
#endif
  if (_event->button() == Qt::LeftButton)
  {
    m_win.origX = position.x();
    m_win.origY = position.y();
    m_win.rotate = true;
  }
  // right mouse translate mode
  else if (_event->button() == Qt::RightButton)
  {
    m_win.origXPos = position.x();
    m_win.origYPos = position.y();
    m_win.translate = true;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent(QMouseEvent *_event)
{
  // that event is called when the mouse button is released
  // we then set Rotate to false
  if (_event->button() == Qt::LeftButton)
  {
    m_win.rotate = false;
  }
  // right mouse translate mode
  if (_event->button() == Qt::RightButton)
  {
    m_win.translate = false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::wheelEvent(QWheelEvent *_event)
{
  // check the diff of the wheel position (0 means no change)
  if (_event->angleDelta().x() > 0)
  {
    m_modelPos.m_z += ZOOM;
  }
  else if (_event->angleDelta().x() < 0)
  {
    m_modelPos.m_z -= ZOOM;
  }
  update();
}
//----------------------------------------------------------------------------------------------------------------------
void removeCoins(float x, float z)
{
    for(auto it = coinTransformations.begin(); it != coinTransformations.end(); ++it)
    {
        if(it->getPosition().m_x == x && it->getPosition().m_z == z)
        {
            it = coinTransformations.erase(it);
        }
    }
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
        loadMaze();
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
      printMazeGrid();
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
                rotateMatrixLeft();
                std::cout << "rotate matrix left" << std::endl;
            }
            else
            {
                rotateMatrixRight();
                std::cout << "rotate matrix right" << std::endl;
            }
        }
        lastRotation = (int)m_cameraYaw;
        findShortestPath();
        if (mazeGrid[cameraGridX + dx][cameraGridY + dy] != 1 && move)
        {
            if(mazeGrid[cameraGridX + 2][cameraGridY + dy] == 3)
            {
                GameOver();
            }
            else
            {
                mazeGrid[cameraGridX][cameraGridY] = 0;
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
                std::cout << "cameraxPosition" << xPosition << "cameraxPosition" << zPosition << std::endl;
                removeCoins(lastxPosition, lastzPosition);
                lastxPosition = xPosition;
                lastzPosition = zPosition;
                m_cameraPosition = ngl::Vec3(xPosition, 0.2, zPosition);

                updateCameraPosition();
                mazeGrid[cameraGridX][cameraGridY] = 2;
                std::cout << cameraGridX << " " << cameraGridY << std::endl;
                printMazeGrid();
            }
        }
      }
    break;
  case Qt::Key_Left:
      m_cameraYaw -= rotateSpeed;
      updateCameraPosition();
      if((int)m_cameraYaw % 90 == 0 && lastRotation != (int)m_cameraYaw)
      {
          rotateMatrixRight();
          std::cout << "reverse" << std::endl;
          printMazeGrid();
          lastRotation = (int)m_cameraYaw;

      }
      break;
  case Qt::Key_Right:
      m_cameraYaw += rotateSpeed;
      updateCameraPosition();
          if((int)m_cameraYaw % 90 == 0 && lastRotation != (int)m_cameraYaw)
              // if(m_cameraYaw == 0 || m_cameraYaw == 90 || m_cameraYaw == 180 || m_cameraYaw == 270 || m_cameraYaw == 360)
          {
              rotateMatrixLeft();
              std::cout << "reverse" << std::endl;
              printMazeGrid();
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

void NGLScene::printMazeGrid() {
    std::cout << "Maze Grid:" << std::endl;
    for (int a = 0; a < mazeGrid.size(); a++) {
        for (int b = 0; b < mazeGrid[a].size(); b++) {
            std::cout << mazeGrid[a][b] << " ";
        }
        std::cout << std::endl;
    }
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

void NGLScene::rotateMatrixRight()
{
    int newX;
    int newY;

    int n = mazeGrid.size();
    QVector<QVector<int>> newGrid(n, QVector<int>(n,0));

    for(int i = 0; i < n; ++i)
    {
        for(int j = 0; j < n; ++j)
        {
            newGrid[j][n - 1 - i] = mazeGrid[i][j];
        }
    }
    mazeGrid = newGrid;
    newX = cameraGridY;
    newY = 14 - cameraGridX;
    cameraGridX = newX;
    cameraGridY = newY;
    newX = selectedY;
    newY = 14 - selectedX;
    selectedX = newX;
    selectedY = newY;
    std::cout << cameraGridX << " " << cameraGridY << std::endl;
    std::cout << "selectedX: " << selectedX << " selectedY" << selectedY << std::endl;
}

void NGLScene::rotateMatrixLeft()
{
    int newX;
    int newY;

    int n = mazeGrid.size();
    QVector<QVector<int>> newGrid(n, QVector<int>(n,0));

    for(int i = 0; i < n; ++i)
    {
        for(int j = 0; j < n; ++j)
        {
            newGrid[n - 1 - j][i] = mazeGrid[i][j];
        }
    }
    mazeGrid = newGrid;
    newX = 14 - cameraGridY;
    newY = cameraGridX;
    cameraGridX = newX;
    cameraGridY = newY;
    newX = 14 - selectedY;
    newY = selectedX;
    selectedX = newX;
    selectedY = newY;
    std::cout << cameraGridX << " " << cameraGridY << std::endl;
    std::cout << "selectedX: "<<selectedX << " selectedY" << selectedY<<std::endl;
}

void NGLScene::findShortestPath()
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
 //   if((int)m_cameraYaw==90 || (int)m_cameraYaw==270)
 //   {
        if (mazeGrid[selectedX + leftX][selectedY + leftY] == 0 || mazeGrid[selectedX + leftX][selectedY + leftY] == 2) {
            if (mazeGrid[selectedX + leftX][selectedY + leftY] == 2)
            {
                GameOver();
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
                GameOver();
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
                GameOver();
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
                GameOver();
            }
            distance = sqrt(pow((selectedX + backwardX - cameraGridX), 2) + pow(((selectedY + backwardY) - cameraGridY), 2));
            if (shortest > distance) {
                shortest = distance;
                currentShortestX = selectedX + backwardX;
                currentShortestY = selectedY + backwardY;
                chosen = 4;
            }
        }
    std::cout << "sphereInitialxPosition" << sphereInitialxPosition << "sphereInitialzPosition" <<sphereInitialzPosition << std::endl;
    std::cout << "sphereLastxPosition" << sphereLastxPosition << "sphereLastzPosition" <<sphereLastzPosition << std::endl;
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
    std::cout << "xPosition: " << xPosition << " zPosition: " << zPosition << std::endl;
    std::cout << "chosen: " << chosen << std::endl;
    placeSphere(xPosition, 0.0f, zPosition);
    sphereLastxPosition = xPosition;
    sphereLastzPosition = zPosition;
    mazeGrid[selectedX][selectedY] = 0;
    selectedX = currentShortestX;
    selectedY = currentShortestY;
    mazeGrid[selectedX][selectedY] = 3;
    shortest = std::numeric_limits<float>::max();
}


