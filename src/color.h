#ifndef COLOR_H
#define COLOR_H

#include "tracer.h"
#include <iostream>

// Writes a PPM format pixel with the Colors from the `Color` vector to the provided output stream.
void writeColor(std::ostream &out, Color pixelColor, int samplesPerPixel) {
    auto r = pixelColor.r();
    auto g = pixelColor.g();
    auto b = pixelColor.b();

    // Divide the color by the number of samples. Then do gamma correction -> raise color to the power 1/γ 
    auto scale = 1.0 / samplesPerPixel;
    double gammaPower = 1.0 / imageGamma;

    r = pow((scale * r), gammaPower);
    g = pow((scale * g), gammaPower);
    b = pow((scale * b), gammaPower);
    
    // r = sqrt(scale * r);
    // g = sqrt(scale * g);
    // b = sqrt(scale * b);

    // Output the PPM format pixel after remapping the clamped RGB values from [0,1] to [0,255].
    out << (int)(clamp1(r) * 256) << ' ' << (int)(clamp1(g) * 256) << ' ' << (int)(clamp1(b) * 256) << '\n';
}

#endif