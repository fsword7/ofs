#version 430

#include "logdepth.glsl"
#include "atmo.glsl"

#define MAX_NLIGHTS 10

struct lightSource
{
    vec3 spos;  // sun position
    vec3 diffuse;
    vec3 specular;

    // float attenuation[4];
};

uniform vec3  uAmbient;
uniform int   unLights;
uniform lightSource lights[MAX_NLIGHTS];

uniform vec2  uCamClip;
// uniform float uCameraK;

layout (binding = 0) uniform sampler2D sTile;

// layout (std140) uniform Lights {
//     lightSource light[MAX_NLIGHTS];
//     int nLights;
// };

in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;

out vec4 fragColor;

// void swap(inout float v0, inout float v1)
// {
//     float tmp = v0;
//     v0 = v1;
//     v1 = tmp;
// }

// bool intersection(vec3 p1, vec3 p2, inout vec3 t1, inout t2, vec3 cplanet, float atmRadius2)
// {
//     vec3 dray = normalize(p2 - p1);
//     vec3 oc = p1 - cplanet;

//     float b = 2.0 * dot(dray, oc);
//     float c = dot(oc, oc) - atmRadius2;
//     float disc = b*b - 4.0*c;

//     t1 = p1;
//     t2 = p2;

//     if (disc < 0.0)
//         return false;
    
//     float d0 = (-b - sqrt(disc))/2.0;
//     float d1 = (-b + sqrt(disc))/2.0;

//     if (d0 > d1)
//         swap(d0, d1);
//     if (d1 < 0.0)
//         return false;
    
//     t1 = max(d0, 0.0) * dray + p1;
//     t2 = (d1 > distance(p1, p2) ? p2 : d1 * dray + p1);

//     return true;
// }

// float shadowDistance(vec3 p, float cosPhi)
// {
//     vec4 shadowMapCoord = depthBiasVP * vec4(p, 1);
//     if (shadowMapCoord.x > 1.0 || shadowMapCoord.x < 0.0 ||
//         shadowMapCoord.y > 1.0 || shadowMapCoord.y < 0.0 ||
//         shadowMapCoord.z > 1.0 || shadowMapCoord.z < 0.0 ||
//         cosPhi < -0.24) {
//         return cosPhi < -0.24 ? 1.0 : 0.0;
//     }
//     float bias = 0.0003;

//     return ((shadowMapCoord.z - bias) - texture(shadowMap, shadowMapCoord.xy).z);
// }

// float shadowDistanceBlur(vec3 p, float cosPhi)
// {
//     vec4 shadowMapCoord = depthBiasVP * vec4(p, 1);
//     if (shadowMapCoord.x > 1.0 || shadowMapCoord.x < 0.0 ||
//         shadowMapCoord.y > 1.0 || shadowMapCoord.y < 0.0 ||
//         shadowMapCoord.z > 1.0 || shadowMapCoord.z < 0.0 ||
//         cosPhi < -0.24) {
//         return 1.0;
//     }

//     float bias = 0.0003;
//     mat3 gaussion = mat3(2.25/25, 3.0/25, 2.25/25,
//                          3.0/25,  4.0/25, 3.0/25,
//                          2.25/25, 3.0/25, 2.25/25);

//     float diff = 0.0;
//     float currShadow = 0.0;
//     vec2 pp = vec2(0.0, 0.0);
//     for (int i = -1; i < 2; i++) {
//         for (int j = -1; j < 2; j++) {
//             pp = vec2(shadowMapCoord.x + bias * i, shadowMapCoord.y + bias * j);
//             currShadow = ((shadowMapCoord.z - bias) - texture(shadowMap, pp).z);
//             diff += currShadow * gaussion[i+1][j+1];
//         }
//     }

//     return diff > 0.0 ? exp(-diff * 750) : 1.0;
// }

// {
//     vec4 transfVec = vec4(256.0*256.0*256.0, 256.0*256.0, 256.0, 1) * 256.0;
//     float nSteps = 30.0;

//     float _3_16pi = 3.0 / (16.0 * pi);
//     float _3_8pi = 3.0 / (8.0 * pi);
    
