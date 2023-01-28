#version 430

layout (binding = 0) uniform sampler2D sTile;

in vec4 texCoord;
in vec3 normal;

out vec4 fragColor;

void main()
{
    fragColor = vec4(0.7, 0.7, 0.7, 1.0);
    // fragColor = texture(sTile, texCoord);
}