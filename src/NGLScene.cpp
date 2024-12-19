#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
#include <ngl/NGLInit.h>
#include <ngl/ShaderLib.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/Transformation.h>
#include <ngl/NGLStream.h>

#include <iostream>


NGLScene::NGLScene()
{
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  setTitle("Render");
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
  glViewport(0, 0, width(), height());
  
}

float vertices[] = {
  // Position           // Colour
  1.0f, 1.0f, 1.0f,     1.0f, 0.0f, 0.0f, // 0 // Top Left Front
  1.0f, -1.0f, 1.0f,    0.0f, 1.0f, 0.0f, // 1 // Bottom Left Front

  -1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f, // 2 // Top Right Front
  -1.0f, -1.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 3 // Bottom Right Front

  1.0f, 1.0f, -1.0f,    1.0f, 0.0f, 0.0f, // 4 // Top Left Back
  1.0f, -1.0f, -1.0f,   0.0f, 1.0f, 0.0f, // 5 // Bottom Left Back

  -1.0f, 1.0f, -1.0f,   0.0f, 1.0f, 0.0f, // 6 // Top Right Back
  -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, 1.0f, // 7 // Bottom Right Back

  //I commented these for my own understanding not yours
};

unsigned int indices[] = {
  1, 0, 2, // Tri 1 // Front
  2, 3, 1, // Tri 2 // Face

  0, 2, 6, // Tri 3 // Top
  6, 4, 0, // Tri 4 // Face

  4, 6, 7, // Tri 5 // Back
  7, 5, 4, // Tri 6 // Face

  7, 5, 1, // Tri 7 // Bottom
  1, 3, 7, // Tri 8 // Face

  1, 5, 4, // Tri 9 // Left
  4, 0, 1, // Tri 10 // Face

  3, 7, 6, // Tri 11 // Right
  6, 2, 3, // Tri 12 // Face
};
unsigned int VAO, VBO;

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

    // ARRAYS AND BUFFERS


  glGenVertexArrays(1,&VAO);
  glBindVertexArray(VAO);
   glGenBuffers(1, &VBO);
   glBindBuffer(GL_ARRAY_BUFFER, VBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  unsigned int EBO;
  glGenBuffers(1,&EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);




  glBindVertexArray(VAO);

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

  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3* sizeof(float)));
  glEnableVertexAttribArray(1);
  // glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(6* sizeof(float)));
  // glEnableVertexAttribArray(2);



  // DRAW

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  // int vertcount = sizeof(vertices) / (sizeof(vertices[0])*3);
  // glDrawArrays(GL_TRIANGLES,0, vertcount);

  glDrawElements(GL_TRIANGLES, 36,GL_UNSIGNED_INT,0);

  // it also works with VAO primitives which I honestly didn't expect, try it out!!

  // ngl::VAOPrimitives::draw("teapot");
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
