#include "tracer.h"

#include "color.h"
#include "corporealList.h"
#include "sphere.h"

#include <iostream>

void progressOut(int i, int imageHeight);
Color rayColor(const Ray& r, const Corporeal& world);
double hitSphere(const Point3& center, double radius, const Ray& r);

int main() {
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
            auto u = double(j) / (imageWidth - 1);
            auto v = double(i) / (imageHeight - 1);

            // Init Ray
            Vec3 direction;
            Ray r(origin,  llCorner + u*hor + v*ver - origin);
            Color pixelColor = rayColor(r, world);
            // Write the Color to `cout`
            writeColor(std::cout, pixelColor);
        }
    }


    std::cerr << "\nRender complete.\n";
    return 0;
}

// Returns BG Color of a Ray based on its Y direction
Color rayColor(const Ray& r, const Corporeal& world) {
    HitRecord rec;
    if (world.hit(r, 0, infinity, rec)) return 0.5 * (rec.normal + Color(1, 1, 1));

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
