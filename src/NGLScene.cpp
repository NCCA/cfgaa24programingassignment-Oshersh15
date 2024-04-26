#include "includes.h"

NGLScene::NGLScene()
{
  setTitle("VAOPrimitives Demo");
  //m_animate = true;
}

NGLScene::~NGLScene()
{
  std::cout << "Shutting down NGL, removing VAO's and Shaders\n";
}

bool hasRun = false;
QVector<QVector<int>> NGLScene::loadMaze()
{
    if (!hasRun) {
        QImage image("/home/s5301744/repos/cfgaa24programingassignment-Oshersh15/image/Maze.png");
        if (image.isNull()) {
            qDebug() << "Failed to load maze image.";
            return QVector<QVector<int>>(0);
        }
        image = image.convertToFormat(QImage::Format_RGB32);
        int mazeSize = 15;
        QVector<QVector<int>> mazeGrid(mazeSize, QVector<int>(mazeSize, 0));
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
//                        int pixelX = gridX * cellWidth + x;
//                        int pixelY = (14 - gridY) * cellHeight + y;
//                        QRgb pixel = image.pixel(pixelX, pixelY); // Flip vertically
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
            qDebug() << row;  // Output each grid row of the maze
            qDebug() << mazeGrid;
            qDebug() << midIndex;
        }
        hasRun = true;
        return mazeGrid;
    }
    return QVector<QVector<int>>(0);
}

void NGLScene::updateCameraPosition()
{
    float radYaw = ngl::radians(m_cameraYaw);
    m_cameraForward = ngl::Vec3(cos(radYaw), 0, sin(radYaw));
    m_cameraRight = m_cameraForward.cross(ngl::Vec3(0,1,0));
    //ngl::Vec3 forward(cos(radYaw), 0, sin(radYaw));
    //ngl::Vec3 right = forward.cross(ngl::Vec3(0,1,0));

    m_view = ngl::lookAt(m_cameraPosition, m_cameraPosition + m_cameraForward, ngl::Vec3(0,1,0));
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
  // We now create our view matrix for a static camera
  ngl::Vec3 from(00.0f, 0.0f, 0.0f);
  ngl::Vec3 to(0.0f, 0.0f, 0.0f);
  ngl::Vec3 up(0.0f, 2.0f, 0.0f);
  // now load to our new camera
  m_view = ngl::lookAt(from, to, up);
  m_project = ngl::perspective(45.0f, 1024.0f/720.0f, 0.05f, 350.0f);
//  m_camera.setView(ngl::Vec3(0.0f,2.0f,10.0f),
//                   ngl::Vec3(0.0f,0.0f,0.0f),
//                    ngl::Vec3(0.0f,1.0f,0.0f));
//  m_camera.setPerspective(45.0f, 1024.0f / 720.0f,0.05f,350.0f);
  ngl::ShaderLib::setUniform("camPos", from);
  // now a light
  m_lightPos.set(0.0, 2.0f, 2.0f, 1.0f);
  // setup the default shader material and light porerties
  // these are "uniform" so will retain their values
  ngl::ShaderLib::setUniform("lightPosition", m_lightPos.toVec3());
  ngl::ShaderLib::setUniform("lightColor", 400.0f, 400.0f, 400.0f);
  ngl::ShaderLib::setUniform("exposure", 2.2f);
  ngl::ShaderLib::setUniform("albedo", 0.950f, 0.71f, 0.29f);

  ngl::ShaderLib::setUniform("metallic", 0.0f);
  ngl::ShaderLib::setUniform("roughness", 0.5f);
  ngl::ShaderLib::setUniform("ao", 0.2f);

/*  ngl::VAOPrimitives::createSphere("sphere", 0.5f, 50);

  ngl::VAOPrimitives::createCylinder("cylinder", 0.5f, 1.4f, 40, 40);

  ngl::VAOPrimitives::createCone("cone", 0.5, 1.4f, 20, 20);

  ngl::VAOPrimitives::createDisk("disk", 0.8f, 120);
  ngl::VAOPrimitives::createTorus("torus", 0.15f, 0.4f, 40, 40);*/
  ngl::VAOPrimitives::createTrianglePlane("plane", 7.5, 7.5, 80, 80, ngl::Vec3(0, 1, 0));
  // this timer is going to trigger an event every 40ms which will be processed in the
  //
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

//  ngl::ShaderLib::use("PBR");
//  ngl::Mat4 MVP = m_camera.getProjectionMatrix() * m_camera.getViewMatrix() * objectMatrix;
//  ngl::ShaderLib::setUniform("MVP", MVP);
}

