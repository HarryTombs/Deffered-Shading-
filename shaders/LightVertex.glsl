#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;


void main()
{
    TexCoord = aTexCoord;
    gl_Position = vec4(aPos,1);
}
