#ifndef CORPOREAL_H
#define CORPOREAL_H

#include "tracer.h"

// Declare the Material class to satisfy compiler as it's not defined here yet. 
class Material;
class AABB;

struct HitRecord {
    Point3 p;
    Vec3 normal;
    shared_ptr<Material> matPtr;
    double t;
    // Surface coordinates of ray-object intersection point.
    double u;
    double v;
    // Whether the ray intersects from the front/outside or back/inside
    bool frontFace;
    // The ray intersected from the front/outside if the normal points in the opposite direction.
    // If they are facing in the same direction the ray hit from the back/inside.
    // The directions are compared with the dot product, where <0 means front hit.
    inline void setFaceNormal(const Ray &r, const Vec3& outwardNormal) {
        frontFace = dot(r.direction(), outwardNormal) < 0;
        normal = frontFace ? outwardNormal : -outwardNormal;
    }
};

class Corporeal {
    public:
        virtual bool hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const = 0;
        virtual bool boundingBox(double time0, double time1, AABB& outputBox) const = 0;
};

#endif