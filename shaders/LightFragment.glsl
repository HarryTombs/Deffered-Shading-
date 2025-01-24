#version 410 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D gPos;
uniform sampler2D gNorm;
uniform sampler2D gColorSpec;

struct light
{
    vec3 Pos;
    vec3 Col;
};
const int NR_Lights = 32;
uniform light lights[NR_Lights];
uniform vec3 viewPos;


void main()
{
    vec3 Fragpos = texture(gPos, TexCoord).rgb;
    vec3 Normal = normalize(texture(gNorm, TexCoord).rgb);
    vec3 albedo = texture(gColorSpec, TexCoord).rgb;

    vec3 lighting = vec3(0.1);
    vec3 viewDir = normalize(viewPos - Fragpos);
    for(int i = 0; i < NR_Lights; i++)
    {
        vec3 lightDir = normalize(lights[i].Pos - Fragpos);

        float diff = max(dot(Normal,lightDir), 0.0);
        vec3 diffuse = diff * albedo * lights[i].Col;

        vec3 halfwaydir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(Normal, halfwaydir), 0.0), 64.0);
        vec3 specular = spec * lights[i].Col;

        lighting += diffuse + specular;
    }
    FragColor = vec4(lighting, 1.0);
}