std::vector<ngl::Transformation> cubeTransformations;
void NGLScene::processArray() {
    auto mazeMatrix = NGLScene::loadMaze();
    float baseX = 0.0f;
    float baseZ = 3.5f;  // Starting Z position for the first row
    float xSpacing = 0.5f;  // Horizontal spacing between cubes
    float zSpacing = -0.5f;
//    int m_cameraGridX;
//    int m_cameraGridY;
//    int centre = mazeMatrix.size() / 2 + 1;
//    ngl::Vec3 startingPosition;
//    bool foundStartingPosition = false;
    //qDebug() << mazeMatrix;
    for (int a = 0; a < mazeMatrix.size(); a++)
    {
        for (int b = 0; b < mazeMatrix.size(); b++)
        {
            ngl::Transformation transform;
            float xPosition = baseX + (float)b * xSpacing;
            float zPosition = baseZ + (float)a * zSpacing;
            transform.setPosition(xPosition, 0.0f, zPosition);
            transform.setScale(0.5f, 0.5f, 0.5f);
            if (mazeMatrix[a][b] == 1)
            {
                std::cout << "[" << a << "][" << b << "] is wall" << std::endl;
                //std::cout << "wall" << std::endl;
                cubeTransformations.push_back(transform);
            }// and before a pop
            else if (mazeMatrix[a][b] == 2)
            {
                std::cout << "Starting position at" << a << " , " << b << std::endl;
                m_cameraPosition = ngl::Vec3(xPosition, 0.5, zPosition); //y is currently changed for testing convenience - supposed to be 0.0f
                updateCameraPosition();

                ngl::Transformation transform;
                transform.setPosition(xPosition, 0.0f, zPosition);
                transform.setScale(0.2f, 0.2f, 0.2f);
                cubeTransformations.push_back(transform);
            }
            else
            {
                    //std::cout << "null" << std::endl;
                    std::cout << "[" << a << "][" << b << "] is path" << std::endl;
            }
        }
    }
//    if (foundStartingPosition)
//    {
//        updateCameraPosition();
//    }
}

void NGLScene::renderMaze() {
    for (const auto& transform : cubeTransformations) {
        m_transform = transform;
        loadMatricesToShader();
        ngl::VAOPrimitives::draw("cube");
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

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
    //qDebug() << "Key pressed:" << _event->key();
   float moveSpeed = 0.1f;
   float rotateSpeed = 5.0f;
   int dx = 0;
   int dy = 0;
   int cameraGridX = 7;
   int cameraGridY = 6;
//   int gridX = m_cameraPosition.m_x / 0.5f + 7.5f;
//   int gridZ = m_cameraPosition.m_z / 0.5f + 7.5f;
   auto mazeMatrix = NGLScene::loadMaze();

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
//      if(mazeGrid[gridZ+1][gridX] == 1)
//      {
//          m_cameraPosition += m_cameraForward * moveSpeed;
//          gridZ--;
//      }
      m_cameraPosition += m_cameraForward * moveSpeed;
      dy = - 1;
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    break;
  // turn off wire frame
  case Qt::Key_Down:
      m_cameraPosition -= m_cameraForward * moveSpeed;
      dy = 1;
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    break;
  case Qt::Key_Left:
      m_cameraPosition -= m_cameraRight * moveSpeed;
      dx = - 1;
      break;
  case Qt::Key_Right:
      m_cameraPosition += m_cameraRight * moveSpeed;
      dx = 1;
      break;
  case Qt::Key_A:
      m_cameraYaw -= rotateSpeed;
      break;
  case Qt::Key_S:
      m_cameraYaw += rotateSpeed;
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
  // show windowed
//  case Qt::Key_N:
//    showNormal();
//    break;
  case Qt::Key_Space:
    m_animate ^= true;
    break;
  default:
    break;
  }

//  int newGridX = cameraGridX + dx;
//  int newGridY = cameraGridY + dy;
//
//  if(newGridX >= 0 && newGridX < mazeMatrix[0].size() && newGridY >= 0 && newGridY < mazeMatrix.size() && mazeMatrix[newGridY][newGridX] == 0)
//  {
//      mazeMatrix[cameraGridY][cameraGridX] = 0;
//      mazeMatrix[newGridY][newGridX] = 2;
//      cameraGridX = newGridX;
//      cameraGridY = newGridY;
//      std::cout << "location is [" << cameraGridY << "][" << cameraGridX << "]" << std::endl;
//
//  }

//  int newX = m_cameraGridX + dx;
//  int newY = m_cameraGridY + dy;


//  if(newX >= 0 && newX <mazeGrid.size() && newY >= 0 && newY < mazeGrid.size() && mazeGrid[newY][newX] == 0)
//  {
//      mazeGrid[m_cameraGridY][m_cameraGridX] = 0;
//
//      m_cameraGridX = newX;
//      m_cameraGridY = newY;
//      mazeGrid[m_cameraGridY][m_cameraGridX] = 2;
//  }
  // finally update the GLWindow and re-draw
//  if (isExposed())
//  {
//      update();
//  }
 // std::cout << "current position:" << gridZ << " , " << gridX << std::endl;
  update();
  updateCameraPosition();

}

bool NGLScene::canMove(const ngl::Vec3 &potentialPosition)
{
    int gridX = static_cast<int>(floor((potentialPosition.m_x / 0.5f) + 7.5f));
    int gridZ = static_cast<int>(floor((potentialPosition.m_z / 0.5f) + 7.5f));

    if(gridX < 0 || gridX >= mazeGrid.size() || gridZ < 0 || gridZ >= mazeGrid[0].size())
        return false;

    return mazeGrid[gridZ][gridX] == 0;
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

