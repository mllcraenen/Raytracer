#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include <iostream>

// Writes a PPM format pixel with the Colors from the `Color` vector to the provided output stream.
void writeColor(std::ostream &out, Color pixelColor) {
    // Map the [0,1] Color value to the range [0,255].
    int r = static_cast<int>(255.999 * pixelColor.r());
    int g = static_cast<int>(255.999 * pixelColor.g());
    int b = static_cast<int>(255.999 * pixelColor.b());

    // Output the PPM format pixel.
    out << r << ' ' << g << ' ' << b << '\n';
}

#endif