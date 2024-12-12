#version 410 core

in vec3 ourColour;
//in vec2 TexCoord;

out vec4 FragColor;

//uniform sampler2D ourTexture;

void main()
{
    FragColor = vec4(ourColour,1.0);
}