#include "FirstPersonCamera.h"
#include <ngl/Util.h>

FirstPersonCamera::FirstPersonCamera(ngl::Vec3 from, ngl::Vec3 to, ngl::Vec3 up)
{
    camPos = from;
    camTo = to;
    camUp = up;
    setVectors();
    // initialiser with inputs if needed
}

void FirstPersonCamera::set(ngl::Vec3 from, ngl::Vec3 to, ngl::Vec3 up)
{
    camPos = from;
    camTo = to;
    camUp = up;
    setVectors();
    // Sets based on inputs, makes it easier to edit within the NGLscene
}


void FirstPersonCamera::setProjection(float FOVdeg, float aspect, float nearPlane, float farPlane)
{
    m_FOV = FOVdeg;
    m_aspect = aspect;
    m_near = nearPlane;
    m_far = farPlane;
    m_projection = ngl::perspective(FOVdeg,aspect,nearPlane,farPlane);
    // sets the projection based on inputs so the MVP calclation has an accurate view
    // means we can have fake camera lens!
}

void FirstPersonCamera::setVectors()
{
    camFront.m_x = cosf(ngl::radians(m_yaw) * cosf(ngl::radians(m_pitch)));
    camFront.m_y = sinf(ngl::radians(m_pitch));
    camFront.m_z = sinf(ngl::radians(m_yaw)) * cosf(ngl::radians(m_pitch));
    camFront.normalize();
    // Taking the new pitch and yaw values (angles) and turning theminto radians so it can be put in a vector

    // I'll be honest I could not replicate this off the cuff
    // but that's what googling the roation matrix is for

    camRight = camFront.cross(worldUp); // World up because that's always perpendicular to the cam right
    camUp = camRight.cross(camFront); // then use right to get the cam local up
    // I actually use this in houdini when making direct parralel curves

    camRight.normalize();
    camFront.normalize(); //normalise them

    m_view=ngl::lookAt(camPos,camPos + camFront,camUp);
    // updates vectors based on change in position
    // uses a radians translation of the change in pitch or yaw to update vector
    // front and right are important since they're used to translate relative to the cameras positon

}


void FirstPersonCamera::Move(float _x, float _y, float _deltaTime)
{
    float velocity = m_speed * _deltaTime; // change in time since last update * the speed of move
    camPos += camFront * velocity * _x;
    camPos += camRight * velocity * _y;
    setVectors();
    // moves the camera based on the directional (not mouse rotation) input
}

void FirstPersonCamera::processMouseMovement(float _newX, float _newY, GLboolean constrainPitch)
{
    _newX *= m_sensitivity;
    _newY *= m_sensitivity; // Multiply by the sensitivity value

    m_yaw -= _newX; // applying to the pitch and yaw
    m_pitch += _newY; // I have them as opposites because i get confused otherwise

    setVectors(); // send them to the reset!
}


// It's hard to make this orignal so im leaving lots of comments to show I understand it
