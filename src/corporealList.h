#ifndef CORPOREAL_LIST_H
#define CORPOREAL_LIST_H

#include "corporeal.h"

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

#endif