#ifndef AARECT_H
#define AARECT_H

#include "tracer.h"
#include "corporeal.h"

class XY_Rectangle : public Corporeal {
    public:
        XY_Rectangle() {}
        XY_Rectangle(double _x0, double _x1, double _y0, double _y1, double _z, shared_ptr<Material> mat)
            : x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_z), matPtr(mat) {};

        virtual bool hit (const Ray& r, double tMin, double tMax, HitRecord& rec) const override;

        virtual bool boundingBox(double time0, double time1, AABB& outputBox) const override {
            // The bounding box must have a non-zero thickness in all dimensions
            outputBox = AABB(Point3(x0, y0, k-0.0001), Point3(x1, y1, k+0.0001));
            return true;
        }

    public:
        shared_ptr<Material> matPtr;
        double x0, x1, y0, y1, k;
};

bool XY_Rectangle::hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const {
    auto t = (k - r.origin().z()) / r.direction().z();
    if (tMin > t || t > tMax) return false;

    auto x = r.origin().x() + t * r.direction().x();
    auto y = r.origin().y() + t * r.direction().y();
    if (x < x0 || x > x1 || y < y0 || y > y1) return false;

    rec.u = (x - x0) / (x1 - x0);
    rec.v = (y - y0) / (y1 - y0);
    rec.t = t;
    auto outwardNormal = Vec3(0, 0, 1);
    rec.setFaceNormal(r, outwardNormal);
    rec.matPtr = matPtr;
    rec.p = r.at(t);
    return true;
}

class XZ_Rectangle : public Corporeal {
    public:
        XZ_Rectangle() {}
        XZ_Rectangle(double _x0, double _x1, double _z0, double _z1, double _y, shared_ptr<Material> mat)
            : x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_y), matPtr(mat) {};

        virtual bool hit (const Ray& r, double tMin, double tMax, HitRecord& rec) const override;

        virtual bool boundingBox(double time0, double time1, AABB& outputBox) const override {
            // The bounding box must have a non-zero thickness in all dimensions
            outputBox = AABB(Point3(x0, z0, k-0.0001), Point3(x1, z1, k+0.0001));
            return true;
        }

    public:
        shared_ptr<Material> matPtr;
        double x0, x1, z0, z1, k;
};

bool XZ_Rectangle::hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const {
    auto t = (k - r.origin().y()) / r.direction().y();
    if (tMin > t || t > tMax) return false;

    auto x = r.origin().x() + t * r.direction().x();
    auto z = r.origin().z() + t * r.direction().z();
    if (x < x0 || x > x1 || z < z0 || z > z1) return false;

    rec.u = (x - x0) / (x1 - x0);
    rec.v = (z - z0) / (z1 - z0);
    rec.t = t;
    auto outwardNormal = Vec3(0, 0, 1);
    rec.setFaceNormal(r, outwardNormal);
    rec.matPtr = matPtr;
    rec.p = r.at(t);
    return true;
}

class YZ_Rectangle : public Corporeal {
    public:
        YZ_Rectangle() {}
        YZ_Rectangle(double _y0, double _y1, double _z0, double _z1, double _x, shared_ptr<Material> mat)
            : y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_x), matPtr(mat) {};

        virtual bool hit (const Ray& r, double tMin, double tMax, HitRecord& rec) const override;

        virtual bool boundingBox(double time0, double time1, AABB& outputBox) const override {
            // The bounding box must have a non-zero thickness in all dimensions
            outputBox = AABB(Point3(y0, z0, k-0.0001), Point3(y1, z1, k+0.0001));
            return true;
        }

    public:
        shared_ptr<Material> matPtr;
        double y0, y1, z0, z1, k;
};

bool YZ_Rectangle::hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const {
    auto t = (k - r.origin().x()) / r.direction().x();
    if (tMin > t || t > tMax) return false;

    auto y = r.origin().y() + t * r.direction().y();
    auto z = r.origin().z() + t * r.direction().z();
    if (y < y0 || y > y1 || z < z0 || z > z1) return false;

    rec.u = (y - y0) / (y1 - y0);
    rec.v = (z - z0) / (z1 - z0);
    rec.t = t;
    auto outwardNormal = Vec3(0, 0, 1);
    rec.setFaceNormal(r, outwardNormal);
    rec.matPtr = matPtr;
    rec.p = r.at(t);
    return true;
}

#endif