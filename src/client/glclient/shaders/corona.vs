#version 430

// In/Out
in vec2 vPosition;
out vec3 fPosition;

// Uniforms
uniform mat4  uMVP;
uniform vec3  uCenter;
uniform vec3  uCameraRight;
uniform vec3  uCameraUp;
uniform float uMaxSize;
uniform float uStarRadius;

void main()
{
    fPosition = (uCameraRight * vPosition.x + uCameraUp * vPosition.y);
    vec3 vpw = fPosition * uMaxSize;
    vpw = uCenter + vpw * uStarRadius;

    gl_Position = uMVP * vec4(vpw, 1.0);
}