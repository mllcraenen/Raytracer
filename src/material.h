#ifndef MATERIAL_H
#define MATERIAL_H

#include "tracer.h"

struct HitRecord;

class Material {
    public:
        virtual bool scatter(
            const Ray& rIn, const HitRecord& rec, Color& attenuation, Ray& scattered
        ) const = 0;
};

class Lambertian : public Material {
    public:
        Lambertian(const Color& a) : albedo(a) {}

        virtual bool scatter(
            const Ray& rayIn, const HitRecord& rec, Color& attenuation, Ray& scattered
        ) const override {
            auto scatterDirection = rec.normal;
        }
};

#endif