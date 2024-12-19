#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColour;

uniform mat4 MVP;

out vec3 ourColour;


void main()
{
    gl_Position = MVP*vec4(aPos,1.0);
    ourColour = aColour;

}
