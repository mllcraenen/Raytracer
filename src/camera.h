#ifndef CAMERA_H
#define CAMERA_H

#include "tracer.h"

class Camera {
    public:
        Camera(
            Point3 lookFrom, // Camera origin point
            Point3 lookAt,
            Vec3 vUp, // Camera up vector (aka determines camera roll)
            double hFOV, // Horizontal field-of-view in degrees
            double aspectRatio,
            double aperture,
            double focusDistance
        ) {
            // h is the distance from viewport center to the left/right side of the viewport. TOA -> h = tan(hFOV[in rads] / 2)
            auto h = tan(degreesToRadians(hFOV) / 2);
            auto viewportWidth = 2.0 * h;
            auto viewportHeight = viewportWidth / aspectRatio;
            
            // (u,v,w) specifies an orthonormal basis to describe the camera orientation. 
            // vUp is the up vector which specifies the camera's roll around the lookFrom-lookAt axis.
            w = unitVector(lookFrom - lookAt);
            u = unitVector(cross(vUp, w));
            v = cross(w, u);
            
            //======================================

            origin = lookFrom;
            hor = focusDistance * viewportWidth * u;
            ver = focusDistance * viewportHeight * v;
            llCorner = origin - hor / 2 - ver / 2 - focusDistance * w;

            lensRadius = aperture / 2;
        }

        Ray getRay(double s, double t) const {
            Vec3 rd = lensRadius * randomVectorInUnitDisk();
            Vec3 offset = u * rd.x() + v * rd.y();

            return Ray(origin + offset, llCorner + s * hor + t * ver - origin - offset);
        }
    private:
        Point3 origin;
        Point3 llCorner;
        Vec3 hor;
        Vec3 ver;
        Vec3 u,v,w;
        double lensRadius;
};

#endif