//     mat4 projTrans = transpose(mvp);
//     vec3 view = normalize(projTrans[2].xyz);
//     vec3 normLightDir = normalize(lightDir);

//     vec3 computedCam;
//     vec3 obj2;

//     bool q = intersection(cam, obj, computedCam, obj2, cPlanet, ATM_RADIUS_2);
//     vec3 distanceVec = (obj2 - computedCam);
//     float distanceToPoint = length(distanceVec);
//     float reducedSteps = clamp(distanceToPoint/(ATM_RADIUS/10), 1.0, 1.0) * nSteps;
//     vec3 deltaP = distanceVec/reducedSteps;

//     float differentalS = length(deltaP);

//     vec2 denistyPC = vec2(0.0, 0.0);
//     vec3 rayleighIn = vec3(0.0, 0.0, 0.0);
//     vec3 mieIn = vec3(0.0, 0.0, 0.0);

//     if (q) for (float s = 0.5; s < reducedSteps; s += 1.0) {
//         vec3 point = computedCam + deltaP * s;
//         float h = max(length(point - cPlanet) - WORLD_RADIUS, 0.0);
//         vec3 normalPlanet = normalize(point - cPlanet);
//         vec2 partDenRM = P0 * exp(-h / vec2(H_R, H_M));
//         float cosPhi = dot(normalPlanet, -normLightDir);

//         vec2 densityCoord = vec2((h/ATM_TOP_HEIGHT), (cosPhi + 1.0)/2.0);
//         vec4 vDAPray = texture(densityRayleigh, densityCoord);
//         vec4 vDAPmie = texture(densityMie, densityCoord);
//         float fDAPray = dot(transfVec, vDAPray.abgr);
//         float fDAPmie = dot(transfVec, vDAPmie.abgr);

//         vec2 densityAP = vec2(fDAPray, fDAPmie);

//         denistyPC += partDenRM * differentalS;
//         vec2 densityAPC = densityAP + densityPC;

//         vec3 extinctionRM = exp(-(densityAPC.x * betaER + densityAPC.y * betaEM));
//         vec3 difLR = partDenRM.x * betaSR * extinctionRM * differentalS;
//         vec3 difLM = partDenRM.y * betaSM * extinctionRM * differentalS;

//         float visi = shadowDistance(point, cosPhi) > 0.0 ? 0.0 : 1.0;

//         rayleighIn += difLR * visi;
//         mieIn += difLM * visi;
//     }

//     float cosTheta = dot(normalize(-=distanceVec), normLightDir);
//     float cos2ThetaP1 = 1.0 + clamp((cosTheta * abs(cosTheta)), 0.0, 1.0);

//     float phaseRayleigh = _3_16pi * cos2ThetaP1;
//     float phaseMie = _3_8pi * (((1.0 - G2) * cos2ThetaP1) /
//         ((2.0+G2) * pow(1.0 + G2 - G*cosTheta, 1.5)));
    
//     vec3 inScattering = (rayleighIn * phaseRayleigh + mieIn * phaseRayleigh) * lightSun;
//     vec3 extintion = exp(-(denistyPC.x * betaER + denistyPC.y * betaEM));
// }

void addLight(in lightSource light, in vec3 normal, in vec3 fragPos, in vec3 viewDir,
    in float specPower, inout vec3 diffuse, inout vec3 specular)
{
    float diff, spec;
    vec3 lightDir = normalize(light.spos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    diff = clamp(dot(normal, lightDir), 0.0, 1.0);
    spec = pow(clamp(dot(viewDir, -reflectDir), 0.0, 1.0), specPower);

    diffuse += light.diffuse * diff;
    specular += light.diffuse * spec;
}

void main()
{
    float uCameraK = 1.0;


    fragColor = texture(sTile, texCoord);

    vec3 diff = uAmbient;
    vec3 spec = vec3(0.0);
    vec3 norm = normalize(-normal);
    vec3 vdir = normalize(-fragPos);

    for (int idx = 0; idx < unLights; idx++)
        addLight(lights[idx], norm, fragPos, vdir,
            1.0, diff, spec);

    fragColor.rgb *= diff + spec;

    gl_FragDepth = getDepth(uCamClip.y, uCameraK);
}