#pragma once
#include <iostream>
#include <cmath>
#include <limits>

class Vec {
public:
    float x,y,z;
    
    static const Vec ZERO;
    static const Vec INFINITE;

    Vec(float x, float y, float z): x(x), y(y), z(z) {}
    
    Vec& operator +=(const Vec& rhs) {
        this->x += rhs.x;
        this->y += rhs.y;
        this->z += rhs.z;
        return *this;
    }
    
    friend Vec operator +(Vec lhs, const Vec& rhs) {
        lhs += rhs;
        return lhs;
    }
    
    Vec& operator *=(float k) {
        this->x *= k;
        this->y *= k;
        this->z *= k;
        return *this;
    }
    
    friend Vec operator *(Vec lhs, float rhs) {
        lhs *= rhs;
        return lhs;
    }
    
    friend Vec operator *(float lhs, Vec rhs) {
        rhs *= lhs;
        return rhs;
    }
    
    Vec& operator -=(const Vec& rhs) {
        this->x -= rhs.x;
        this->y -= rhs.y;
        this->z -= rhs.z;
        return *this;
    }
    
    friend Vec operator -(Vec lhs, const Vec& rhs) {
        lhs -= rhs;
        return lhs;
    }
    
    Vec& operator /=(float rhs) {
        this->x /= rhs;
        this->y /= rhs;
        this->z /= rhs;
        return *this;
    }
    
    friend Vec operator /(Vec lhs, float rhs) {
        lhs /= rhs;
        return lhs;
    }
    
    friend Vec operator /(float lhs, Vec rhs) {
        rhs /= lhs;
        return rhs;
    }
    
    friend Vec operator -(Vec a) {
        return -1*a;
    }
    
    // Magnitude
    friend float operator ~(Vec a) {
        return std::sqrt(a.x*a.x+a.y*a.y+a.z*a.z);
    }
    
    // Norm
    friend Vec operator !(Vec a) {
        return a/~a;
    }
    
    // Dot product
    friend float operator *(const Vec& lhs, const Vec& rhs) {
        return lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z;
    }
    
    // Cross product
    friend Vec operator %(const Vec& lhs, const Vec& rhs) {
        return Vec(lhs.y*rhs.z-rhs.y*lhs.z, 
                   lhs.z*rhs.x-rhs.z*lhs.x, 
                   lhs.x*rhs.y-rhs.x*lhs.y);
    }
    
    friend std::ostream& operator <<(std::ostream& strm, const Vec& a) {
        return strm << "(" << a.x << "," << a.y << "," << a.z << ")";
    }     
};

const Vec Vec::ZERO = Vec(0,0,0);
const Vec Vec::INFINITE = Vec(std::numeric_limits<float>::infinity(), 
        std::numeric_limits<float>::infinity(), 
        std::numeric_limits<float>::infinity());

