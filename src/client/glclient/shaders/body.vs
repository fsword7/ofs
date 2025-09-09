#version 430

layout (location = 0) in vec3 vPositionh;
layout (location = 1) in vec3 vPositionl;
layout (location = 2) in vec3 vNormal;
layout (location = 3) in vec2 vTexCoord;

uniform mat4 uViewProj;
// uniform mat4 uView;
uniform mat4 uModel;
uniform mat4 urte;

uniform vec3 uCamEyeHigh;
uniform vec3 uCamEyeLow;

// out vec4 eyePosition;
out vec3 normal;
out vec3 fragPos;
out vec2 texCoord;

void main()
{
    // vec3 t1 = vPositionl - uCamEyeLow;
    // vec3 e = t1 - vPositionl;
    // vec3 t2 = ((-uCamEyeLow - e) + (vPositionl - (t1 - e))) +
    //     vPositionh - uCamEyeHigh;
    // vec3 diffh = t1 + t2;
    // vec3 diffl = t2 - (diffh - t1);
    // vec3 pos = diffh + diffl;

    // gl_Position = urte * vec4(pos, 1.0);
    gl_Position = uViewProj * uModel * vec4(vPositionh, 1.0);

    normal = mat3(transpose(inverse(uModel))) * vNormal;
    fragPos = vec3(uModel * vec4(vPositionh, 1.0));
    texCoord = vTexCoord;

    // eyePosition = uView * uModel * vec4(vPosition, 1.0);
}