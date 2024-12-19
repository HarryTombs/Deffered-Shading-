#version 410 core

in vec3 ourColour;
in vec2 TexCoord;
uniform sampler2D tex;
out vec4 FragColor;

//uniform sampler2D ourTexture;

void main()
{
    FragColor = vec4(ourColour,1);
}