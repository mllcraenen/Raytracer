#ifndef TEXTURE_H
#define TEXTURE_H

#include "tracer.h"
#include "perlin.h"

class Texture {
    public:
        virtual Color value(double u, double v, const Point3& p) const = 0;
};

class SolidColor : public Texture {
    public:
        SolidColor() {}
        SolidColor(Color c) : colorValue(c) {}
        SolidColor(double r, double g, double b) : SolidColor(Color(r,g,b)) {}

        virtual Color value(double u, double v, const Point3& p) const override {
            return colorValue;
        }

    private:
        Color colorValue;
};

class Checker : public Texture {
    public:
        Checker() {}
        Checker(shared_ptr<Texture> _even, shared_ptr<Texture> _odd) : even(_even), odd(_odd) {}
        Checker(Color c1, Color c2) : even(make_shared<SolidColor>(c1)), odd(make_shared<SolidColor>(c2)) {}

        virtual Color value(double u, double v, const Point3& p) const override {
            auto sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
            if (sines < 0) return odd->value(u, v, p);
            else return even->value(u, v, p);
        }

    public:
        shared_ptr<Texture> odd;
        shared_ptr<Texture> even;
};

class NoiseTexture : public Texture {
    public:
        NoiseTexture() {}
        NoiseTexture(double sc) : scale(sc) {}

        virtual Color value(double u, double v, const Point3& p) const override {
            return Color(1,1,1) * 0.5 * (1.0 + noise.noise(scale * p));
        }

    public:
        Perlin noise;
        double scale;
};

class TurbulenceTexture : public Texture {
    public:
        TurbulenceTexture() {}
        TurbulenceTexture(double sc) : scale(sc) {}

        virtual Color value(double u, double v, const Point3& p) const override {
            return Color(1,1,1) * 0.5 * (1 + sin(scale * p.z() + 10 * noise.turbulence(p)));
        }

    public:
        Perlin noise;
        double scale;
};

#endif
