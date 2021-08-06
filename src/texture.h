#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>
#include "lib/stb_image_helper.h"

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

class ImageTexture : public Texture {
    public:
        // RGB color makes 3 bytes of data per texel.
        const static int bytesPerTexel = 3;

        ImageTexture() : data(nullptr), width(0), bytesPerScanline(0) {}
        ImageTexture(const char* filename) {
            auto componentsPerTexel = bytesPerTexel;
            data = stbi_load(filename, &width, &height, &componentsPerTexel, componentsPerTexel);

            if (!data) {
                std::cerr << "ERROR: Could not load texture file '" << filename << "'.\n";
                width = height = 0;
            }

            bytesPerScanline = bytesPerTexel * width;
        }

        ~ImageTexture() {
            delete data;
        }

        virtual Color value(double u, double v, const Vec3& p) const override {
            // No texture data returns magenta.
            if (data == nullptr) return Color(1,0,1);

            // Clamp input coordinates to [0,1] x [1,0]
            u = clamp(u, 0.0, 1.0);
            v = 1.0 - clamp(v, 0.0, 1.0); // Flip v because image is bottomleft indexed but texels are topleft indexed. (?)

            auto i = static_cast<int>(u * width);
            auto j = static_cast<int>(v * height);

            // If u or v was 1 we get out of bounds if we try to grab the texel, limit it to axis - 1.
            if (i >= width)  i = width-1;
            if (j >= height) j = height-1;

            const auto colorScale = 1.0 / 255.0;
            auto texel = data + j * bytesPerScanline + i * bytesPerTexel;

            return Color(colorScale * texel[0], colorScale * texel[1], colorScale * texel[2]);
        }

    private:
        unsigned char *data;
        int width, height;
        int bytesPerScanline;
};

#endif
