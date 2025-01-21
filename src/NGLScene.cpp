#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
#include "Mesh.h"
#include <ngl/NGLInit.h>
#include <ngl/ShaderLib.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/Transformation.h>
#include <ngl/NGLStream.h>


#include <iostream>


void Mesh::Draw()
{
  m_MESH->draw();
}

void Mesh::CreateVAO()
{
  m_MESH.reset(new ngl::Obj(m_OBJFilename,m_TEXFilename));
  m_MESH->createVAO();
}

void Mesh::Transform(float _xDif, float _yDif, float _zDif)
{
  m_TRANS.addPosition(_xDif,_yDif,_zDif);
}

Mesh mesh1;


NGLScene::NGLScene(const std::string &_objName, const std::string &_texName)
{
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  setTitle("Render");
  m_objFileName = _objName;
  m_texFileName = _texName;
  // m_MeshArray.resize(m_meshNum);
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

unsigned int gBuffer;
unsigned int gPos, gNorm, gColorSpec;
unsigned int gBuffProgram;
unsigned int lightingProgram;



void NGLScene::initializeGL()
{
  // we must call that first before any other GL commands to load and link the
  // gl commands from the lib, if that is not done program will crash
  ngl::NGLInit::initialize();
  glClearColor(0.7f, 0.7f, 0.7f, 1.0f);			   // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);



  ngl::ShaderLib::loadShader("ParticleShader","shaders/DSVertext.glsl","shaders/DSFragment.glsl");
  ngl::ShaderLib::loadShader("GBufferShader","shaders/DSVertext.glsl","shaders/LightFragment.glsl");

  // ngl::ShaderLib::createShaderProgram("GbuffProgram",ngl::ErrorExit::ON);
  // ngl::ShaderLib::attachShaderToProgram("GbuffProgram","shaders/DSVertext.glsl");
  // ngl::ShaderLib::attachShaderToProgram("GbuffProgram","/shaders/DSFragment.glsl");
  // ngl::ShaderLib::createShaderProgram("LightProgram");

  ngl::Vec3 from(0, 1, 5);
  ngl::Vec3 to(0, 0, 0);
  ngl::Vec3 up(0, 1, 0);
  m_cam.set(from,to,up);
  m_cam.setProjection(45.0f,720.0f/576.0f,0.05f,100.0f);

  int w = this->size().width();
  int h = this->size().height();

  glGenFramebuffers(1,&gBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

  glGenTextures(1, &gPos);
  glBindTexture(GL_TEXTURE_2D, gPos);
  glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA16F, m_win.width,m_win.height,0,GL_RGBA,GL_FLOAT,NULL);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,gPos,0);

  glGenTextures(1, &gNorm);
  glBindTexture(GL_TEXTURE_2D, gNorm);
  glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA16F, m_win.width,m_win.height,0,GL_RGBA,GL_FLOAT,NULL);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,gNorm,0);

  glGenTextures(1, &gColorSpec);
  glBindTexture(GL_TEXTURE_2D, gColorSpec);
  glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA16F, m_win.width,m_win.height,0,GL_RGBA,GL_FLOAT,NULL);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D,gColorSpec,0);

  unsigned int attachments[3] = {GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2};
  glDrawBuffers(3, attachments);

  mesh1.CreateVAO();
  // mesh1.Transform(1.0f,1.0f,1.0f);

  glViewport(0, 0, width(), height());

}


void NGLScene::loadMatricesToShader(std::string ProgramName)
{
  ngl::Mat3 normalMatrix;
  ngl::Mat4 M;
  ngl::Mat4 V;
  ngl::Mat4 P;

  M = mesh1.m_TRANS.getMatrix();
  V = m_cam.getView();
  P = m_cam.getProjection();

  normalMatrix = ngl::Mat3(M*V);
  normalMatrix.inverse().transpose();

  ngl::ShaderLib::use(ProgramName);
  GLuint programID = ngl::ShaderLib::getProgramID(ProgramName);

  glUniformMatrix4fv(glGetUniformLocation(programID,"Model"),1,GL_FALSE,M.openGL());
  glUniformMatrix4fv(glGetUniformLocation(programID,"View"),1,GL_FALSE,V.openGL());
  glUniformMatrix4fv(glGetUniformLocation(programID,"Projection"),1,GL_FALSE,P.openGL());
  glUniformMatrix3fv(glGetUniformLocation(programID,"normalMatrix"),1,GL_FALSE,normalMatrix.openGL());
}



void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_win.width,m_win.height);


  float currentFrame = m_timer.elapsed() * 0.001f;
  m_deltatime = currentFrame-m_lastframe;
  m_lastframe = currentFrame;

  ngl::Mat4 rotX = ngl::Mat4::rotateX(m_win.spinXFace);
  ngl::Mat4 rotY = ngl::Mat4::rotateY(m_win.spinYFace);


  m_mouseGlobalTX = rotY * rotX;
  m_mouseGlobalTX.m_m[3][0] = m_modelPos.m_x;
  m_mouseGlobalTX.m_m[3][1] = m_modelPos.m_y;
  m_mouseGlobalTX.m_m[3][2] = m_modelPos.m_z;


  loadMatricesToShader(useProgram);
  mesh1.Draw();

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);



}
//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  const float inc = 0.5f;
  float xDirection = 0.0f;
  float yDirection = 0.0f;
  switch (_event->key())
  {
  // escape key to quite
  case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
  case Qt::Key_Space :
      m_win.spinXFace=0;
      m_win.spinYFace=0;
      m_modelPos.set(ngl::Vec3::zero());
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
  case Qt::Key_N :
    useProgram = "GBufferShader";
    break;
  case Qt::Key_M :
    useProgram = "ParticleShader";
    break;

  break;


  }
  // finally update the GLWindow and re-draw

    update();
}
