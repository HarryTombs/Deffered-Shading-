#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
#include <ngl/NGLInit.h>
#include <ngl/ShaderLib.h>
#include <ngl/VAOPrimitives.h>
#include <iostream>
#include <oneapi/tbb/info.h>
#include <OpenImageIO/imageio.h>
using namespace OIIO;

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


}


void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_win.width,m_win.height);


   float vertices[] = {
     // Triangle 1
     0.0f, 0.5f, 0.0f,    1.0f, 0.0f, 0.0f,  // 0.0f, 0.0f,
     -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // 1.0f, 0.0f,
     0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,  // 0.5f, 1.0f

   };


  //  **ARRAYS AND BUFFERS**


  unsigned int VAO, VBO;
  glGenVertexArrays(1,&VAO);
  glBindVertexArray(VAO);
   glGenBuffers(1, &VBO);
   glBindBuffer(GL_ARRAY_BUFFER, VBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // unsigned int EBO;
  // glGenBuffers(1,&EBO);
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


  // TEXTURE

  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
  //
  // ImageInput::unique_ptr inp = ImageInput::open("wall.jpg");
  // if(!inp)
  // {
  //   std::cout << "FAILED TO LOAD TEXTURE\n";
  // }
  // else
  // {
  //   const ImageSpec &spec = inp->spec();
  //   int width = spec.width;
  //   int height = spec.height;
  //   int nrChannels = spec.nchannels;
  //   auto pixels = std::unique_ptr<unsigned char[]>(new unsigned char[width * height * nrChannels]);
  //   inp->read_image(0, 0, 0, nrChannels, TypeDesc::UINT8, &pixels[0]);
  //   inp->close();
  //   glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB, GL_UNSIGNED_BYTE, inp);
  //   glGenerateMipmap(GL_TEXTURE_2D);
  // }







  // SHADERS

  ngl::ShaderLib::use("ParticleShader");

  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3* sizeof(float)));
  glEnableVertexAttribArray(1);
  // glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(6* sizeof(float)));
  // glEnableVertexAttribArray(2);



  // Count and Draw
  int vertcount = sizeof(vertices) / (sizeof(vertices[0])*3);

  // glBindTexture(GL_TEXTURE_2D,texture);
  glBindVertexArray(VAO);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glDrawArrays(GL_TRIANGLES,0, vertcount);
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
