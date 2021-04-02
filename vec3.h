#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

using std::sqrt;

class vec3 {
    public:
        vec3() : e{0,0,0} {};
        vec3(double e0, double e1, double e2) : e{e0, e1, e2} {};
        
        // Getters for XYZ or their alias RGB
        double x() const { return e[0];}
        double y() const { return e[1];}
        double z() const { return e[2];}
        double r() const { return e[0];}
        double g() const { return e[1];}
        double b() const { return e[2];}

        // - operator returns the negated value of all elements
        vec3 operator-() const { 
            return vec3(-e[0], -e[1], -e[2]);
        }
        // vec3[i] returns e[i], also when asked by reference
        double operator[](int i) const { return e[i]; }
        double& operator[](int i) { return e[i]; }

        // Adding another vector to this through v1 (this) += v2 just adds all the components of v2 to v1 and returns v1;
        vec3& operator+=(const vec3 &v) {
            e[0] += v.e[0];
            e[1] += v.e[1];
            e[2] += v.e[2];
            return *this;
        }

        // Same for multiplication
        vec3& operator*=(const double t) {
            e[0] *= t;
            e[1] *= t;
            e[2] *= t;
            return *this;
        }

        // Dividing by a number through v1 /= t returns the multiplied by 1/t vector.
        vec3& operator/=(const double t) {
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

    public:
        double e[3];
};

// Type aliases
using point3 = vec3;
using color = vec3;


// << output operator returns the 3 values of the vector like "e0 e1 e2"
inline std::ostream& operator<<(std::ostream &out, const vec3 &v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

// + operator sums the two vector references and returns a new one
inline vec3 operator+(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

// See operator+
inline vec3 operator-(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

// See operator+
inline vec3 operator*(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

// Multiplying with a double `t` returns a new `vec3` with each element multiplied by `t`
inline vec3 operator*(double t, const vec3 &v) {
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

// Multiplying with a double `t` returns a new `vec3` with each element multiplied by `t`
inline vec3 operator*(const vec3 &v, double t) {
    return t * v;
}

// Dividing a `vec3 v` with a double `t` returns the result of multiplying `v` with `1/t`.
inline vec3 operator/(vec3 v, double t) {
    return (1/t) * v;
}

// Dot product.
inline double dot(const vec3 &u, const vec3 &v) {
    return u.e[0] * v.e[0]
         + u.e[1] * v.e[1]
         + u.e[2] * v.e[2];
}

// Cross product.
inline vec3 cross(const vec3 &u, const vec3 &v) {
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

// Unit vector.
inline vec3 unitVector(vec3 v) {
    return v / v.length();
}

#endif