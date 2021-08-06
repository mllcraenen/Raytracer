#include "tracer.h"

#include "color.h"
#include "corporealList.h"
#include "material.h"
#include "sphere.h"
#include "triangle.h"
#include "bvh.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include "unistd.h"

#define SCENE 1

void progressOut(int i, int imageHeight);
void allocateThread(int pixelsToAllocate, int imageHeight, int imageWidth, int& i, int& j, CorporealList& world);
void tracePixel(int iStart, int iEnd, int jStart, int jEnd, int imageHeight, int imageWidth, CorporealList world);
Color rayColor(const Ray& r, const Corporeal& world, int depth);
double hitSphere(const Point3& center, double radius, const Ray& r);
CorporealList randomScene();
CorporealList devScene();
CorporealList textureDemoScene();

int maxThreads = std::thread::hardware_concurrency();
Color imageBuffer[imageHeight][imageWidth];
std::vector<std::thread> threads;


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
        case 2: {
            world = textureDemoScene();
            break;
        }
        default: return 1;
    }

    // Define output
    freopen("out.ppm", "w", stdout);

    // Init PPM file
    std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";
    
    // (0,0) is bottom left. 
    // Thus working from top to bottom left to right has us counting down for rows and counting up columns.
    int pixels = imageWidth * imageHeight;
    int pixelsPerThread = (int)(pixels / (maxThreads - 1));
    // std::cerr << "Pixels per thread: " << pixelsPerThread << "\n";
    int i = imageHeight - 1;
    int j = 0;
    while (i > 0) {
        allocateThread(pixelsPerThread, imageHeight, imageWidth, i, j, world);
    }

    for (auto& th : threads) th.join();

    for (int i = imageHeight - 1; i >= 0; i--) {
        for (int j = 0; j < imageWidth; j++) {
            writeColor(std::cout, imageBuffer[i][j], samplesPerPixel);
        }
    }


    // for (int i = imageHeight - 1; i >= 0; i--) {
    //     progressOut(i, imageHeight);
    //     for (int j = 0; j < imageWidth; j++) {
    //         threads.emplace_back(std::thread(tracePixel, i, j, j, world));
    //     }
    // }


    //DEBUGTIMER
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cerr << "\nRender complete.\n";
    std::cerr << "Elapsed time = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " [ms]" << std::endl;
    return 0;
}

void allocateThread(int pixelsToAllocate, int imageHeight, int imageWidth, int& i, int& j, CorporealList& world) {
    int threadStartI = i;

    //// Determine for the thread the i and j to end on.
    
    // First subtract pixel's to the end of the row. These don't count for the full row count because they have to be filled first.
    //
    // row ||--------|-----------------||
    //     0        j=4                99     -> subtract 95 from pixels to allocate
    pixelsToAllocate -= (imageWidth - j);

    // Now we will determine how many full rows we can render with the remaining pixels to allocate
    int fullRowsToRender = (int)pixelsToAllocate / imageWidth;
    // Subtract the amount of full rows from the pixels to allocate.
    pixelsToAllocate -= (fullRowsToRender * imageWidth);
    // And from that determine the j to end on. Subtract 1 because j is 0 indexed
    // int endJ = pixelsToAllocate % imageWidth;
    int endJ = pixelsToAllocate;
    endJ--;
    int threadEndI = (i - fullRowsToRender) >= 0 ? i - fullRowsToRender : 0;

    // std::cerr << "Start row: " << threadStartI << " / End row: " << threadEndI << " || setting another " << endJ << " pixels.\n";

    threads.emplace_back(std::thread(tracePixel, threadStartI, threadEndI, j, endJ, imageHeight, imageWidth, world));
    // Set i and j to the value the next thread can start from.
    i = threadEndI;
    j = endJ;
}

void tracePixel(int iStart, int iEnd, int jStart, int jEnd, int imageHeight, int imageWidth, CorporealList world) {
    int i = iStart;
    int j = jStart;
    
    // While we still have rows to render, or we are on the last row but not the last pixel to render
    while (i > iEnd || (i == iEnd && j < jEnd)) {
        // Trace pixels until the end of the row or if we are on the last row until the designated stop spot.
        while (j < imageWidth || (i == iEnd && j < jEnd)) {
            // Render a pixel
            Color pixelColor(0,0,0);
            for (int sample = 0; sample < samplesPerPixel; sample++) {
                auto u = (j + randomDouble()) / (imageWidth - 1);
                auto v = (i + randomDouble()) / (imageHeight - 1);              

                // Birthe a Ray and send it out into the wild world
                Ray r = cam.getRay(u, v);
                pixelColor += rayColor(r, world, maxBounceDepth);              
            }
            // Write the Color to `cout`
            imageBuffer[i][j] = pixelColor;
            j++;
        }
        // Decrement the row and reset the j pointer
        i--;
        j = 0;
    }
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
    objects.add(make_shared<Sphere>(Point3(0,-1000,0), 1000, earthSurface));
    objects.add(make_shared<Sphere>(Point3(0,2,0), 2, earthSurface));

    return CorporealList(make_shared<BvhNode>(objects, 0.0, 1.0));
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

// Displays pretty progress bar in terminal based on current row and rows still to render
void progressOut(int current, int total) {
    int barWidth = 100;
    std::cerr << "[";
    int pos = barWidth * (((double)total - (double)current) / (double)total);
    // std::cout<< pos;
    for (int i = 0; i < barWidth; i++) {
        if (i <= pos) std::cerr << "#";
        else std::cerr << "-";
    }
    std::cerr << "] " << int(pos) << " %\r";
    std::cerr.flush();
}
