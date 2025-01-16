#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aUV;

uniform mat4 MVP;
uniform mat3 normalMatrix;
out vec2 TexCoord;
out vec3 Normal;


void main()
{
    gl_Position = MVP*vec4(aPos,1.0);
    TexCoord = aUV.st;
}
