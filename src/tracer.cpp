#include "tracer.h"

#include "color.h"
#include "corporealList.h"
#include "sphere.h"

#include <iostream>
#include <chrono>

void progressOut(int i, int imageHeight);
Color rayColor(const Ray& r, const Corporeal& world, int depth);
double hitSphere(const Point3& center, double radius, const Ray& r);

int main() {
    //DEBUGTIMER
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    
    // Define World with objects
    CorporealList world;
    world.add(make_shared<Sphere>(Point3(0, 0, -1), 0.5));
    world.add(make_shared<Sphere>(Point3(0, -100.5, -1), 100));

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
    std::cerr << "Elapsed time = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " [s]" << std::endl;
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
        Point3 target;
        // The target point of the new bounced ray is the result of a diffuse bounce.
        // That is, impact point P plus the Normal + a random point in the unit sphere tangent to P.
        if (DIFFUSER == LAMBERT_APPROX)
            target = rec.p + rec.normal + randomUnitVector();
        else {
            target = rec.p + randomInHemisphere(rec.normal);
        }
        
        // Recursively keep bouncing with a new ray with origin P and direction target (random point in unit sphere) - P.
        return 0.5 * rayColor(Ray(rec.p, target - rec.p), world, depth - 1);
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
