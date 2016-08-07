#include <iostream>
#include <cmath>
#include <vector>
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
        
        desc = sqrt(desc);
        
        float t1 = -(l*(o-this->pos)) + desc;
        float t2 = -(l*(o-this->pos)) - desc;
        
        //Discard negative or zero solutions
        if (t1 <= 0) t1 = t2;
        if (t2 <= 0) t2 = t1;
        if (t1 <= 0 && t2 <= 0) return -1;
        
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
    
    return Vec(y*0.3 + 0.05, y*0.3 + 0.05, y*0.7 + 0.05);
}

Vec sample(Vec pos, Vec dir, const std::vector<Sphere>& world, int recursiveCount) {
    if (recursiveCount >= 2) return background(pos,dir);
    
    Vec intersection = Vec::INFINITE;
    const Sphere* sphere = &world[0];
    
    for (const Sphere& s : world) {
        Vec sol = s.collision(pos, dir);
        if (sol.isinf()) continue;
        
        if (~(sol - pos) <= ~(intersection - pos)) {
            intersection = sol;
            sphere = &s;
        }
    }
    
    if (intersection.isinf()) return background(pos, dir);
    
    Vec normal = !sphere->objectSpace(intersection);
    
    Vec reflectedDirection = dir + (-2*normal*(normal*dir));    
    Vec reflectedSample = sample(intersection + reflectedDirection*0.0001, 
        reflectedDirection, world, recursiveCount+1);
    
    float shade = reflectedDirection * normal;
    if (shade < 0) shade = 0;
    
    float ambient = 0.2, diffuse = 0.7;
    
    return Vec(1,1,1) * (ambient + diffuse*shade) * 0.1 + 0.9*reflectedSample;
}

int main() {
    Camera camera;
    camera.pos = Vec(0,0,-2);
    std::vector<Sphere> world = {
        Sphere(Vec(-0.5,0,0), 0.45), 
        Sphere(Vec(0.5,0,0), 0.45)
    };

    //Netppm image
    int width = 300;
    int height = 255;
    std::cout << "P3 " << width << " " << height << " 255";
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float normX = (x/(float) width) - 0.5;
            float normY = (y/(float) height) - 0.5;
            Vec dir = !Vec(normX, normY, camera.f);
            Vec color = sample(camera.pos, dir, world, 0);
            
            //Convert form [0.0, 1.0] to [0, 255]
            std::cout << " " << int(color.x*255.0) 
                << " " << int(color.y*255.0) 
                << " " << int(color.z*255.0);
        } 
    }
    
    return 0;
}
