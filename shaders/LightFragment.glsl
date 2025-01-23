#version 410 core

in vec2 TexCoord;

uniform sampler2D gPos;
uniform sampler2D gNorm;
uniform sampler2D gColorSpec;

out vec4 FragColor;

struct light
{
    vec3 Pos;
    vec3 Col;
};
const int NR_Lights = 20;
uniform light lights[NR_Lights];
uniform vec3 viewPos;


void main()
{
    vec3 Fragpos = texture(gPos, TexCoord).rgb;
    vec3 Normal = texture(gNorm, TexCoord).rgb;
    vec3 albedo = texture(gColorSpec, TexCoord).rgb;
    vec3 lighting = albedo * 0.5;
    vec3 viewDir = normalize(viewPos - Fragpos);
    for(int i = 0; i < NR_Lights; i++)
    {
        vec3 lightDir = normalize(lights[i].Pos - Fragpos);
        vec3 diffuse = max(dot(Normal,lightDir), 0.0) * albedo * lights[i].Col;
        lighting += diffuse;
    }
    FragColor = vec4(lighting, 1.0);
}