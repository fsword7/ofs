#version 450

// vertex buffer objects
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 ePosition;
layout (location = 2) in vec3 vNormal;
layout (location = 3) in vec2 vTexCoord;

uniform mat4 mvp;

out vec4 myColor;
out vec2 texCoord;

void main()
{
    gl_Position = mvp * vec4(vPosition, 1.0);
    myColor = vec4(0.7, 0.7, 0.7, 1.0);
    texCoord = vTexCoord;
}