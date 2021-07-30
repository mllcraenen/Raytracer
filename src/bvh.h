#ifndef BVH_H
#define BVH_H

#define FRAME_THICKNESS 0.05

#include "tracer.h"

#include <algorithm>
#include "aabb.h"
#include "corporeal.h"
#include "corporealList.h"


class BvhNode : public Corporeal {
    public:
        BvhNode();

        BvhNode(const CorporealList& list, double time0, double time1) 
            : BvhNode(list.objects, 0, list.objects.size(), time0, time1) {}

        BvhNode(
            const std::vector<shared_ptr<Corporeal>>& sourceObjects, 
            size_t start, size_t end, double time0, double time1);

        virtual bool hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override;
        virtual bool boundingBox(double time0, double time1, AABB& outputBox) const override;

    public:
        shared_ptr<Corporeal> left;
        shared_ptr<Corporeal> right;
        AABB box;
        AABB innerBox;
};

inline bool boxCompare(const shared_ptr<Corporeal> a, const shared_ptr<Corporeal> b, int axis) {
    AABB boxA;
    AABB boxB;

    if (!a->boundingBox(0, 0, boxA) || !b->boundingBox(0, 0, boxB))
        std::cerr << "No bounding box in bvh_node constructor.\n";

    return boxA.min().e[axis] < boxB.min().e[axis];
}

bool boxXCompare (const shared_ptr<Corporeal> a, const shared_ptr<Corporeal> b) {
    return boxCompare(a, b, 0);
}

bool boxYCompare (const shared_ptr<Corporeal> a, const shared_ptr<Corporeal> b) {
    return boxCompare(a, b, 1);
}

bool boxZCompare (const shared_ptr<Corporeal> a, const shared_ptr<Corporeal> b) {
    return boxCompare(a, b, 2);
}

// Optimize this bvh population
BvhNode::BvhNode(
    const std::vector<shared_ptr<Corporeal>>& sourceObjects, 
    size_t start, size_t end, double time0, double time1
    ) {
    auto objects = sourceObjects; 

    int axis = randomInt(0,2);
    auto comparator = (axis == 0) ? boxXCompare
                    : (axis == 1) ? boxYCompare
                    :               boxZCompare;
    size_t objectSpan = end - start;

    if (objectSpan == 1) {
        left = right = objects[start];
    } else if (objectSpan == 2) {
        if (comparator(objects[start], objects[start+1])) {
            left = objects[start];
            right = objects[start + 1];
        } else {
            left = objects[start + 1];
            right = objects[start];
        }
    } else {
        // Sort the list of objects in this node
        std::sort(objects.begin() + start, objects.begin() + end, comparator);
        // Find the middle of the list
        auto middle = start + objectSpan / 2;
        // Put each half of the list in a new node to sort recursively.
        left = make_shared<BvhNode>(objects, start, middle, time0, time1);
        right = make_shared<BvhNode>(objects, middle, end, time0, time1);
    }

    AABB boxLeft, boxRight;

    if (!left->boundingBox(time0, time1, boxLeft) || !right->boundingBox(time0, time1, boxRight)) {
        std::cerr << "Nou bounding box in BvhNode constructor. \n";
    }

    box = surroundingBox(boxLeft, boxRight);
    innerBox = frameBox(box, FRAME_THICKNESS);
};

bool BvhNode::boundingBox(double time0, double time1, AABB& outputBox) const {
    outputBox = box;
    return true;
}

/**
 * Check whether the box for this node is hit, and if so check the children for hits recursively.
 */
bool BvhNode::hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const {
    if (!box.hit(r, tMin, tMax)) return false;
    #ifdef WIREFRAME_MODE
    else if (!innerBox.hit(r, tMin, tMax)) {
        // The ray hits the box and specifically its edge, which we want to render
        box.hitFrame(r, tMin, tMax, rec);
        return true;
    } 
    #endif
    else {
        // The ray hits the box but does not hit the edge
        bool hitLeft = left->hit(r, tMin, tMax, rec);
        bool hitRight = right->hit(r, tMin, tMax, rec);

        return hitLeft || hitRight;
    }

}
#endif