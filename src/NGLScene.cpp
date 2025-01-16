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
  // m_POS.m_x += _xDif;
  // m_POS.m_y += _yDif;
  // m_POS.m_z += _zDif;
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

  ngl::Vec3 from(0, 1, 5);
  ngl::Vec3 to(0, 0, 0);
  ngl::Vec3 up(0, 1, 0);
  m_cam.set(from,to,up);
  m_cam.setProjection(45.0f,720.0f/576.0f,0.05f,100.0f);

  int w = this->size().width();
  int h = this->size().height();


  mesh1.CreateVAO();

  glViewport(0, 0, width(), height());

}


void NGLScene::loadMatricesToShader()
{
  ngl::Mat4 MV;
  ngl::Mat4 MVP;
  // ngl::Mat3 normalMatrix;
  ngl::Mat4 M;
  M = mesh1.m_TRANS.getMatrix();
  MV = m_cam.getView() * M;
  MVP = m_cam.getProjection() * MV;
  // normalMatrix = MV;
  // normalMatrix.inverse().transpose();
  ngl::ShaderLib::setUniform("MVP", MVP);
  // ngl::ShaderLib::setUniform("normalMatrix", normalMatrix);

  // // saving the normals for later
}

void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_win.width,m_win.height);
  // TRANSFORMS



  float currentFrame = m_timer.elapsed() * 0.001f;
  m_deltatime = currentFrame-m_lastframe;
  m_lastframe = currentFrame;

  ngl::Mat4 rotX = ngl::Mat4::rotateX(m_win.spinXFace);
  ngl::Mat4 rotY = ngl::Mat4::rotateY(m_win.spinYFace);


  m_mouseGlobalTX = rotY * rotX;
  m_mouseGlobalTX.m_m[3][0] = m_modelPos.m_x;
  m_mouseGlobalTX.m_m[3][1] = m_modelPos.m_y;
  m_mouseGlobalTX.m_m[3][2] = m_modelPos.m_z;



  // SHADERS

  ngl::ShaderLib::use("ParticleShader");

  // ngl::Mat4 MV;
  // ngl::Mat4 MVP;
  // // ngl::Mat3 normalMatrix;
  // ngl::Mat4 M;
  // int i = 0;
  // for (auto &m : m_MeshArray)
  // {
  //   m.CreateVAO();
  //
  //   // SILLY
  //   m.Transform(0.0f,0.0f,float(i));
  //   // STUPID this changes everytime opengl update this is a silly way to do this
  //
  //   M = m.m_TRANS.getMatrix();
  //   MV = m_cam.getView() * M;
  //   MVP = m_cam.getProjection() * MV;
  //   // normalMatrix = MV;
  //   // normalMatrix.inverse().transpose();
  //   ngl::ShaderLib::setUniform("MVP", MVP);
  //   m.Draw();
  //   ++i;
  //
  // }



  //mesh1.Transform(0.0f,0.0f,0.0f);
  loadMatricesToShader();
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


  break;


  }
  // finally update the GLWindow and re-draw

    update();
}
