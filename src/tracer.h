#ifndef TRACER_H
#define TRACER_H

#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>


// Utility constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

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
const int imageWidth = 1920;
const int imageHeight = (int)(imageWidth / aspectRatio);

const int samplesPerPixel = 100;
const int maxBounceDepth = 50;
const double imageGamma = 2.0;

////Variables
Point3 cameraOrigin = Point3(13, 2, 3);
Point3 cameraLookAt = Point3(0, 0, 0);
Vec3 cameraUp = Vec3(0, 1, 0);
auto hFOV = 50.0;
auto distToFocus = 10.0;
auto aperture = 0.1;

Camera cam(cameraOrigin, cameraLookAt, cameraUp, hFOV, aspectRatio, aperture, distToFocus);


#endif