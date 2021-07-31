#ifndef AABB_H
#define AABB_H

#include "tracer.h"
#include "corporeal.h"
#include "material.h"

class AABB {
    public:
        AABB() {}
        AABB(const Point3& a, const Point3& b) { minimum = a; maximum = b;}

        Point3 min() const { return minimum; }
        Point3 max() const { return maximum; }
       
        bool hit(const Ray& r, double tMin, double tMax) const {
            // Loop over each axis
            for (int i = 0; i < 3; i++) {
                auto invD = 1.0f / r.direction()[i];
                // The first slab edge of the aabb slab method is the smallest coordinate of this axis 
                //  minus the origin divided by the whole length (makes it into a percentage of total 
                //  distance/time t). 
                auto t0 = (minimum[i] - r.origin()[i]) * invD;
                // The far slab edge is the maximum of the AABB minus the origin divided by d.
                auto t1 = (maximum[i] - r.origin()[i]) * invD;

                // Actually, if the ray is shooting from the other side then t1 will be the near hit and t0 the far hit, swap them.
                if (invD < 0.0f) std::swap(t0, t1);
                
                // Check whether there is an intersection. See https://youtu.be/8JJ-4JgR7Dg?t=1034 
                //   for a good explanation and visualization of all this.
                //   tMin and tMax are the near and far coordinates of the previous intersection here.
                //   These code lines test for the intersection rules:
                //   t0(x) < t1(y) && t0(y) < t1(x)  [near must be smaller than far in any axis combination!]
                tMin = t0 > tMin ? t0 : tMin;
                tMax = t1 < tMax ? t1 : tMax;

                if(tMax <= tMin) return false;
            }
            return true;
        }
       
        bool hitFrame(const Ray& r, double tMin, double tMax, HitRecord& rec) const {
            for (int i = 0; i < 3; i++) {
                auto invD = 1.0f / r.direction()[i];
                // The first slab edge of the aabb slab method is the smallest coordinate of this axis 
                //      minus the origin divided by the direction vector
                auto t0 = (minimum[i] - r.origin()[i]) * invD;
                auto t1 = (maximum[i] - r.origin()[i]) * invD;

                if (invD < 0.0f) std::swap(t0, t1);
                
                if (t0 > tMin) tMin = t0;
                if (t1 < tMax) tMax = t1;

                if(tMax <= tMin) return false;
            }

            //tMin is the closest intersection point so set it as rec.t
            rec.t = tMin;
            rec.p = r.at(rec.t);
            // Point3 center = maximum - minimum;
            // // Divisor is hardcoded experimentally, is supposed to be radius or some shit
            // rec.normal = (rec.p - center) / (maximum.x() - minimum.x());
            // // Here too
            // Vec3 outwardNormal = (rec.p - center) / (maximum.x() - minimum.x());
            // rec.setFaceNormal(r, outwardNormal);
            auto outwardNormal = Vec3(0,0,1);
            rec.setFaceNormal(r, outwardNormal);
            rec.matPtr = make_shared<Lambertian>(Color(1.0, 0.05, 0.05));;
            return true;
        }

        Point3 minimum;
        Point3 maximum;
};

AABB surroundingBox(AABB box0, AABB box1) {
    Point3 small(fmin(box0.min().x(), box1.min().x()),
                fmin(box0.min().y(), box1.min().y()),
                fmin(box0.min().z(), box1.min().z()));

    Point3 big(fmax(box0.max().x(), box1.max().x()),
                fmax(box0.max().y(), box1.max().y()),
                fmax(box0.max().z(), box1.max().z()));
    return AABB(small,big);
}

AABB frameBox(AABB box, float thickness = 0.03f) {
    Point3 small = box.min();
    Point3 big = box.max();
    small += Vec3(thickness, thickness, thickness);
    big -= Vec3(thickness, thickness, thickness);
    return AABB(small, big);
}


#endif