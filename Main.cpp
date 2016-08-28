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
    Vec colour = Vec(1,1,1);
    float reflect=0.5;
    bool light = false;
    
    Sphere() {}
    Sphere(Vec pos, float radius): pos{pos}, radius{radius} {}
    Sphere(Vec pos, float radius, Vec colour, bool light): 
        pos{pos}, radius{radius}, colour{colour}, light{light} {}
    Sphere(Vec pos, float radius, Vec colour, float reflect): 
        pos{pos}, radius{radius}, colour{colour}, reflect{reflect} {}
    
    Vec collision(const Vec& origin, const Vec& direction) const {
        float solution = this->solution(origin, direction);
        if (solution < 0) return Vec::INFINITE;
        
        return (direction*solution) + origin;
    }
    
    Vec objectSpace(Vec x) const {
        return x - pos;
    }
    
    bool operator ==(const Sphere &b) const {
        return this->pos==b.pos && this->radius==b.radius && this->colour==b.colour
            && this->reflect==b.reflect && this->light==b.light;
    }
    
    bool operator !=(const Sphere &b) const {
        return !(*this==b);
    }
    
    friend std::ostream& operator <<(std::ostream& strm, const Sphere& a) {
        return strm << "Sphere(" << a.pos << "," << a.radius << ")";
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
    //Find intersection with ground plane
    //https://en.wikipedia.org/wiki/Line%E2%80%93plane_intersection
    
    float groundPlaneY = 1;
    Vec p0(0,groundPlaneY, 0);
    Vec n(0,1,0);
    
    float d = (p0 - pos)*n/(dir*n);
    Vec intersectionPlane = dir*d;
    if (d < 0) intersectionPlane = Vec::INFINITE;
    
    //Find intersection with skysphere
    const Sphere backgroundSphere = Sphere(pos, 50);
    Vec intersectionSphere = backgroundSphere.collision(pos, dir);
    
    if (intersectionPlane.isinf() && intersectionSphere.isinf()) return Vec(0,0,0);
    if (intersectionPlane.isinf() || intersectionSphere.y < groundPlaneY) {
        //Determine the sky gradient
        Vec obj = backgroundSphere.objectSpace(intersectionSphere)/backgroundSphere.radius;
        float h = (-obj.y+1.0)/2.0;
        return Vec(h*0.3 + 0.3, h*0.4 + 0.4, h*0.7 + 0.5);
    }
    else {
        //Checkerboard
        float scale = 2;
    
        if ((int((intersectionPlane.x+100)*scale)%2==0 
            || int((intersectionPlane.z+100)*scale)%2==0)
            && !(int((intersectionPlane.x+100)*scale)%2==0 && int((intersectionPlane.z+100)*scale)%2==0)) return Vec(1,0.1,0.1);
        else return Vec(1,1,1);
    }
}

const Sphere& traceRay(const Vec& pos, const Vec& dir, const std::vector<Sphere>& world) {
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
    
    return *sphere;
}

Vec sample(Vec pos, Vec dir, const std::vector<Sphere>& world, int recursiveCount) {
    if (recursiveCount >= 5) return Vec(0,0,0);
    
    const Sphere& sphere = traceRay(pos, dir, world);
    Vec intersection = sphere.collision(pos, dir);
    
    if (intersection.isinf()) return background(pos, dir);
    if (sphere.light) return sphere.colour;
    
    Vec normal = !sphere.objectSpace(intersection);
    
    //Fix aliasing when some values are inside the sphere due to rounding    
    intersection += normal*0.0001;
    
    Vec reflectedDirection = dir + (-2*normal*(normal*dir));    
    Vec reflectedSample = sample(intersection, 
        reflectedDirection, world, recursiveCount+1);

    //look for lights
    Vec lighting = Vec(0,0,0);
    for (const Sphere& light : world) {
        if (!light.light) continue;
    
        Vec lightDir = !(light.pos - intersection);
        const Sphere& closestObject = traceRay(intersection, lightDir, world);

        if (closestObject != light) continue; //We are in shaddow of another object        
        if (light.collision(intersection, lightDir).isinf()) continue; //This is not a real intersection
        
        //the object is illuminated by this light, so use cosine shading 
        float shading = lightDir*normal;
        if (shading < 0) continue;
              
        lighting += light.colour*shading;
    }

    return 0.5*Vec(sphere.colour.x*lighting.x, sphere.colour.y*lighting.y, sphere.colour.z*lighting.z) 
        + sphere.reflect*Vec(sphere.colour.x*reflectedSample.x, sphere.colour.y*reflectedSample.y, sphere.colour.z*reflectedSample.z);
}

int main() {
    Camera camera;
    camera.pos = Vec(0,0,-2);
    std::vector<Sphere> world = {
        Sphere(Vec(-1.1,-1.1,0), 0.01, Vec(1,1,1), true),
    };
    
    float name[5][5] = {
        {1,1,1,1,1},
        {0,0,1,0,0},
        {0,0,1,0,0},
        {1,0,1,0,0},
        {1,1,1,0,0},
    };
    
    for (int i=0; i<5; i++) {
        for (int j=0; j<5; j++) {
            if (name[j][i]) world.push_back(Sphere(Vec((i-2)*0.15,(j-2)*0.15,0), 0.1, Vec(1,1,1), 0.7f));
        }
    }

    //Netppm image
    int width = 500;
    int height = 500;
    std::cout << "P3 " << width << " " << height << " 255";
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float normX = (x/(float) width) - 0.5;
            float normY = (y/(float) height) - 0.5;
            Vec dir = !Vec(normX, normY, camera.f);
            Vec color = (255.0*sample(camera.pos, dir, world, 0)).clamp(0,255.0);
            
            //Convert form [0.0, 1.0] to [0, 255]
            std::cout 
                << " " << int(color.x)
                << " " << int(color.y)
                << " " << int(color.z);
        } 
    }
    
    return 0;
}
