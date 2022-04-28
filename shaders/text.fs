#version 450

in vec2 texCoords;
out vec4 fragColor;

layout (binding = 0) uniform sampler2D text;

uniform vec3 textColor;

void main()
{
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, texCoords).r);
	fragColor = vec4(textColor, 1.0) * sampled;
}