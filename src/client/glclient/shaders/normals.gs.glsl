#version 430 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT { vec3 normal; } vs[];

const float MAGNITUDE = 0.4;

uniform mat4 uProj;

void generateLine(int idx)
{
    gl_Position = uProj * gl_in[idx].gl_Position;
    EmitVertex();

    gl_Position = uProj * (gl_in[idx].gl_Position + 
        vec4(vs[idx].normal, 0.0) * MAGNITUDE);
    EmitVertex();

    EndPrimitive();
}

void main()
{
    generateLine(0);
    generateLine(1);
    generateLine(2);
}