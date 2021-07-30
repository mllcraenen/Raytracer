#ifndef TRIANGLE_H
#define TRIANGLE_H

#define EPSILON 0.00001

#include "corporeal.h"
#include "vec3.h"

class Triangle : public Corporeal {
    public: 
        Triangle () {}
        Triangle (Point3 v0, Point3 v1, Point3 v2, shared_ptr<Material> mat) 
            : v0(v0), v1(v1), v2(v2), matPtr(mat) {};

        virtual bool hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override;
        virtual bool boundingBox(double time0, double time1, AABB& outputBox) const override;
        bool mollerTrumboreIntersection(const Ray& r, Vec3& hitLocation, float& t) const;
    public:
        Point3 v0;
        Point3 v1;
        Point3 v2;
        shared_ptr<Material> matPtr;
};

bool Triangle::hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const {
    // Calculate whether the ray hits the triangle
    Vec3 hitLocation;
    float distance;
    bool hit = mollerTrumboreIntersection(r, hitLocation, distance);
    if (!hit) return false;
    
    // We hit. Add a record of it.
    rec.t = distance;
    rec.p = r.at(rec.t);
    rec.normal = cross((v1 - v0), (v2 - v0));
    Vec3 outwardNormal = cross((v1 - v0), (v2 - v0));
    rec.setFaceNormal(r, outwardNormal);
    rec.matPtr = matPtr;

    return true;
}

bool Triangle::mollerTrumboreIntersection(const Ray& r, Vec3& hitLocation, float& t) const {
    float baryU, baryV;
    Vec3 edge1 = v1 - v0;
    Vec3 edge2 = v2 - v0;

    Vec3 crossRayDirEdge2 = cross(r.direction(), edge2);
    float determinant = dot(edge1, crossRayDirEdge2);

    #ifdef CULLING
    // If the determinant is less than 0 the ray does not hit. 
    if (determinant < EPSILON) return false;
    #else 
    // If the determinant is close to 0 the ray and triangle are parallel.
    if (fabs(determinant) < EPSILON) return false;
    #endif
    // Calculate distance from v0 to ray origin
    Vec3 tvec = r.origin() - v0;
    // Calculate barycentric u coordinate
    baryU = dot(tvec, crossRayDirEdge2);
    // If the barycentric u coordinate is less than 0 or greater than the determinant we know we did not hit the triangle, quick exit.
    if (baryU < 0.0 || baryU > determinant) return false;

    Vec3 qvec = cross(tvec, edge1);
    baryV = dot(r.direction(), qvec);
    if (baryV < 0.0 || baryU + baryV > determinant) return 0;

    // We know we've hit:  finish up Cramer's rule operations. Divide by determinant.
    float invDet = 1/determinant;
    t = dot(edge2, qvec) * invDet;
    hitLocation[0] = baryU * invDet;
    hitLocation[1] = baryV * invDet;
    hitLocation[2] = 1 - baryU - baryV;

    return true;
}


bool Triangle::boundingBox(double time0, double time1, AABB& outputBox) const {
    Point3 minPoint = Point3(
        fmin( fmin(v0.x(), v1.x()),  v2.x()),
        fmin( fmin(v0.y(), v1.y()),  v2.y()),
        fmin( fmin(v0.z(), v1.z()),  v2.z())
    );
    Point3 maxPoint = Point3(
        fmax( fmax(v0.x(), v1.x()),  v2.x()),
        fmax( fmax(v0.y(), v1.y()),  v2.y()),
        fmax( fmax(v0.z(), v1.z()),  v2.z())
    );
    outputBox = AABB(minPoint, maxPoint);
    return true;
}

#endif