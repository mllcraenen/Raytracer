#ifndef SPHERE_H
#define SPHERE_H

#include "corporeal.h"
#include "vec3.h"

class Sphere : public Corporeal {
    public: 
        Sphere () {}
        Sphere (Point3 cen, double r) : center(cen), radius(r) {};

        virtual bool hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override;
    public:
        Point3 center;
        double radius;
};

bool Sphere::hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const {
    // Calculate the solution of the quadratic function with the sphere's equation.
    Vec3 origin_center = r.origin() - center;
    auto a = r.direction().lengthSquared();
    auto halfB = dot(origin_center, r.direction());
    auto c = origin_center.lengthSquared() - radius*radius;

    auto discriminant = halfB*halfB - a*c;
    if (discriminant < 0) return false;
    auto sqrtd = sqrt(discriminant);

    // Find the nearest solution that lies in the acceptable range
    auto solution = (-halfB - sqrtd) / a;
    if (solution < tMin || solution > tMax) {
        solution = (-halfB + sqrtd) / a;
        if (solution < tMin || solution > tMax) return false;
    }

    // That solution is the distance to the intersection. Add a record of it.
    rec.t = solution;
    rec.p = r.at(rec.t);
    rec.normal = (rec.p - center) / radius;
    Vec3 outwardNormal = (rec.p - center) / radius;
    rec.setFaceNormal(r, outwardNormal);

    return true;
}

#endif