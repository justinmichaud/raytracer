#include <iostream>
#include <cmath>
#include "Vector.h"

Vec sample(int x, int y) {
    return Vec(std::abs(std::sin(x)), std::abs(std::sin(y)), 0);
}

int main() {
    //Netppm image
    int width = 300;
    int height = 255;
    std::cout << "P3 " << width << " " << height << " 255";
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Vec color = sample(x,y);
            
            //Convert form [0.0, 1.0] to [0, 255]
            std::cout << " " << int(color.x*255.0) 
                << " " << int(color.y*255.0) 
                << " " << int(color.z*255.0);
        } 
    }
    
    return 0;
}
