#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
#include "Mesh.h"
#include <random>
#include <ngl/NGLInit.h>
#include <ngl/ShaderLib.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/Transformation.h>
#include <ngl/NGLStream.h>


#include <iostream>


void Mesh::Draw()
{
  m_MESH->draw();
  m_MESH->setTexture(m_TEXFilename);
  texId = m_MESH->getTextureID();
  // Drawing function also sets the texture and gives ID for assignment later
}

void Mesh::CreateVAO()
{
  m_MESH.reset(new ngl::Obj(m_OBJFilename,m_TEXFilename));
  m_MESH->createVAO();
  // Creates a VAO for that specific mesh, makes multiple meshes possible
}

void Mesh::Transform(float _xDif, float _yDif, float _zDif)
{
  m_TRANS.addPosition(_xDif,_yDif,_zDif);
  // Change position for testing lighting locations
}


NGLScene::NGLScene(const std::string &_objName, const std::string &_texName)
{
  setTitle("Render");
  m_objFileName = _objName;
  m_texFileName = _texName;
  m_timer.start(); // timer to know what time it is
}


NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}



void NGLScene::resizeGL(int _w , int _h)
{
  m_cam.setProjection(45.0f, static_cast<float>(_w) / _h, 0.05f, 50.0f);
  m_win.width  = static_cast<int>( _w * devicePixelRatio() );
  m_win.height = static_cast<int>( _h * devicePixelRatio() );
}


