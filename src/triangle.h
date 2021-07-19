#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "corporeal.h"
#include "vec3.h"

class Triangle : public Corporeal {
    public: 
        Triangle () {}
        Triangle (Point3 v1, Point3 v2, Point3 v3, shared_ptr<Material> mat) 
            : v1(v1), v2(v2), v3(v3), matPtr(mat) {};

        virtual bool hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override;
    public:
        Point3 v1;
        Point3 v2;
        Point3 v3;
        shared_ptr<Material> matPtr;
};

bool Triangle::hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const {
    // Calculate whether 
    
    
    
    // Calculate the solution of the quadratic function with the sphere's equation.
    

    // That solution is the distance to the intersection. Add a record of it.
    rec.t = solution;
    rec.p = r.at(rec.t);
    rec.normal = (rec.p - center) / radius;
    Vec3 outwardNormal = (rec.p - center) / radius;
    rec.setFaceNormal(r, outwardNormal);
    rec.matPtr = matPtr;

    return true;
}