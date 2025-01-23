#version 410 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
uniform sampler2D tex;
layout (location=0)out vec3 gPos;
layout (location=1)out vec3 gNorm;
layout (location=2)out vec4 gColorSpec;

void main()
{
    gPos = FragPos;
    gNorm = normalize(Normal);
    gColorSpec = texture(tex,TexCoord);
}