void NGLScene::initializeGL()
{
  // initialise and clear
  ngl::NGLInit::initialize();
  glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);


  // load shaders with fragment and vertex
  ngl::ShaderLib::loadShader("GbufferShader","shaders/DSVertext.glsl","shaders/DSFragment.glsl");
  ngl::ShaderLib::loadShader("LightingShader","shaders/LightVertex.glsl","shaders/LightFragment.glsl");
  ngl::ShaderLib::loadShader("BoxLightShader", "shaders/BoxlightVertex.glsl","shaders/BoxlightFragment.glsl");

  // Camera inital setup
  ngl::Vec3 from(0, 0, 5);
  ngl::Vec3 to(0, 0, 0);
  ngl::Vec3 up(0, 1, 0);
  m_cam.set(from,to,up);
  m_cam.setProjection(45.0f,720.0f/576.0f,0.05f,100.0f);

  int w = this->size().width();
  int h = this->size().height();


  GLint currentTextureID;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTextureID);
  std::cout << "Current Texture ID: " << currentTextureID << std::endl;

  // G BUFFER CREATION

  // Following section modified from :-
  // Learnopengl.com. (2020). LearnOpenGL - Deferred Shading. [online]
  // [Accessed 2024] Available at: https://learnopengl.com/code_viewer_gh.php?code=src/5.advanced_lighting/8.1.deferred_shading/deferred_shading.cpp

  glGenFramebuffers(1,&gBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
  // position color buffer
  glGenTextures(1, &gPos);
  glBindTexture(GL_TEXTURE_2D, gPos);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F,  m_win.width , m_win.height, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPos, 0);

  glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTextureID);
  std::cout << "Positon data set to ID: " << currentTextureID << std::endl; // Tests not part of citation

  // normal color buffer
  glGenTextures(1, &gNorm);
  glBindTexture(GL_TEXTURE_2D, gNorm);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F,  m_win.width , m_win.height, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNorm, 0);

  glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTextureID);
  std::cout << "Normal data set to ID: " << currentTextureID << std::endl; // Tests not part of citation

  // color + specular color buffer
  glGenTextures(1, &gColorSpec);
  glBindTexture(GL_TEXTURE_2D, gColorSpec);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_win.width , m_win.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gColorSpec, 0);

  glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTextureID);
  std::cout << "ColourSpecular data set to ID: " << currentTextureID << std::endl; // Test not part of citation

  // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
  unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
  glDrawBuffers(3, attachments);

  unsigned int rboDepth;
  glGenRenderbuffers(1, &rboDepth);
  glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_win.width , m_win.height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cerr << "FrameBufferError" << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // end of citation
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
  {
    std::cout << "Gbuffer Complete" << std::endl;
  }

  // LIGHTS

  glGenBuffers(1,&uBuffer);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, uBuffer);

  int offset = 0;
  for (unsigned int i = 0; i < numLights; i++)
  {
    std::random_device rd;
    std::uniform_real_distribution<double> posDist(-lightDiff,lightDiff);
    std::uniform_real_distribution<double> ColDist(0.1,1.0);
    lightPos.push_back(ngl::Vec3(posDist(rd),posDist(rd),posDist(rd)));
    lightCol.push_back(ngl::Vec3(ColDist(rd),ColDist(rd),ColDist(rd)));
    // randomly sets position and colour of each light
  }

  // UNIFORM BUFFER FOR LIGHTS

  glGenBuffers(1, &uBuffer);
  glBindBuffer(GL_UNIFORM_BUFFER, uBuffer);
  glBufferData(GL_UNIFORM_BUFFER,500, nullptr, GL_STATIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  if (glCheckFramebufferStatus(GL_UNIFORM_BUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    std::cout << "Uniform Buffer Error"<< std::endl;
  }


  mesh1.CreateVAO();

  glViewport(0, 0, width(), height());

}


void NGLScene::loadMatricesToShader(std::string ProgramName, bool CalcMatrix)
{
  ngl::ShaderLib::use(ProgramName);
  GLuint programID = ngl::ShaderLib::getProgramID(ProgramName);
  glUseProgram(programID);

  ngl::Mat3 normalMatrix;

  ngl::Mat4 V;
  ngl::Mat4 P;

  V = m_cam.getView();
  P = m_cam.getProjection();

  glUniformMatrix4fv(glGetUniformLocation(programID,"View"),1,GL_FALSE,V.openGL());
  glUniformMatrix4fv(glGetUniformLocation(programID,"Projection"),1,GL_FALSE,P.openGL());

  if (CalcMatrix == true)
  {
    ngl::Mat4 M;
    M = mesh1.m_TRANS.getMatrix();
    glUniformMatrix4fv(glGetUniformLocation(programID,"Model"),1,GL_FALSE,M.openGL());
    normalMatrix = ngl::Mat3(M*V);
    normalMatrix.inverse().transpose();
    glUniformMatrix3fv(glGetUniformLocation(programID,"normalMatrix"),1,GL_FALSE,normalMatrix.openGL());
  }

}



void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_win.width,m_win.height);

  // Get current frame
  float currentFrame = m_timer.elapsed() * 0.001f;
  m_deltatime = currentFrame-m_lastframe;
  m_lastframe = currentFrame;

  // G BUFFER PASS

  glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  loadMatricesToShader("GbufferShader", true);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,mesh1.texId);
  mesh1.Draw();
  glBindFramebuffer(GL_FRAMEBUFFER,0);

  // // LIGHT PASS

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  loadMatricesToShader("LightingShader", true);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,gPos);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D,gNorm);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D,gColorSpec);

  GLuint programID = ngl::ShaderLib::getProgramID("LightingShader");
  glUniform1i(glGetUniformLocation(programID, "gPos"), 0);
  glUniform1i(glGetUniformLocation(programID, "gNorm"), 1);
  glUniform1i(glGetUniformLocation(programID, "gColorSpec"), 2);

  if (glGetUniformLocation(programID, "gPos") == -1 || glGetUniformLocation(programID, "gNorm") == -1 || glGetUniformLocation(programID, "gColorSpec") == -1)
  {
    std::cerr << "Uniform Error" << std::endl;
  }

  // if (glGetUniformLocation(programID, "gPos") != -1 && glGetUniformLocation(programID, "gNorm") != -1 && glGetUniformLocation(programID, "gColorSpec") != -1)
  // {
  //   std::cout << "Uniforms Loaded" << std::endl;
  // }

  for(unsigned int i = 0; i < numLights; i++)
  {
    glUniform3fv(glGetUniformLocation(programID,("lights[" + std::to_string(i) + "].Pos").c_str()),1,lightPos[i].openGL());
    glUniform3fv(glGetUniformLocation(programID,("lights[" + std::to_string(i) + "].Col").c_str()),1,lightCol[i].openGL());
    const float constant = 1.0f;
    const float linear = 0.7f;
    GLfloat quadratic = 1.8f;
    glUniform1f(glGetUniformLocation(programID,("lights[" + std::to_string(i) + "].Linear").c_str()),linear);
    glUniform1f(glGetUniformLocation(programID,("lights[" + std::to_string(i) + "].Quadratic").c_str()),quadratic);
    const float maxBrightness = std::fmaxf(std::fmaxf(lightCol[i].m_x, lightCol[i].m_y), lightCol[i].m_z);
    float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
    glUniform1f(glGetUniformLocation(programID,("lights[" + std::to_string(i) + "].Radius").c_str()),radius);

  }

  glUniform3fv(glGetUniformLocation(programID,"viewPos"),1,m_cam.camPos.openGL());
  glDisable(GL_DEPTH_TEST);

  renderQuad();

  glEnable(GL_DEPTH_TEST);

  glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

  glBlitFramebuffer(0, 0, m_win.width , m_win.height, 0, 0, m_win.width, m_win.height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);


  ngl::ShaderLib::use("BoxLightShader");
  programID = ngl::ShaderLib::getProgramID("BoxLightShader");

  loadMatricesToShader("BoxLightShader",false);
  for (unsigned int i = 0; i < numLights; i++)
  {
    lightTrans.setPosition(lightPos[i]);
    lightTrans.setScale(0.05,0.05,0.05);
    ngl::Mat4 model = lightTrans.getMatrix();
    glUniformMatrix4fv(glGetUniformLocation(programID,"Model"),1,GL_FALSE,model.openGL());
    glUniform3fv(glGetUniformLocation(programID,"lightCol"),1,lightCol[i].openGL());
    renderCube();
  }
}

