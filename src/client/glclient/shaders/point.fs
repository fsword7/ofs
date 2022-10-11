#version 450

in vec4 starColor;
out vec4 fragColor;

void main()
{
    // Rendering circular points
    vec2 circCoord = (gl_PointCoord * 2.0) - 1.0;
    if (dot(circCoord, circCoord) > 1.0)
        discard;

    fragColor = starColor;
}