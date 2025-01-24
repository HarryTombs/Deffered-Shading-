#version 410 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
uniform sampler2D tex;
layout (location=0)out vec4 gPos;
layout (location=1)out vec4 gNorm;
layout (location=2)out vec4 gColorSpec;



void main()
{
    gPos = vec4(FragPos,1.0);
    gNorm = vec4(normalize(Normal),1.0);
    gColorSpec.rgb = texture(tex,TexCoord).rgb;
    gColorSpec.a = 1.0;
}