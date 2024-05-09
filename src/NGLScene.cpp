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
void NGLScene::loadMaze()
{
    if (!hasRun) {
        QImage image("image/Maze.png");
        if (image.isNull()) {
            qDebug() << "Failed to load maze image.";
            mazeGrid = QVector<QVector<int>>();
         //   return QVector<QVector<int>>(0);
            return;
        }
        image = image.convertToFormat(QImage::Format_RGB32);
        int mazeSize = 15;
        //QVector<QVector<int>> mazeGrid(mazeSize, QVector<int>(mazeSize, 0));
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
        hasRun = true;
        findPathCorners();
    }
}

std::vector<ngl::Transformation> sphereTransformations;
std::vector<ngl::Transformation> coinTransformations;
void NGLScene::findPathCorners()
{
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
//        int index = rand() % corners.size();
        selectedX = corners[index][0];
        selectedY = corners[index][1];
        mazeGrid[selectedX][selectedY] = 3;
        std::cout << "selectedX: "<<selectedX << " selectedY" << selectedY<<std::endl;
        sphereLastxPosition = baseX + selectedX * xSpacing;
        sphereLastzPosition = baseZ + selectedY * zSpacing;
        std::cout << "sphereLastxPosition: "<<sphereLastxPosition << " sphereLastzPosition" << sphereLastzPosition<<std::endl;

        //placeSphere(xPosition, 0.0f, zPosition);
        ngl::Transformation transform;
        transform.setPosition(sphereLastxPosition, 0.0f, sphereLastzPosition);
        transform.setScale(0.3f,0.3f,0.3f);
        sphereTransformations.push_back(transform);
        printMazeGrid();
    }
}

//std::vector<ngl::Transformation> sphereTransformations;
//void NGLScene::placeSphere(float x, float y, float z)
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
 //   m_cameraForward = ngl::Vec3(1, 0, 0); //forward
//    m_cameraForward = ngl::Vec3(-1, 0, 0); //backwards
//    m_cameraForward = ngl::Vec3(0, 0, 1); //right
    //m_cameraForward = ngl::Vec3(0, 0, -1); //left
 //   m_cameraRight = m_cameraForward.cross(ngl::Vec3(0,1,0));

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
//  m_camera.setView(ngl::Vec3(0.0f,2.0f,10.0f),
//                   ngl::Vec3(0.0f,0.0f,0.0f),
//                    ngl::Vec3(0.0f,1.0f,0.0f));
  //m_camera.setPerspective(45.0f, 1024.0f / 720.0f,0.05f,350.0f);
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
//
//  ngl::VAOPrimitives::createCone("cone", 0.5, 1.4f, 20, 20);
//
 // ngl::VAOPrimitives::createDisk("disk", 0.2f, 120);
//  ngl::VAOPrimitives::createTorus("torus", 0.15f, 0.4f, 40, 40);
  ngl::VAOPrimitives::createTrianglePlane("plane", 7.5, 7.5, 80, 80, ngl::Vec3(0, 1, 0)); //need to find out how to move the plane to a different place
  //ngl::VAOPrimitives::createTrianglePlane("plane", 15.0, 15.0, 80, 80, ngl::Vec3(0, 1, 0)); // for now making it bigger than necessary
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

}

int cameraGridX;
int cameraGridY;
std::vector<ngl::Transformation> cubeTransformations;

