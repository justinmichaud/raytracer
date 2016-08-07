#include <iostream>
#include <cmath>
#include "Vector.h"

class Camera {
public:
    Vec pos = Vec(0,0,0);
    Vec up = Vec(0,1,0);
    Vec right = Vec(1,0,0);
    Vec forward = Vec(0,0,1);
    float f = 1;
};

class Sphere {
public:
    Vec pos = Vec(0,0,0);
    float radius = 1;
    
    Sphere() {}
    
    Sphere(Vec pos, float radius): pos{pos}, radius{radius} {}
    
    Vec collision(Vec origin, Vec direction) {
        float solution = this->solution(origin, (!direction));
        if (solution < 0) return Vec::INFINITE;
        
        return (!direction)*solution;
    }
    
private:    
    float solution(Vec o, Vec l) {
        float desc = (l*(o-this->pos))*(l*(o-this->pos))
            - (~(o-this->pos)) + this->radius*this->radius;
        if (desc < 0) return -1;
        
        desc = sqrt(desc);
        
        float t1 = -(l*(o-this->pos)) + desc;
        float t2 = -(l*(o-this->pos)) - desc;
        
        if (t2 > 0 && t2 <= t1) return t2;
        if (t1 > 0 && t1 <= t2) return t1;
        return -1;
    }
    
};

Vec sample(float x, float y, Sphere s) {
    Vec pos = Vec(x*10,y*10,-2);
    Vec dir = Vec(0,0,1);
    
    Vec sol = s.collision(pos, dir);
    if (~(sol - pos) > 1000) return (Vec((y + 0.5)*0.3 + 0.05, (y + 0.5)*0.3 + 0.05, (y + 0.5)*0.7 + 0.05));
    else return Vec(1,1,1);
}

int main() {
    Camera camera;
    
    Sphere s;

    //Netppm image
    int width = 300;
    int height = 255;
    std::cout << "P3 " << width << " " << height << " 255";
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Vec color = sample((x/(float) width) - 0.5, (y/(float) height) - 0.5, s);
            
            //Convert form [0.0, 1.0] to [0, 255]
            std::cout << " " << int(color.x*255.0) 
                << " " << int(color.y*255.0) 
                << " " << int(color.z*255.0);
        } 
    }
    
    return 0;
}
