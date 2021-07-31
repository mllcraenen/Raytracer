#ifndef SPHERE_H
#define SPHERE_H

#include "corporeal.h"
#include "vec3.h"
#include "aabb.h"

class Sphere : public Corporeal {
    public: 
        Sphere () {}
        Sphere (Point3 cen, double r, shared_ptr<Material> mat) 
            : center(cen), radius(r), matPtr(mat) {};

        virtual bool hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override;
        virtual bool boundingBox(double time0, double time1, AABB& outputBox) const override;
    public:
        Point3 center;
        double radius;
        shared_ptr<Material> matPtr;

    private:
        /**
         * p: point on a unit sphere in the origin. 
         * u: returned value [0,1] of angle around Y axis from X=-1.
         * v: returned value [0,1] of angle from Y=-1 to Y=+1
         */
        static void getSphereUV(const Point3& p, double& u, double& v) {
            auto theta = acos(-p.y());
            auto phi = atan2(-p.z(), p.x()) + pi;
            u = phi / (2*pi);
            v = theta / pi;
        }
};

bool Sphere::boundingBox(double time0, double time1, AABB& outputBox) const {
    outputBox = AABB(
        center - Vec3(radius, radius, radius),
        center + Vec3(radius, radius, radius));
    return true;
}

bool Sphere::hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const {
    // Calculate the solution of the quadratic function with the sphere's equation.
    Vec3 origin_center = r.origin() - center;
    auto a = r.direction().lengthSquared();
    auto halfB = dot(origin_center, r.direction());
    auto c = origin_center.lengthSquared() - radius*radius;

    auto discriminant = halfB*halfB - a*c;
    if (discriminant < 0) return false;
    auto sqrtd = sqrt(discriminant);

    // Find the nearest solution that lies in the acceptable distance range
    // Start with the - solution
    auto solution = (-halfB - sqrtd) / a;
    if (solution < tMin || solution > tMax) {
        // If that's outside the t range go to the + solution.
        solution = (-halfB + sqrtd) / a;
        // If that is also outside the range return false.
        if (solution < tMin || solution > tMax) return false;
    }

    // That solution is the distance to the intersection. Add a record of it.
    rec.t = solution;
    rec.p = r.at(rec.t);
    rec.normal = (rec.p - center) / radius;
    Vec3 outwardNormal = (rec.p - center) / radius;
    rec.setFaceNormal(r, outwardNormal);
    getSphereUV(outwardNormal, rec.u, rec.v);
    rec.matPtr = matPtr;

    return true;
}

#endif