#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include <cstdint>
#include <math.h>

class ImageProcessing {
public:
    ImageProcessing() = delete; // Prevent instantiation
    ~ImageProcessing() = delete; // Prevent instantiation
    static int detectEdges(const uint8_t* image, int width, int height);
    static bool isDoorOpen(int edgeCount);
    
};

#endif // IMAGE_PROCESSING_H