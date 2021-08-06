#ifndef BOX_H
#define BOX_H

#include "tracer.h"
#include "aarect.h"
#include "corporealList.h"

class Box : public Corporeal {
    public:
        Box() {}
        Box(const Point3& p0, const Point3& p1, shared_ptr<Material> matPtr);

        virtual bool hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override;
        virtual bool boundingBox(double time0, double time1, AABB& outputBox) const override {
            outputBox = AABB(boxMin, boxMax);
            return true;
        }

    public:
        Point3 boxMin;
        Point3 boxMax;
        CorporealList sides;
};

Box::Box(const Point3& p0, const Point3& p1, shared_ptr<Material> matPtr) {
    boxMin = p0;
    boxMax = p1;

    sides.add(make_shared<XY_Rectangle>(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), matPtr));
    sides.add(make_shared<XY_Rectangle>(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), matPtr));

    sides.add(make_shared<XZ_Rectangle>(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), matPtr));
    sides.add(make_shared<XZ_Rectangle>(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), matPtr));

    sides.add(make_shared<YZ_Rectangle>(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), matPtr));
    sides.add(make_shared<YZ_Rectangle>(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), matPtr));
}

bool Box::hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const {
    return sides.hit(r, tMin, tMax, rec);
}

#endif