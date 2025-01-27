#ifndef NGLSCENE_H_
#define NGLSCENE_H_
#include <ngl/Vec3.h>
#include <ngl/Mat4.h>
#include <ngl/Obj.h>
#include <ngl/Transformation.h>
#include "WindowParams.h"
#include "Mesh.h"
#include "FirstPersonCamera.h"
#include <QOpenGLWindow>
#include <QElapsedTimer>
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
    WinParams m_win;

    std::string m_objFileName;
    std::string m_texFileName;

    ngl::Mat4 m_view;
    ngl::Mat4 m_projection;
    FirstPersonCamera m_cam;

    QElapsedTimer m_timer;
    float m_deltatime = 0.0f;
    float m_lastframe = 0.0f;
    QSet<Qt::Key> m_keysPressed;


    struct Lights
    {
        ngl::Vec3 lightsPos;
        float padding;
        ngl::Vec3 lightsCol;
        float radius;
    }; // struct matching the one in the light shader in case that will make it not work
    std::vector<Lights> lightArray;

    const float constant = 1.0f;
    const float linear = 0.7f;
    GLfloat quadratic = 1.8f;

    std::vector<ngl::Vec3> lightPos;
    std::vector<ngl::Vec3> lightCol;
    ngl::Transformation lightTrans;
    float lightDiff = 1;
    int numLights = 32;
    int maxLights = 200; // 200 because beyond that its just pure white basically
    void changeLights();
    void clearLights();

    int numMesh = 10;
    std::vector<Mesh> meshArray;
    Mesh mesh;

    unsigned int gBuffer;
    unsigned int ssBuffer;
    unsigned int gPos, gNorm, gColorSpec; // i make a lot of global ints because i use them across a lot of functions

    void renderQuad();
    void renderCube();
    void loadMatricesToShader(std::string ProgramName, bool CalcMatrix);


};



#endif
