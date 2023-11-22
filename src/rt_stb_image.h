#ifndef RTW_STB_IMAGE_H
#define RTW_STB_IMAGE_H

#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb_image.h"

#include <cstdlib>
#include <iostream>

class rt_image {

    public:
        // Constructors
        rt_image() : data(nullptr) {}

        rt_image(const char* image_filename) {
            // Loads image data from the specified file (into a big array of unsigned chars). 
            // If the RT_IMAGES environment variable is defined, looks only in that
            // directory for the image file. If the image was not found, searches for the specified
            // image file in the textures/ subdirectory. If the image was not
            // loaded successfully, width() and height() will return 0.

            auto filename = std::string(image_filename);
            auto imagedir = getenv("RTW_IMAGES");

            if (imagedir && load(std::string(imagedir) + "/" + image_filename)) return;
            if (load("textures/" + filename)) return;

            std::cerr << "ERROR: Could not load image file '" << image_filename << "'.\n";
        }

        // Destructor
        ~rt_image() { STBI_FREE(data); }

        // Functions
        bool load(const std::string filename) {
            // Loads image data from the given file name. Returns true if the load succeeded.
            auto n = bytes_per_pixel; // Dummy out parameter: original components per pixel
            data = stbi_load(filename.c_str(), &image_width, &image_height, &n, bytes_per_pixel);
            bytes_per_scanline = image_width * bytes_per_pixel;
            return data != nullptr;
        }

        int width() const { return (data==nullptr) ? 0 : image_width; }
        int height() const { return (data==nullptr) ? 0 : image_height; }

        const unsigned char* pixel_data(int x, int y) const {
            // Return the address of the three bytes of the pixel at x,y (or magenta if no data).
            static unsigned char magenta[] = { 255, 0, 255 };
            if (data == nullptr) return magenta;

            x = clamp(x, 0, image_width);
            y = clamp(y, 0, image_height);

            return data + y*bytes_per_scanline + x*bytes_per_pixel; // This is how you navigate with the pointer:
            // --> Returned 'data' points to the first char in the array, which is the first 8-bit component (there are n)
            // --> of the first pixel (for an RGB image, that is the R value of the first pixel).
            // --> We then increment the pointer go get the pointer, pointing to the first channel of the desired pixel
            // --> After we reach the desired pixel, we can use pointer indexing (ptr[i]) to get all channel values
        }

    private:
        const int bytes_per_pixel = 3;
        unsigned char *data; // ptr, points to the object address
        int image_width, image_height;
        int bytes_per_scanline;

        static int clamp(int x, int low, int high) { // Prevent values outside the range
            // Return the value clamped to the range [low, high).
            if (x < low) return low;
            if (x < high) return x;
            return high - 1;
    }

};

#endif