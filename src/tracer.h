#ifndef TRACER_H
#define TRACER_H

#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>

//// Common headers

#include "vec3.h"
#include "ray.h"
#include "camera.h"

//// Usings
using std::shared_ptr;
using std::make_shared;
using std::sqrt;

//// Constants
// Define image parameters
const auto aspectRatio = 16.0 / 9.0;
const int imageWidth = 400;
const int imageHeight = (int)(imageWidth / aspectRatio);
const int samplesPerPixel = 100;

// Utility
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

////Variables
// Declare camera and define parameters
auto viewportHeight = 2.0;
auto viewportWidth = aspectRatio * viewportHeight;
auto focalLength = 1.0;

auto origin = Point3(0, 0, 0);
auto horizontal = Vec3(viewportWidth, 0, 0);
auto vertical = Vec3(0, viewportHeight, 0);
auto llCorner = origin - horizontal/2 - vertical/2 - Vec3(0, 0, focalLength);

Camera cam;


//// Utility functions
inline double degreesToRadians(double deg) {
    return deg * pi / 180.0;
}

inline double randomDouble() {
    // Returns a random real in [0,1)
    return rand() / (RAND_MAX + 1.0);
}

inline double randomDouble(double min, double max) {
    // Returns a random real in [min,max)
    return min + (max-min) * randomDouble();
}

inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    else return x;
}
//Clamp for a number between 0 and 1
inline double clamp1(double x) {
    return clamp(x, 0, 0.999);
}

#endif