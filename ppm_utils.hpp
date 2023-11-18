#include <fstream>
#include <iostream>
#include <string>

struct Pixel {
    int red, green, blue;
};
struct PPMObject {
    std::string magicNum;
    int width, height, maxColVal;
    Pixel *pixels;
};

std::istream &operator>>(std::istream &inputStream, PPMObject &image) {
    inputStream >> image.magicNum;
    inputStream >> image.width >> image.height >> image.maxColVal;
    size_t size = image.width * image.height;
    image.pixels = new Pixel[size];
    for (int i = 0; i < size; i++) {
        inputStream >> image.pixels[i].red >> image.pixels[i].green >> image.pixels[i].blue;
    }
    return inputStream;
}

std::ostream &operator<<(std::ostream &outputStream, const PPMObject &image) {
    outputStream << "P3"
                 << "\n"
                 << image.width << " " << image.height << "\n"
                 << image.maxColVal << "\n";
    size_t size = image.width * image.height;
    for (int i = 0; i < size; i++) {
        outputStream << image.pixels[i].red << " " << image.pixels[i].green << " " << image.pixels[i].blue << " ";
    }
    return outputStream;
}