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
    float radius = 0.5;
    
    Sphere() {}
    
    Sphere(Vec pos, float radius): pos{pos}, radius{radius} {}
    
    Vec collision(const Vec& origin, const Vec& direction) const {
        float solution = this->solution(origin, direction);
        if (solution < 0) return Vec::INFINITE;
        
        return (direction*solution) + origin;
    }
    
    Vec objectSpace(Vec x) const {
        return x - pos;
    }
    
private:    
    float solution(const Vec& o, const Vec& l) const {
        //Solve for line-sphere intersection: 
        // https://en.wikipedia.org/wiki/Line%E2%80%93sphere_intersection
        float desc = (l*(o-this->pos))*(l*(o-this->pos))
            - (~(o-this->pos))*(~(o-this->pos)) + (this->radius*this->radius);
        if (desc < 0) return -1;
        
        //std::cerr << "Desc: " << desc << std::endl;
        
        desc = sqrt(desc);
        
        float t1 = -(l*(o-this->pos)) + desc;
        float t2 = -(l*(o-this->pos)) - desc;
        
        //std::cerr << "Solutions: " << t1 << "," << t2 << std::endl;
        
        //Discard negative solutions
        if (t1 < 0) t1 = t2;
        if (t2 < 0) t2 = t1;
        if (t1 < 0 && t2 < 0) return -1;
        
        //Return smallest possible solution        
        if (t2 <= t1) return t2;
        else return t1;
    }    
};

Vec background(const Vec& pos, const Vec& dir) {
    const Sphere background = Sphere(pos, 0.5);
    Vec b = background.collision(pos, dir);
    if (b.isinf()) return Vec(1,0,0); //Should not happen
    
    float y = background.objectSpace(b).y/background.radius/2 + 0.5;
    
    //std::cerr << y << std::endl;
    
    return Vec(y*0.3 + 0.05, y*0.3 + 0.05, y*0.7 + 0.05);
}

Vec sample(float x, float y, Sphere s) {
    Vec pos = Vec(x,y,-2);
    Vec dir = Vec(0,0,1);
    
    Vec sol = s.collision(pos, dir);
    if (sol.isinf()) return background(pos, dir);
    
    Vec colour (0.1,0.1,0.1);
    
    return colour;
}

int main() {
    Camera camera;
    
    Sphere s;
    
    //std::cerr << background(Vec(0,0,-2), Vec(0,0,1)) << std::endl;

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