void NGLScene::processArray() {
    //auto mazeMatrix = NGLScene::loadMaze();
    if(!hasRun)
    {
        loadMaze();
    }
    float BaseX = 0.0f;
    float BaseZ = 3.5f;  // Starting Z position for the first row
//    float xSpacing = 0.5f;  // Horizontal spacing between cubes
//    float zSpacing = -0.5f;
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
            //    std::cout << "Starting position at" << a << " , " << b << std::endl;
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
            coinTransformations.erase(it);
        }
    }
}

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
    float moveSpeed = 0.1f;
    float rotateSpeed = 5.0f;
    int dx = 0;
    int dy = 0;
    int newX;
    int newY;
    static bool move = false;
    static bool flip = true;
    float xPosition;
    float zPosition;
    //float cameraRef = m_cameraYaw;

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
      findShortestPath();
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
      std::cout << "newm_cameraYaw" << m_cameraYaw << std::endl;

      if((int)m_cameraYaw % 90 == 0)
      {
        move = true;
        dx = 1;
        //std::cout << "cameraRef " << cameraRef << std::endl;
        if (mazeGrid[cameraGridX + dx][cameraGridY + dy] != 1 && move)
        {
            mazeGrid[cameraGridX][cameraGridY] = 0;
            cameraGridX += dx;
            switch((int)m_cameraYaw)
            {
                case(90):
                    xPosition = lastxPosition;
                    zPosition = lastzPosition + xSpacing;
                    std::cout << "xPosition: " << xPosition << " zPosition: " << zPosition << std::endl;
                    break;
                case(180):
                    xPosition = lastxPosition + zSpacing;
                    zPosition = lastzPosition;
                    std::cout << "xPosition: " << xPosition << " zPosition: " << zPosition << std::endl;
                    break;
                case(270):
                    xPosition = lastxPosition;
                    zPosition = lastzPosition + zSpacing;
                    std::cout << "xPosition: " << xPosition << " zPosition: " << zPosition << std::endl;
                    break;
                default:
                    xPosition = baseX + (float) cameraGridX * xSpacing;
                    zPosition = baseZ + (float) cameraGridY * zSpacing;
                    std::cout << "xPosition: " << xPosition << " zPosition: " << zPosition << std::endl;
                    break;
            }
            removeCoins(xPosition, zPosition);
            lastxPosition = xPosition;
            lastzPosition = zPosition;
            std::cout << "lastxPosition: " << lastxPosition << " lastzPosition: " << lastzPosition << std::endl;
            m_cameraPosition = ngl::Vec3(xPosition, 0.2, zPosition);

            updateCameraPosition();
            mazeGrid[cameraGridX][cameraGridY] = 2;
            std::cout << cameraGridX << " " << cameraGridY << std::endl;
            printMazeGrid();
        }
      }
    break;
  // turn off wire frame
  case Qt::Key_Down:
      if((int)m_cameraYaw % 90 == 0) {
          move = true;
          dx = -1;
      }
      //m_cameraForward = ngl::Vec3(-1, 0, 0); //backwards
      //dx = -1;
    break;
  case Qt::Key_Left:
      m_cameraYaw -= rotateSpeed;
      //m_cameraYaw -= rotateSpeed * moveSpeed; -> trying to stop rotation every 90 degrees
      updateCameraPosition();
      if((int)m_cameraYaw % 90 == 0)
      {
          rotateMatrixRight();
          std::cout << "reverse" << std::endl;
          printMazeGrid();
          newX = cameraGridY;
          newY = 14 - cameraGridX;
          cameraGridX = newX;
          cameraGridY = newY;
          newX = selectedY;
          newY = 14 - selectedX;
          selectedX = newX;
          selectedY = newY;
          std::cout << cameraGridX << " " << cameraGridY << std::endl;
          std::cout << "selectedX: "<<selectedX << " selectedY" << selectedY<<std::endl;
      }
      //dy = 1;
      break;
  case Qt::Key_Right:
      m_cameraYaw += rotateSpeed;
      updateCameraPosition();
          if((int)m_cameraYaw % 90 == 0)
              // if(m_cameraYaw == 0 || m_cameraYaw == 90 || m_cameraYaw == 180 || m_cameraYaw == 270 || m_cameraYaw == 360)
          {
              rotateMatrixLeft();
              std::cout << "reverse" << std::endl;
              printMazeGrid();
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
      //dy = -1;
      break;
  case Qt::Key_A: //rotate left
      m_cameraYaw -= rotateSpeed;
      updateCameraPosition();
      break;
  case Qt::Key_S: //rotate right
      m_cameraYaw += rotateSpeed;
      updateCameraPosition();
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
  case Qt::Key_Space:
    m_animate ^= true;
    break;
  default:
    break;
  }
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
}

void NGLScene::rotateMatrixLeft()
{
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
}

void NGLScene::flipMatrix()
{
    int n = mazeGrid.size();
    for(int i = 0; i < n / 2; ++i)
    {
        for(int j = 0; j < n; ++j)
        {
            std::swap(mazeGrid[i][j], mazeGrid[n - 1 - i][j]);
        }
    }
}

void NGLScene::findShortestPath()
{
    float distance;

    if(mazeGrid[selectedX+1][selectedY] == 0)
    {
        distance = sqrt(pow(((selectedX+1) - cameraGridX), 2) + pow((selectedY - cameraGridY), 2));
        if(shortest>distance)
        {
            shortest = distance;
            currentShortestX = selectedX + 1;
            currentShortestY = selectedY;
        }
    }
    if(mazeGrid[selectedX-1][selectedY] == 0)
    {
        distance = sqrt(pow(((selectedX-1) - cameraGridX), 2) + pow((selectedY - cameraGridY), 2));
        if(shortest>distance)
        {
            shortest = distance;
            currentShortestX = selectedX - 1;
            currentShortestY = selectedY;
        }
    }
    if(mazeGrid[selectedX][selectedY+1] == 0)
    {
        distance = sqrt(pow((selectedX - cameraGridX), 2) + pow(((selectedY+1) - cameraGridY), 2));
        if(shortest>distance)
        {
            shortest = distance;
            currentShortestX = selectedX;
            currentShortestY = selectedY + 1;
        }
    }
    if(mazeGrid[selectedX][selectedY-1] == 0)
    {
        distance = sqrt(pow((selectedX - cameraGridX), 2) + pow(((selectedY-1) - cameraGridY), 2));
        if(shortest>distance)
        {
            shortest = distance;
            currentShortestX = selectedX;
            currentShortestY = selectedY - 1;
        }
    }
    std::cout << "currentShortestX" << currentShortestX << "currentShortestY" <<currentShortestY << std::endl;
    float xPosition;
    float zPosition;
    switch((int)m_cameraYaw)
    {
        case(90):
            xPosition = sphereLastxPosition;
            zPosition = sphereLastzPosition + xSpacing;
            std::cout << "xPosition: " << xPosition << " zPosition: " << zPosition << std::endl;
            break;
        case(180):
            xPosition = sphereLastxPosition + zSpacing;
            zPosition = sphereLastzPosition;
            std::cout << "xPosition: " << xPosition << " zPosition: " << zPosition << std::endl;
            break;
        case(270):
            xPosition = sphereLastxPosition;
            zPosition = sphereLastzPosition + zSpacing;
            std::cout << "xPosition: " << xPosition << " zPosition: " << zPosition << std::endl;
            break;
        default:
            xPosition = baseX + currentShortestX * xSpacing;
            zPosition = baseZ + currentShortestY * zSpacing;
            std::cout << "xPosition: " << xPosition << " zPosition: " << zPosition << std::endl;
            break;
    }
    sphereLastxPosition = xPosition;
    sphereLastzPosition = zPosition;
    placeSphere(xPosition, 0.0f, zPosition);
    mazeGrid[selectedX][selectedY] = 0;
    selectedX = currentShortestX;
    selectedY = currentShortestY;
    mazeGrid[selectedX][selectedY] = 3;
    shortest = std::numeric_limits<float>::max();
}


