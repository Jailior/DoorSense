#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include <cstdint>
#include <math.h>

class ImageProcessing {
public:
    ImageProcessing() = delete; // Prevent instantiation
    ~ImageProcessing() = delete; // Prevent instantiation
    static int countEdges(const uint8_t* image, int width, int height);
    static void detectEdges(const uint8_t* image, int width, int height, uint8_t* output);
    static void convertToGrayscale(const uint8_t* image, int width, int height, uint8_t* output);

private:
    
};

#endif // IMAGE_PROCESSING_H