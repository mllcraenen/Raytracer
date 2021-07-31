#ifndef MATERIAL_H
#define MATERIAL_H

#include "tracer.h"
#include "texture.h"

struct HitRecord;

class Material {
    public:
        virtual bool scatter(
            const Ray& rIn, const HitRecord& rec, Color& attenuation, Ray& scattered
        ) const = 0;
};

class Lambertian : public Material {
    public:
        Lambertian(const Color& a) : albedo(make_shared<SolidColor>(a)) {}
        Lambertian(shared_ptr<Texture> a) : albedo(a) {}

        virtual bool scatter(
            const Ray& rayIn, const HitRecord& rec, Color& attenuation, Ray& scattered
        ) const override {
            auto scatterDirection = rec.normal + randomUnitVector();

            // If the random unit vector is almost exactly opposite the normal the scatter will be almost 0. In that case override it to the normal.
            if (scatterDirection.nearZero()) {
                scatterDirection = rec.normal;
            }

            scattered = Ray(rec.p, scatterDirection);
            attenuation = albedo->value(rec.u, rec.v, rec.p);
            return true;
        }
    public:
        shared_ptr<Texture> albedo;
};

class HemisphereDiffuse : public Material {
    public:
        HemisphereDiffuse(const Color& a) : albedo(a) {}

        virtual bool scatter(
            const Ray& rayIn, const HitRecord& rec, Color& attenuation, Ray& scattered
        ) const override {
            auto scatterDirection = rec.p + randomInHemisphere(rec.normal);
            if (scatterDirection.nearZero()) scatterDirection = rec.normal;
            scattered = Ray(rec.p, scatterDirection);
            attenuation = albedo;
            return true;
        }
    public:
        Color albedo;
};

class Metal : public Material {
    public:
        Metal(const Color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

        virtual bool scatter(
            const Ray& r, const HitRecord& rec, Color& attenuation, Ray& scattered
        ) const override {
            // Metal directly reflects rays. If there is some fuzziness factor < 1, the reflection is disturbed a bit.
            Vec3 reflected = reflect(unitVector(r.direction()), rec.normal);
            scattered = Ray(rec.p, reflected + fuzz * randomVectorInUnitSphere());
            attenuation = albedo;

            return (dot(scattered.direction(), rec.normal) > 0);
        }

    public:
        Color albedo;
        double fuzz;
};

class Dielectric : public Material {
    public:
        Dielectric(double refractionIndex) : ir(refractionIndex) {}

        virtual bool scatter(
            const Ray& rIn, const HitRecord& rec, Color& attenuation, Ray& scattered
        ) const override {
            attenuation = Color(1.0, 1.0, 1.0);
            double refractionRatio = rec.frontFace ? (1.0 / ir) : ir;

            Vec3 unitDirection = unitVector(rIn.direction());
            
            // Handle total internal reflection, a situation where the incidence angle > critical angle, making it that light cannot escape slower medium and thus cannot refract.
            double cosTheta = fmin( dot( -unitDirection, rec.normal), 1.0);
            double sinTheta = sqrt(1.0 - cosTheta * cosTheta);
            bool cannotRefract = refractionRatio * sinTheta > 1.0;
            Vec3 direction;

            if (cannotRefract || reflectance(cosTheta, refractionRatio) > randomDouble()) {
                direction = reflect(unitDirection, rec.normal);
            } else {
                direction = refract(unitDirection, rec.normal, refractionRatio);
            } 


            scattered = Ray(rec.p, direction);
            return true;
        }

    public:
        double ir;
    private:
        //Christophe Schlick's polynomial reflectance approximation for reflectivity variance over angles
        static double reflectance(double cosine, double referenceIndex) {
            auto r0 = (1 - referenceIndex) / (1 + referenceIndex);
            return r0 * r0 + (1 - r0) * pow((1-cosine), 5);
        }

};

#endif