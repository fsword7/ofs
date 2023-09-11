// logdepth.h - Logarithmic depth buffer package
//
// Author:  Tim Stark
// Date:    Jul 25, 2023
//
// For more information, check https://outerra.blogspot.com/2012/11/maximizing-depth-buffer-range-and.html

#define vlogz() max(1e-6, gl_Position.w + 1.0)
#define flogz(z) (log2(z) / log2(1e11f + 1.0))