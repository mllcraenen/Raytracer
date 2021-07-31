#ifndef CORPOREAL_LIST_H
#define CORPOREAL_LIST_H

#include "corporeal.h"
#include "aabb.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

class CorporealList : public Corporeal {
    public:
        CorporealList() {}
        CorporealList(shared_ptr<Corporeal> object) { add(object); }

        void clear() { objects.clear(); }
        void add(shared_ptr<Corporeal> object) { objects.push_back(object); }

        virtual bool hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override;
        virtual bool boundingBox(double time0, double time1, AABB& outputBox) const override;
    public:
        std::vector<shared_ptr<Corporeal>> objects;
};

// Checks whether a ray hits any physical object.
bool CorporealList::hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const {
    HitRecord tmp_rec;
    bool hitAnything = false;
    auto closest = tMax;

    for (const auto& object : objects) {
        if (object->hit(r, tMin, closest, tmp_rec)) {
            hitAnything = true;
            closest = tmp_rec.t;
            rec = tmp_rec;
        }
    }
    
    return hitAnything;
}

bool CorporealList::boundingBox(double time0, double time1, AABB& outputBox) const {
    if (objects.empty()) return false;

    AABB tmpBox;
    bool isFirstBox = true;

    // For all objects in the list
    for (const auto& object : objects) {
        // If the object has a bounding box it is put in tmpBox and we continue, else return false.
        bool objectHasBox = object->boundingBox(time0, time1, tmpBox);
        if (!objectHasBox) return false;
        // If this is the first box of a pair we make outputBox the first box. 
        if (isFirstBox) outputBox = tmpBox;
        // Else we make the outputbox the surrounding box of the previous box and this one.
        else outputBox = surroundingBox(outputBox, tmpBox);
        isFirstBox = false;
    }

    return true;
}

#endif