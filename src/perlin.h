#ifndef PERLIN_H
#define PERLIN_H

#include "tracer.h"

class Perlin {
    public:
        Perlin() {
            ranvec = new Vec3[pointCount];
            for (int i = 0; i < pointCount; i++) {
                ranvec[i] = unitVector(Vec3::random(-1,1));
            }

            permX = perlinGeneratePerm();
            permY = perlinGeneratePerm();
            permZ = perlinGeneratePerm();
        }

        ~Perlin() {
            delete[] ranvec;
            delete[] permZ;
            delete[] permX;
            delete[] permY;
        }

        double noise(const Point3& p) const {
            auto u = p.x() - floor(p.x());
            auto v = p.y() - floor(p.y());
            auto w = p.z() - floor(p.z());
            auto i = static_cast<int>(floor(p.x()));
            auto j = static_cast<int>(floor(p.y()));
            auto k = static_cast<int>(floor(p.z()));
            Vec3 c[2][2][2];

            for (int di = 0; di < 2; di++) {
                for (int dj = 0; dj < 2; dj++) {
                    for (int dk = 0; dk < 2; dk++) {
                        c[di][dj][dk] = ranvec[
                            permX[(i + di) & 255] ^
                            permY[(j + dj) & 255] ^
                            permZ[(k + dk) & 255] 
                        ];
                    }
                }
            }
            return trilinInterpolate(c, u, v, w);
        }

        double turbulence(const Point3& p, int depth = 7) const {
            auto accumulator = 0.0;
            auto tempPoint = p;
            auto weight = 1.0;

            for (int i = 0; i < depth; i++) {
                accumulator += weight * noise(tempPoint);
                weight *= 0.5;
                tempPoint *= 2;
            }
            
            return fabs(accumulator);
        }

    private:
        static const int pointCount = 256;
        Vec3* ranvec;
        int* permX;
        int* permY;
        int* permZ;

        static int* perlinGeneratePerm() {
            auto p = new int[pointCount];

            for (int i = 0; i < Perlin::pointCount; i++) p[i] = i;

            permute(p, pointCount);
            return p;
        }

        static void permute(int* p, int n) {
            for (int i = n - 1; i > 0; i--) {
                int target = randomInt(0, i);
                int tmp = p[i];
                p[i] = p[target];
                p[target] = tmp;
            }
        }

        static double trilinInterpolate(Vec3 c[2][2][2], double u, double v, double w) {
            auto accumulator = 0.0;
            auto uu = u * u * (3-2*u);
            auto vv = v * v * (3-2*v);
            auto ww = w * w * (3-2*w);

            for (int i = 0; i < 2; i++) {
                for (int j = 0; j < 2; j++) {
                    for (int k = 0; k < 2; k++) {
                        Vec3 weightVector(u-i, v-j, w-k);
                        accumulator +=  (i*uu + (1-i)*(1-uu))
                                    *   (j*vv + (1-j)*(1-vv))
                                    *   (k*ww + (1-k)*(1-ww))
                                    *   dot(c[i][j][k], weightVector);
                    }
                }
            }
            return accumulator;
        }
};

#endif
