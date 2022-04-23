#version 450

#define NLIGHTS 8

layout (binding = 0) uniform sampler2D sTexture;

struct LightSource {
    vec4 direction;
    vec4 diffuse;
    vec4 specular;
};

layout (std140) uniform Lights 
{
    vec3 ambient;
    int  nLights;
    LightSource lights[NLIGHTS];
};

in vec2 texCoord;
in vec4 myColor;

out vec4 fragColor;

void main()
{
    fragColor = texture(sTexture, texCoord);
    // fragColor = myColor;
}