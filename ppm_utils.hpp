#include <cmath>
#include <fstream>
#include <iostream>
#include <omp.h>
#include <string>
#include <vector>

class Pixel {
public:
    int red, green, blue;
};
class HSV {
public:
    double hue, saturation, value;
};

class PPMObject {
public:
    std::string magicNum;
    int width, height, maxColVal;
    std::vector<Pixel> pixels;

    PPMObject() {}
    PPMObject(std::string magicNum, int width, int height, int maxColVal) {
        this->magicNum = magicNum;
        this->width = width;
        this->height = height;
        this->maxColVal = maxColVal;
        pixels.resize(width * height);
    }
    PPMObject(const PPMObject &obj) {
        this->magicNum = obj.magicNum;
        this->width = obj.width;
        this->height = obj.height;
        this->maxColVal = obj.maxColVal;
        pixels = obj.pixels;
    }
};

std::istream &operator>>(std::istream &inputStream, PPMObject &image) { // For reading ppm image from stream
    inputStream >> image.magicNum;
    inputStream >> image.width >> image.height >> image.maxColVal;
    size_t size = image.width * image.height;
    image.pixels.resize(size);
    for (int i = 0; i < size; i++) {
        inputStream >> image.pixels[i].red >> image.pixels[i].green >> image.pixels[i].blue;
    }
    return inputStream;
}

std::ostream &operator<<(std::ostream &outputStream, const PPMObject &image) { // For writing ppm image to stream
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

void RGB_to_HSV(PPMObject &RGB_image, int threads) {
    size_t size = RGB_image.width * RGB_image.height;
#pragma omp parallel for num_threads(threads) shared(RGB_image, size)
    for (int i = 0; i < size; i++) {
        double scaled_red = RGB_image.pixels[i].red / 255.0;
        double scaled_green = RGB_image.pixels[i].green / 255.0;
        double scaled_blue = RGB_image.pixels[i].blue / 255.0;
        double c_min = std::min(scaled_red, std::min(scaled_green, scaled_blue));
        double c_max = std::max(scaled_red, std::max(scaled_green, scaled_blue));
        double delta = c_max - c_min;
        double h, s, v;
        if (delta == 0) {
            h = 0;
            s = 0;
            v = c_max * 255;
        } else {
            if (c_max == scaled_red) {
                h = 60.0 * ((scaled_green - scaled_blue) / delta);
            } else if (c_max == scaled_green) {
                h = 120.0 * ((scaled_blue - scaled_red) / delta);
            } else {
                h = 60.0 * ((scaled_red - scaled_green) / delta);
            }
            if (h < 0)
                h = h + 360;
            h = ceil(h / 2);
            if (c_max == 0)
                s = 0;
            else
                s = (delta / c_max) * 255.0;
            v = c_max * 255;
            RGB_image.pixels[i].red = h;
            RGB_image.pixels[i].green = s;
            RGB_image.pixels[i].blue = v;
        }
    }
}

void HSV_to_RGB(PPMObject &HSV_image, int threads) {
    size_t size = HSV_image.width * HSV_image.height;
#pragma omp parallel for num_threads(threads) shared(HSV_image, size)
    for (int i = 0; i < size; i++) {
        double R, G, B;
        int H = HSV_image.pixels[i].red * 2;
        double S = HSV_image.pixels[i].green / 255.0;
        double V = HSV_image.pixels[i].blue / 255.0;
        double C = S * V;
        double X = C * (1.0 - fabs(fmod(((float)H / 60.0), 2.0) - 1.0));
        double m = V - C;
        if (H >= 0 && H < 60) {
            R = C;
            G = X;
            B = 0;
        } else if (H >= 60 && H < 120) {
            R = X;
            G = C;
            B = 0;
        } else if (H >= 120 && H < 180) {
            R = 0;
            G = C;
            B = X;
        } else if (H >= 180 && H < 240) {
            R = 0;
            G = X;
            B = C;
        } else if (H >= 240 && H < 300) {
            R = X;
            G = 0;
            B = C;
        } else {
            R = C;
            G = 0;
            B = X;
        }
        R = (R + m) * 255.0;
        G = (G + m) * 255.0;
        B = (B + m) * 255.0;
        HSV_image.pixels[i].red = R;
        HSV_image.pixels[i].green = G;
        HSV_image.pixels[i].blue = B;
    }
}

void RGB_to_Grayscale(PPMObject &RGB_image, int threads) {
    size_t size = RGB_image.width * RGB_image.height;
#pragma omp parallel for num_threads(threads) shared(RGB_image, size)
    for (int i = 0; i < size; i++) {
        int grayVal = (RGB_image.pixels[i].red * 0.21) + (RGB_image.pixels[i].green * 0.72) + (RGB_image.pixels[i].blue * 0.07);
        RGB_image.pixels[i].red = grayVal;
        RGB_image.pixels[i].green = grayVal;
        RGB_image.pixels[i].blue = grayVal;
    }
}