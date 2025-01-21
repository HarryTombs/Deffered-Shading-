#version 410 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
uniform sampler2D tex;
layout (location=0)out vec4 FragColor;

void main()
{
    float ambientStrength = 0.1;
//    FragColor = vec4(FragPos,1.0); //<--¬
//    FragColor = vec4(Normal,1.0); // <---- For testing
    FragColor = texture(tex,TexCoord);
}