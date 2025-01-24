#ifndef NGLSCENE_H_
#define NGLSCENE_H_
#include <ngl/Vec3.h>
#include <ngl/Mat4.h>
#include <ngl/Obj.h>
#include <ngl/Transformation.h>
#include "WindowParams.h"
#include "Mesh.h"
#include "FirstPersonCamera.h"
// this must be included after NGL includes else we get a clash with gl libs
#include <QOpenGLWindow>
#include <QElapsedTimer>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
//----------------------------------------------------------------------------------------------------------------------
/// @file NGLScene.h
/// @brief this class inherits from the Qt OpenGLWindow and allows us to use NGL to draw OpenGL
/// @author Jonathan Macey
/// @version 1.0
/// @date 10/9/13
/// Revision History :
/// This is an initial version used for the new NGL6 / Qt 5 demos
/// @class NGLScene
/// @brief our main glwindow widget for NGL applications all drawing elements are
/// put in this file
//----------------------------------------------------------------------------------------------------------------------

class NGLScene : public QOpenGLWindow
{
  public:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief ctor for our NGL drawing class
    /// @param [in] parent the parent window to the class
    //----------------------------------------------------------------------------------------------------------------------
    NGLScene(const std::string &_objName, const std::string &_texName);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief dtor must close down ngl and release OpenGL resources
    //----------------------------------------------------------------------------------------------------------------------
    ~NGLScene() override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the initialize class is called once when the window is created and we have a valid GL context
    /// use this to setup any default GL stuff
    //----------------------------------------------------------------------------------------------------------------------
    void initializeGL() override;
    void loadMatricesToShader(std::string ProgramName);

    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this is called everytime we want to draw the scene
    //----------------------------------------------------------------------------------------------------------------------
    void paintGL() override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this is called everytime we resize the window
    //----------------------------------------------------------------------------------------------------------------------
    void resizeGL(int _w, int _h) override;
    //----------------------------------------------------------------------------------------------------------------------
    /// IMAGE INPUT
    //----------------------------------------------------------------------------------------------------------------------



    GLuint CreateShader(const std::string& filePath, GLenum shaderType);
    std::string readShaderFromFile(const std::string& filePath);

private:

    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Qt Event called when a key is pressed
    /// @param [in] _event the Qt event to query for size etc
    //----------------------------------------------------------------------------------------------------------------------
    void keyPressEvent(QKeyEvent *_event) override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called every time a mouse is moved
    /// @param _event the Qt Event structure
    //----------------------------------------------------------------------------------------------------------------------
    void mouseMoveEvent (QMouseEvent * _event ) override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called everytime the mouse button is pressed
    /// inherited from QObject and overridden here.
    /// @param _event the Qt Event structure
    //----------------------------------------------------------------------------------------------------------------------
    void mousePressEvent ( QMouseEvent *_event) override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called everytime the mouse button is released
    /// inherited from QObject and overridden here.
    /// @param _event the Qt Event structure
    //----------------------------------------------------------------------------------------------------------------------
    void mouseReleaseEvent ( QMouseEvent *_event ) override;

    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called everytime the mouse wheel is moved
    /// inherited from QObject and overridden here.
    /// @param _event the Qt Event structure
    //----------------------------------------------------------------------------------------------------------------------
    void wheelEvent( QWheelEvent *_event) override;
    /// @brief windows parameters for mouse control etc.
    WinParams m_win;
    /// position for our model
    ngl::Vec3 m_modelPos;
    ngl::Mat4 m_view;
    ngl::Mat4 m_projection;
    ngl::Mat4 m_mouseGlobalTX;
    std::unique_ptr<ngl::Obj> m_mesh;
    std::string m_objFileName;
    std::string m_texFileName;
    FirstPersonCamera m_cam;
    QElapsedTimer m_timer;
    float m_deltatime = 0.0f;
    float m_lastframe = 0.0f;
    QSet<Qt::Key> m_keysPressed;

    std::vector<ngl::Vec3> lightPos;
    std::vector<ngl::Vec3> lightCol;
    int numLights = 32;

    unsigned int gBuffer;




    void renderQuad();



};



#endif
