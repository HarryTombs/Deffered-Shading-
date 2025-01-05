#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
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

Mesh mesh1;


NGLScene::NGLScene(const std::string &_objName, const std::string &_texName)
{
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  setTitle("Render");
  m_objFileName = _objName;
  m_texFileName = _texName;

}


NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}



void NGLScene::resizeGL(int _w , int _h)
{
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

  ngl::Vec3 from(0, 2, 5);
  ngl::Vec3 to(0, 0, 0);
  ngl::Vec3 up(0, 1, 0);
  m_view = lookAt(from,to,up);

  int w = this->size().width();
  int h = this->size().height();

  m_projection = ngl::perspective(45, static_cast<float>(w) / h, 0.05f, 350.0f);

  mesh1.CreateVAO();

  glViewport(0, 0, width(), height());
  
}


void NGLScene::loadMatricesToShader()
{
  ngl::Mat4 MV;
  ngl::Mat4 MVP;
  // ngl::Mat3 normalMatrix;
  ngl::Mat4 M;
  M = m_mouseGlobalTX;
  MV = m_view * M;
  MVP = m_projection * MV;
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

  ngl::Mat4 rotX = ngl::Mat4::rotateX(m_win.spinXFace);
  ngl::Mat4 rotY = ngl::Mat4::rotateY(m_win.spinYFace);


  m_mouseGlobalTX = rotY * rotX;
  m_mouseGlobalTX.m_m[3][0] = m_modelPos.m_x;
  m_mouseGlobalTX.m_m[3][1] = m_modelPos.m_y;
  m_mouseGlobalTX.m_m[3][2] = m_modelPos.m_z;



  // SHADERS

  ngl::ShaderLib::use("ParticleShader");

  loadMatricesToShader();


  // DRAW

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  mesh1.Draw();
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  switch (_event->key())
  {
  // escape key to quite
  case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
  case Qt::Key_Space :
      m_win.spinXFace=0;
      m_win.spinYFace=0;
      m_modelPos.set(ngl::Vec3::zero());

  break;
  default : break;
  }
  // finally update the GLWindow and re-draw

    update();
}
