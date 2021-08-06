#ifndef SCENES_H
#define SCENES_H

#include "../tracer.h"

#include "../color.h"
#include "../corporealList.h"
#include "../material.h"
#include "../sphere.h"
#include "../triangle.h"
#include "../aarect.h"
#include "../bvh.h"


CorporealList randomScene();
CorporealList devScene();
CorporealList textureDemoScene();
CorporealList lightTestScene();
CorporealList cornellBox();

CorporealList worldGen(Scenes scene) {
    CorporealList world;
    switch(scene) {
        default:
        case DEV: {
            world = devScene();
            background = Color(0.70, 0.80, 1.00);
            break;
        }
        case BENCHMARK: {
            world = randomScene();
            background = Color(0.70, 0.80, 1.00);
            break;
        }
        case TEXTURE: {
            world = textureDemoScene();
            background = Color(0.70, 0.80, 1.00);
            break;
        }
        case LIGHT: {
            world = lightTestScene();
            samplesPerPixel = 400;
            background = Color(0,0,0);
            cameraOrigin = Point3(26,3,6);
            cameraLookAt = Point3(0,2,0);
            hFOV = 34.0;
            cam = Camera(cameraOrigin, cameraLookAt, cameraUp, hFOV, aspectRatio, aperture, distToFocus);
            break;
        }
        case CORNELL: {
            world = cornellBox();
            // aspectRatio = 1.0;
            // imageWidth = 600;
            samplesPerPixel = 300;
            background = Color(0,0,0);
            cameraOrigin = Point3(278, 278, -800);
            cameraLookAt = Point3(278, 278, 0);
            hFOV = 50.0;
            cam = Camera(cameraOrigin, cameraLookAt, cameraUp, hFOV, aspectRatio, aperture, distToFocus);
            break;
        }
    }
    return world;
}

CorporealList devScene() {
    CorporealList objects;
    auto checker = make_shared<Checker>(Color(0.2, 0.3, 0.9), Color(0.9, 0.9, 0.9));
    auto checker2 = make_shared<Checker>(Color(0.6, 0.6, 0.1), Color(0.9, 0.9, 0.9)); 
    auto materialGround = make_shared<Metal>(Color(0.4, 0.9, 0.4), .1);
    // auto materialGround = make_shared<Lambertian>(checker2);
    auto materialLeft = make_shared<Dielectric>(1.5);
    auto materialCenter = make_shared<Lambertian>(checker);
    auto materialRight = make_shared<Lambertian>(Color(0.2, 0.4, 0.1));
    auto materialFiretruckFuckingRed = make_shared<Lambertian>(Color(1.0, 0.05, 0.05));
    auto materialPerlin = make_shared<Lambertian>(make_shared<NoiseTexture>());

    objects.add(make_shared<Sphere>(Point3( 0.0, -1000.5, -1.0), 1000.0, materialGround));
    objects.add(make_shared<Sphere>(Point3( 0.0,    0.0, -1.0),   0.5, materialCenter));
    objects.add(make_shared<Sphere>(Point3(-1.0,    0.0, -1.0),   0.5, materialLeft));
    objects.add(make_shared<Sphere>(Point3( 1.0,    0.0, -1.0),   0.5, materialRight));
    objects.add(make_shared<Sphere>(Point3( 1.0,    1.5, -3.0),   2, materialPerlin));
    objects.add(make_shared<Triangle>(Point3( -0.5, 2.0, 0.0), Point3(0.0, 1.0, 0.0), Point3( 0.5, 2.0, 0.0), materialFiretruckFuckingRed));

    return CorporealList(make_shared<BvhNode>(objects, 0.0, 1.0));
}

CorporealList textureDemoScene() {
    CorporealList objects;
    auto earthSurface = make_shared<Lambertian>(make_shared<ImageTexture>("src/txt/earthmap.jpg"));
    // objects.add(make_shared<Sphere>(Point3(0,-1000,0), 1000, earthSurface));
    objects.add(make_shared<Sphere>(Point3(0,2,0), 2, earthSurface));

    return CorporealList(make_shared<BvhNode>(objects, 0.0, 1.0));
}

CorporealList lightTestScene() {
    CorporealList objects;
    auto earth = make_shared<ImageTexture>("src/txt/earthmap.jpg");

    auto pertext = make_shared<TurbulenceTexture>(randomInt(2,4));
    objects.add(make_shared<Sphere>(Point3(0,-1000,0), 1000, make_shared<Lambertian>(pertext)));
    objects.add(make_shared<Sphere>(Point3(0,2,0), 2, make_shared<Lambertian>(pertext)));

    auto difflight = make_shared<DiffuseLight>(Color(4,4,4));
    objects.add(make_shared<XY_Rectangle>(3, 5, 1, 3, -2, difflight));
    return objects;
}

CorporealList randomScene() {
    CorporealList objects;

    //Pleasant shade of checker ground.
    auto groundMat = make_shared<Lambertian>(make_shared<Checker>(Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9)));
    objects.add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, groundMat));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto chooseMat = randomDouble();
            Point3 center(a + 0.9 * randomDouble(), 0.2, b + 0.9 * randomDouble());

            if ((center - Point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<Material> sphereMat;

                if (chooseMat < 0.25) {
                    // Diffuse Lambertian
                    auto albedo = Color::random() * Color::random();
                    sphereMat = make_shared<Lambertian>(albedo);
                    objects.add(make_shared<Sphere>(center, 0.2, sphereMat));
                } else if (chooseMat < 0.5) {
                    // Diffuse Marbled
                    sphereMat = make_shared<Lambertian>(make_shared<TurbulenceTexture>(randomInt(10,15)));
                    objects.add(make_shared<Sphere>(center, 0.2, sphereMat));
                } else if (chooseMat < 0.8) {
                    // Metal
                    auto albedo = Color::random(0.5, 1);
                    auto fuzz = randomDouble(0, 0.1);
                    sphereMat = make_shared<Metal>(albedo, fuzz);
                    objects.add(make_shared<Sphere>(center, 0.2, sphereMat));
                } else {
                    // Glass
                    sphereMat = make_shared<Dielectric>(1.5);
                    objects.add(make_shared<Sphere>(center, 0.2, sphereMat));
                    if (randomDouble() < 0.5) objects.add(make_shared<Sphere>(center, -0.15, sphereMat));
                }
            }
        }
    }

    auto mat1 = make_shared<Dielectric>(1.5);
    objects.add(make_shared<Sphere>(Point3(0, 1, 0), 1.0, mat1));

    auto mat2 = make_shared<Lambertian>(Color(0.4, 0.0, 0.5));
    objects.add(make_shared<Sphere>(Point3(-4, 1, 0), 1.0, mat2));

    auto mat3 = make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
    objects.add(make_shared<Sphere>(Point3(4, 1, 0), 1.0, mat3));

    return CorporealList(make_shared<BvhNode>(objects, 0.0, 1.0));
}

CorporealList cornellBox() {
    CorporealList objects;
    
    auto red   = make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = make_shared<Lambertian>(Color(.73, .73, .73));
    auto green = make_shared<Lambertian>(Color(.12, .45, .15));
    auto light = make_shared<DiffuseLight>(Color(15, 15, 15));

    objects.add(make_shared<YZ_Rectangle>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<YZ_Rectangle>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<XZ_Rectangle>(213, 343, 227, 332, 554, light));
    objects.add(make_shared<XZ_Rectangle>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<XZ_Rectangle>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<XY_Rectangle>(0, 555, 0, 555, 555, white));

    return objects;
}
#endif
