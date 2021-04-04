#ifndef CAMERA_H
#define CAMERA_H

#include "tracer.h"

class Camera {
    public:
        Camera() {
            auto aspectRatio = 16.0 / 9.0;
            auto viewportHeight = 2.0;
            auto viewportWidth = aspectRatio * viewportHeight;
            auto focalLength = 1;
            
            //======================================

            origin = Point3(0, 0, 0);
            hor = Vec3(viewportWidth, 0, 0);
            ver = Vec3(0, viewportHeight, 0);
            llCorner = origin - hor / 2 - ver / 2 - Vec3(0, 0, focalLength);
        }

        Ray getRay(double u, double v) const {
            return Ray(origin, llCorner + u * hor + v * ver - origin);
        }
    private:
        Point3 origin;
        Point3 llCorner;
        Vec3 hor;
        Vec3 ver;
};

#endif