#include "tracer.h"

#include "scn/scenes.h"
#include "color.h"
#include "corporealList.h"
#include "material.h"
#include "sphere.h"
#include "triangle.h"
#include "aarect.h"
#include "bvh.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include "unistd.h"


void progressOut(int i, int imageHeight);
void allocateThread(int pixelsToAllocate, int imageHeight, int imageWidth, int& i, int& j, CorporealList& world);
void tracePixel(int iStart, int iEnd, int jStart, int jEnd, int imageHeight, int imageWidth, CorporealList world);
Color rayColor(const Ray& r, const Color& background, const Corporeal& world, int depth);
double hitSphere(const Point3& center, double radius, const Ray& r);

int maxThreads = std::thread::hardware_concurrency();
Color imageBuffer[imageHeight][imageWidth];
std::vector<std::thread> threads;


int main() {
    //DEBUGTIMER
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    
    // Define World with objects
    CorporealList world = worldGen(SCENE);
    // Define Camera

    // Define output
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-result" 
    freopen("out.ppm", "w", stdout);
    #pragma GCC diagnostic pop 

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
                pixelColor += rayColor(r, background, world, maxBounceDepth);              
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


Color rayColor(const Ray& r, const Color& background, const Corporeal& world, int depth) {
    HitRecord rec;

    // If the ray bounce limit is exceeded do not continue.
    if (depth <= 0) {
        return Color(0,0,0);
    }
    
    // If the ray hits a physical ("Corporeal") object, diffuse. tMin is 0.001 to solve floating point bugs around 0.
    if (world.hit(r, 0.001, infinity, rec)) {
        Ray scattered;
        Color attenuation;
        Color emitted = rec.matPtr->emitted(rec.u, rec.v, rec.p);
        
        // Scatter the ray in accordance with the Corporeal's Material.
        if (rec.matPtr->scatter(r, rec, attenuation, scattered)) {
            return emitted + attenuation * rayColor(scattered, background, world, depth - 1);
        }
        // If the material doesn't scatter we return the emitted color.
        return emitted;
    }
    // If we don't hit anything in the first place we return the BG color.
    return background;
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
