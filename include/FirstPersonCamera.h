#ifndef FIRSTPERSONCAMER_H_
#define FIRSTPERSONCAMER_H_
#include <ngl/Vec3.h>
#include <ngl/Mat4.h>
#include <ngl/Transformation.h>
#include <memory>

class FirstPersonCamera
{
public:
    ngl::Vec3 camPos = ngl::Vec3(2.0f,2.0f,2.0f); // Position
    ngl::Vec3 camTo = ngl::Vec3(0.0f,0.0f,-1.0f); // Start looking pos
    ngl::Vec3 camUp; // Local up
    ngl::Vec3 worldUp = ngl::Vec3(0.0f,1.0f,0.0f); // World Up
    ngl::Vec3 camFront; //Where the camera is looking
    ngl::Vec3 camRight; // Right of the camera
    ngl::Mat4 m_view; // view matrix
    ngl::Mat4 m_projection; // projection matrix
    ngl::Real m_FOV = 45.0f;
    ngl::Real m_aspect = 1.2f;
    ngl::Real m_near = 0.1f;
    ngl::Real m_far = 100.0f;
    ngl::Real m_yaw = -90.0f;
    ngl::Real m_pitch = 0.0f;
    ngl::Real m_speed = 0.75f;
    ngl::Real m_sensitivity = 0.1f;

    int width;
    int height;

    FirstPersonCamera()=default;
    FirstPersonCamera(ngl::Vec3 from, ngl::Vec3 to, ngl::Vec3 up);
    void set(ngl::Vec3 from,ngl::Vec3 to,ngl::Vec3 up);
    void setProjection(float FOVdeg, float aspect, float nearPlane, float farPlane );
    void setView();
    void setVectors();
    void processMouseMovement(float _newX,float _newY,GLboolean constrainPitch = true);
    ngl::Mat4 getView(){return m_view;}
    ngl::Mat4 getProjection(){return m_projection;}

    void Move(float _x, float _y, float _deltaTime);

private:
};


#endif
