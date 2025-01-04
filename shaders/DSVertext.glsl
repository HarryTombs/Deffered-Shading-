#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aUV;

uniform mat4 MVP;
out vec2 TexCoord;


void main()
{
    gl_Position = MVP*vec4(aPos,1.0);
    TexCoord = aUV.st;

}
