#include "vec3.h"
#include "ray.h"
#include "color.h"

#include <iostream>

void progressOut(int i, int imageHeight);
color rayColor(const ray& r);

int main() {
    // Define image parameters
    const auto aspectRatio = 16.0 / 9.0;
    const int imageWidth = 400;
    const int imageHeight = (int)(imageWidth / aspectRatio);

    // Define camera parameters
    auto viewportHeight = 2.0;
    auto viewportWidth = aspectRatio * viewportHeight;
    auto focalLength = 1.0;

    auto origin = point3(0, 0, 0);
    auto hor = vec3(viewportWidth, 0, 0);
    auto ver = vec3(0, viewportHeight, 0);
    auto llCorner = origin - hor / 2 - ver / 2 - vec3(0, 0, focalLength);

    // Define output
    freopen("out.ppm", "w", stdout);

    // Render PPM
    std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";
    
    // (0,0) is bottom left. 
    // Thus working from top to bottom left to right has us counting down for rows and counting up columns.
    for (int i = imageHeight - 1; i >= 0; i--) {
        progressOut(i, imageHeight);


        for (int j = 0; j < imageWidth; j++) {
            auto u = double(j) / (imageWidth - 1);
            auto v = double(i) / (imageHeight - 1);

            // Init ray
            vec3 direction;
            ray r(origin,  llCorner + u*hor + v*ver - origin);
            color pixelColor = rayColor(r);
            // Write the color to `cout`
            writeColor(std::cout, pixelColor);
        }
    }


    std::cerr << "\nRender complete.\n";
    return 0;
}

// Returns BG color of a ray based on its Y direction
color rayColor(const ray& r) {
    // Get the unit direction of the ray 
    vec3 unitDir = unitVector(r.direction());
    // Grab the y component of the unit vector. |unitvector| = 1 so the max length of y is also 1.
    // Add 1 and divide by 2 to get the distance to the top or bottom of the viewport.
    auto t = 0.5 * (unitDir.y() +  1.0);

    // Set a BG gradient
    color color1BG = color(1.0, 1.0, 1.0);
    color color2BG = color(0.5, 0.7, 1.0);
    return (1.0 - t) * color1BG + t * color2BG;
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