void NGLScene::clearLights()
{
  lightPos.clear();
  lightCol.clear();
}


void NGLScene::changeLights()
{
  for (unsigned int i = 0; i < numLights; i++)
  {
    std::random_device rd;
    std::uniform_real_distribution<double> posDist(-lightDiff,lightDiff);
    std::uniform_real_distribution<double> ColDist(0.1,1.0);
    lightPos.push_back(ngl::Vec3(posDist(rd),posDist(rd),posDist(rd)));
    lightCol.push_back(ngl::Vec3(ColDist(rd),ColDist(rd),ColDist(rd)));
  }
}

// Following section modified from :-
// Learnopengl.com. (2020). LearnOpenGL - Deferred Shading. [online]
// [Accessed 2024] Available at: https://learnopengl.com/code_viewer_gh.php?code=src/5.advanced_lighting/8.1.deferred_shading/deferred_shading.cpp

unsigned int quadVAO = 0;
unsigned int quadVBO;

void NGLScene::renderQuad()
{
  if (quadVAO == 0 )
  {
    float quadVerticies[] = {
      -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
      -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
      1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
      1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(quadVerticies),&quadVerticies,GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3,GL_FLOAT, GL_FALSE, 5 * sizeof(float),(void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2,GL_FLOAT, GL_FALSE, 5 * sizeof(float),(void*)(3*sizeof(float)));
  }
  glBindVertexArray(quadVAO);
  glDrawArrays(GL_TRIANGLE_STRIP,0,4);
  glBindVertexArray(0);
}

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;

void NGLScene::renderCube()
{
  if (cubeVAO == 0)
  {
    float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left
        };
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    // fill buffer
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // link vertex attributes
    glBindVertexArray(cubeVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }
  // render Cube
  glBindVertexArray(cubeVAO);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
}

// end of citation

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  switch (_event->key())
  {
  // escape key to quite
  case Qt::Key_Escape :
    QGuiApplication::exit(EXIT_SUCCESS); break;
  case Qt::Key_A :
    m_cam.Move(0.0,-1.0,m_deltatime);
    break;
  case Qt::Key_D :
    m_cam.Move(0.0,1.0,m_deltatime);
    break;
  case Qt::Key_W :
    m_cam.Move(1.0,0.0,m_deltatime);
    break;
  case Qt::Key_S :
    m_cam.Move(-1.0,0.0,m_deltatime);
    break;
  case Qt::Key_Up :
    mesh1.Transform(0.0,0.5,0.0);
    break;
  case Qt::Key_Down :
    mesh1.Transform(0.0,-0.5,0.0);
    break;
  case Qt::Key_Left :
    mesh1.Transform(-0.5,0.0,0.0);
    break;
  case Qt::Key_Right :
    mesh1.Transform(0.5,0.0,0.0);
    break;
  case Qt::Key_L :
    clearLights();
    changeLights();
    break;
  case Qt::Key_O :
    lightDiff += 0.1;
    for (unsigned int i = 0; i < numLights; i++)
    {
      lightPos[i].operator*=(1.1);

    }
    break;
  case Qt::Key_I :
    lightDiff -= 0.1;
    for (unsigned int i = 0; i < numLights; i++)
    {
      lightPos[i].operator*=(0.9);
    }
    break;
  case Qt::Key_M :
    numLights += 2;
    changeLights();
    break;
  case Qt::Key_N :
    if (numLights > 2)
    {
      numLights -= 2;
      changeLights();
    }


  break;


  }
  // finally update the GLWindow and re-draw

    update();
}
