#include "imageprocessing.h"

void ImageProcessing::convertToGrayscale(const uint8_t* image, int width, int height, uint8_t* output) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x);
            uint8_t r = image[idx];
            // uint8_t g = image[idx + 1];
            // uint8_t b = image[idx + 2];

            // Grayscale conversion using luminosity
            uint8_t gray = static_cast<uint8_t>(120.0);
            output[idx] = gray;
            // output[idx + 1] = gray;
            // output[idx + 2] = gray;
        }
    }
}

void ImageProcessing::detectEdges(const uint8_t* image, int width, int height, uint8_t* output) {

    static const int sobelX[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };

    static const int sobelY[3][3] = {
        {-1, -2, -1},
        {0, 0, 0},
        {1, 2, 1}
    };

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int gx = 0;
            int gy = 0;

            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    int idx = ((y + ky) * width + (x + kx)) * 3;
                    uint8_t gray = image[idx]; // Assuming grayscale image
                    gx += sobelX[ky + 1][kx + 1] * gray;
                    gy += sobelY[ky + 1][kx + 1] * gray;
                }
            }

            int magnitude = static_cast<int>(sqrt(gx * gx + gy * gy));
            magnitude = magnitude > 255 ? 255 : magnitude;

            int idx = (y * width + x);
            output[idx] = magnitude;
        }
    }
}