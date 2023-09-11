// logdepth.glsl - Logarithmic depth buffer package
//
// Author:  Tim Stark
// Date:    Jul 25, 2023
//
// For more information, check https://outerra.blogspot.com/2012/11/maximizing-depth-buffer-range-and.html

float getDefaultDepth(float z, float near, float far)
{
    return ((1.0 / z) - (1.0 / near)) / ((1.0 / far) - (1.0 / near));
}

float getLogarithmicDepth(float z, float far, float k)
{
    return log(k * z + 1.0) / log(far * k + 1.0);
}

float getDepth(float far, float k)
{
    float w = 1.0 / gl_FragCoord.w;
    return getLogarithmicDepth(w, far, k);
}
