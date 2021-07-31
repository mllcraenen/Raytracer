#include "tracer.h"

#include "color.h"
#include "corporealList.h"
#include "material.h"
#include "sphere.h"
#include "triangle.h"
#include "bvh.h"

#include <iostream>
#include <chrono>

#define SCENE 1

void progressOut(int i, int imageHeight);
Color rayColor(const Ray& r, const Corporeal& world, int depth);
double hitSphere(const Point3& center, double radius, const Ray& r);
CorporealList randomScene();
CorporealList devScene();


int main() {
    //DEBUGTIMER
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    
    // Define World with objects
    CorporealList world;
    switch(SCENE) {
        case 0: {
            world = devScene();
            break;
        }
        case 1: {
            world = randomScene();
            break;
        }
        default: return 1;
    }

    // Define output
    freopen("out.ppm", "w", stdout);

    // Render PPM
    std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";
    
    // (0,0) is bottom left. 
    // Thus working from top to bottom left to right has us counting down for rows and counting up columns.
    for (int i = imageHeight - 1; i >= 0; i--) {
        // progressOut(i, imageHeight);


        for (int j = 0; j < imageWidth; j++) {
            Color pixelColor(0,0,0);
            
            for(int sample = 0; sample < samplesPerPixel; sample++) {
                auto u = (j + randomDouble()) / (imageWidth - 1);
                auto v = (i + randomDouble()) / (imageHeight - 1);              

                // Birthe a Ray and send it out into the wild world
                Ray r = cam.getRay(u, v);
                pixelColor += rayColor(r, world, maxBounceDepth);                  
            }


            // Write the Color to `cout`
            writeColor(std::cout, pixelColor, samplesPerPixel);
        }
    }


    //DEBUGTIMER
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cerr << "\nRender complete.\n";
    std::cerr << "Elapsed time = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " [ms]" << std::endl;
    return 0;
}


Color rayColor(const Ray& r, const Corporeal& world, int depth) {
    HitRecord rec;

    // If the ray bounce limit is exceeded do not continue.
    if (depth <= 0) {
        return Color(0,0,0);
    }
    
    // If the ray hits a physical ("Corporeal") object, diffuse. tMin is 0.001 to solve floating point bugs around 0.
    if (world.hit(r, 0.001, infinity, rec)) {
        Ray scattered;
        Color attenuation;
        
        // Scatter the ray in accordance with the Corporeal's Material.
        if (rec.matPtr->scatter(r, rec, attenuation, scattered)) {
            return attenuation * rayColor(scattered, world, depth - 1);
        }

        return Color(0,0,0);
    }

    Vec3 unitDirection = unitVector(r.direction());
    auto t = 0.5 * (unitDirection.y() + 1.0);
    
    // Set a BG gradient
    Color Color1BG = Color(1.0, 1.0, 1.0);
    Color Color2BG = Color(0.5, 0.7, 1.0);
    return (1.0 - t) * Color1BG + t * Color2BG;
}


double hitSphere(const Point3& center, double radius, const Ray& r) {
    //Vector from origin of the Ray to the center of the sphere
    Vec3 oc = r.origin() - center;

    //The quadratic formula here is rewritten to
    //(-b +- sqrt(b^2 - ac)) / a because apparently that's more efficient? idk man.
    
    // a = r . r = |r| ^ 2
    auto a = r.direction().lengthSquared();
    auto halfB = dot(oc, r.direction());
    auto c = oc.lengthSquared() - radius * radius;

    auto discriminant = halfB * halfB - a*c;
    if(discriminant < 0) {
        return -1.0;
    } else {
        //After solving the quadratic formula for the sphere return the smallest solution.
        return (-halfB - sqrt(discriminant)) / a;
    }
}

CorporealList devScene() {
    CorporealList objects;
    auto materialGround = make_shared<Metal>(Color(0.6, 0.6, 0.0), .2);
    auto materialLeft = make_shared<Dielectric>(1.5);
    auto materialCenter = make_shared<HemisphereDiffuse>(Color(0.9, 0.0, 0.9));
    auto materialRight = make_shared<Lambertian>(Color(0.4, 0.1, 0.1));
    auto materialFiretruckFuckingRed = make_shared<Lambertian>(Color(1.0, 0.05, 0.05));

    objects.add(make_shared<Sphere>(Point3( 0.0, -200.5, -1.0), 200.0, materialGround));
    objects.add(make_shared<Sphere>(Point3( 0.0,    0.0, -1.0),   0.5, materialCenter));
    objects.add(make_shared<Sphere>(Point3(-1.0,    0.0, -1.0),   0.5, materialLeft));
    objects.add(make_shared<Sphere>(Point3( 1.0,    0.0, -1.0),   0.5, materialRight));
    objects.add(make_shared<Triangle>(Point3( 2.0, 2.0, 0.0), Point3(0.0, 0.0, 0.0), Point3( 3.0, 2.0, -1.0), materialFiretruckFuckingRed));

    // return objects;
    return CorporealList(make_shared<BvhNode>(objects, 0.0, 1.0));
}

CorporealList randomScene() {
    CorporealList objects;

    //Pleasant shade of grey ground.
    auto groundMat = make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
    objects.add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, groundMat));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto chooseMat = randomDouble();
            Point3 center(a + 0.9 * randomDouble(), 0.2, b + 0.9 * randomDouble());

            if ((center - Point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<Material> sphereMat;

                if (chooseMat < 0.3) {
                    // Diffuse Lambertian
                    auto albedo = Color::random() * Color::random();
                    sphereMat = make_shared<Lambertian>(albedo);
                    objects.add(make_shared<Sphere>(center, 0.2, sphereMat));
                } else if (chooseMat < 0.5) {
                    // Diffuse Hemisphere
                    auto albedo = Color::random() * Color::random();
                    sphereMat = make_shared<HemisphereDiffuse>(albedo);
                    objects.add(make_shared<Sphere>(center, 0.2, sphereMat));
                } else if (chooseMat < 0.8) {
                    // Metal
                    auto albedo = Color::random(0.5, 1);
                    auto fuzz = randomDouble(0, 0.5);
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

// Displays pretty progress bar in terminal based on current row and rows still to render
void progressOut(int i, int imageHeight) {
    int barWidth = 100;
    std::cerr << "[";
    int pos = barWidth * (((double)imageHeight - (double)i) / (double)imageHeight);
    // std::cout<< pos;
    for (int i = 0; i < barWidth; i++) {
        if (i <= pos) std::cerr << "#";
        else std::cerr << "-";
    }
    std::cerr << "] " << int(pos) << " %\r";
    std::cerr.flush();
}
