#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

using std::sqrt;

class Vec3 {
    public:
        Vec3() : e{0,0,0} {};
        Vec3(double e0, double e1, double e2) : e{e0, e1, e2} {};
        
        // Getters for XYZ or their alias RGB
        double x() const { return e[0];}
        double y() const { return e[1];}
        double z() const { return e[2];}
        double r() const { return e[0];}
        double g() const { return e[1];}
        double b() const { return e[2];}

        // - operator returns the negated value of all elements
        Vec3 operator-() const { 
            return Vec3(-e[0], -e[1], -e[2]);
        }
        // Vec3[i] returns e[i], also when asked by reference
        double operator[](int i) const { return e[i]; }
        double& operator[](int i) { return e[i]; }

        // Adding another vector to this through v1 (this) += v2 just adds all the components of v2 to v1 and returns v1;
        Vec3& operator+=(const Vec3 &v) {
            e[0] += v.e[0];
            e[1] += v.e[1];
            e[2] += v.e[2];
            return *this;
        }

        // Same for multiplication
        Vec3& operator*=(const double t) {
            e[0] *= t;
            e[1] *= t;
            e[2] *= t;
            return *this;
        }

        // Dividing by a number through v1 /= t returns the multiplied by 1/t vector.
        Vec3& operator/=(const double t) {
            return *this *= 1/t;
        }

        // Returns length of vector
        double length() const {
            return sqrt(lengthSquared());
        }

        // Squares and sums all components, returns the double.
        double lengthSquared() const {
            return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
        }

        // Generate a Vec3 with random elements [0,1]
        inline static Vec3 random() {
            return Vec3(randomDouble(), randomDouble(), randomDouble());
        }

        // Generate a Vec3 with random elements [min,max]
        inline static Vec3 random(double min, double max) {
            return Vec3(randomDouble(min, max), randomDouble(min, max), randomDouble(min, max));
        }

    public:
        double e[3];
};

// Type aliases
using Point3 = Vec3;
using Color = Vec3;


// << output operator returns the 3 values of the vector like "e0 e1 e2"
inline std::ostream& operator<<(std::ostream &out, const Vec3 &v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

// + operator sums the two vector references and returns a new one
inline Vec3 operator+(const Vec3 &u, const Vec3 &v) {
    return Vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

// See operator+
inline Vec3 operator-(const Vec3 &u, const Vec3 &v) {
    return Vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

// See operator+
inline Vec3 operator*(const Vec3 &u, const Vec3 &v) {
    return Vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

// Multiplying with a double `t` returns a new `Vec3` with each element multiplied by `t`
inline Vec3 operator*(double t, const Vec3 &v) {
    return Vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

// Multiplying with a double `t` returns a new `Vec3` with each element multiplied by `t`
inline Vec3 operator*(const Vec3 &v, double t) {
    return t * v;
}

// Dividing a `Vec3 v` with a double `t` returns the result of multiplying `v` with `1/t`.
inline Vec3 operator/(Vec3 v, double t) {
    return (1/t) * v;
}

// Dot product.
inline double dot(const Vec3 &u, const Vec3 &v) {
    return u.e[0] * v.e[0]
         + u.e[1] * v.e[1]
         + u.e[2] * v.e[2];
}

// Cross product.
inline Vec3 cross(const Vec3 &u, const Vec3 &v) {
    return Vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

// Unit vector.
inline Vec3 unitVector(Vec3 v) {
    return v / v.length();
}


// Generate a random vector p in the unit sphere. If its length squared is
inline Vec3 randomVectorInUnitSphere() {
    //TODO:: i is purely for a performance check here. Delete in future
    // int i = 0;
    while (true) {
        auto p = Vec3::random(-1,1);
        // i++;
        if (p.lengthSquared() >= 1) continue;
        // if (i > 5) std::cerr << i << ' ';
        return p;
    }
}

// Uses `randomVectorInUnitSphere()` to generate a random vector and normalizes it.
Vec3 randomUnitVector() {
    return unitVector(randomVectorInUnitSphere());
}

// Generates a random unit sphere vector for the random in hemisphere diffuser method.
Vec3 randomInHemisphere(const Vec3& normal) {
    Vec3 inUnitSphere = randomVectorInUnitSphere();
    
    //If the random vector is pointing in the same direction as the normal return it.
    if (dot(inUnitSphere, normal) > 0.0) {
        return inUnitSphere;
    } // Otherwise return the backwards vector.
    else return -inUnitSphere;
}

#endif