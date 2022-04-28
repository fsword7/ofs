#version 450

layout (location = 0) in vec4 vertex;
out vec2 texCoords;

uniform mat4 proj;

void main()
{
	gl_Position = proj * vec4(vertex.xy, 0.0, 1.0);
	texCoords = vertex.zw;
}
