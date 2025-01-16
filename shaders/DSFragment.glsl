#version 410 core


in vec2 TexCoord;
uniform sampler2D tex;
layout (location=0)out vec4 FragColor;

//uniform sampler2D ourTexture;

void main()
{
    float ambientStrength = 0.1;

    FragColor = texture(tex,TexCoord);
}