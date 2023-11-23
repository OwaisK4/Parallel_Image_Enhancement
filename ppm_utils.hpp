#ifndef PPM_UTILS_HPP
#define PPM_UTILS_HPP
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <omp.h>
#include <string>
#include <vector>
namespace fs = std::filesystem;

// TODO: Calculate image entropy (data loss)

std::vector<std::vector<double>> kernel = {{1, 4, 6, 4, 1},
                                           {4, 16, 24, 16, 4},
                                           {6, 24, 36, 24, 6},
                                           {4, 16, 24, 16, 4},
                                           {1, 4, 6, 4, 1}};

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

std::vector<HSV> RGB_to_HSV(const PPMObject &RGB_image, int threads) {
    size_t size = RGB_image.width * RGB_image.height;
    std::vector<HSV> hsv(size);
#pragma omp parallel for num_threads(threads) shared(RGB_image, hsv, size)
    for (int i = 0; i < size; i++) {
        double scaled_red = RGB_image.pixels[i].red / 255.0;
        double scaled_green = RGB_image.pixels[i].green / 255.0;
        double scaled_blue = RGB_image.pixels[i].blue / 255.0;
        double c_max = std::max({scaled_red, scaled_green, scaled_blue});
        double c_min = std::min({scaled_red, scaled_green, scaled_blue});
        double delta = c_max - c_min;
        double h, s, v;
        if (delta == 0) {
            h = 0.0;
        } else {
            if (c_max == scaled_red) {
                h = 60.0 * ((scaled_green - scaled_blue) / delta);
            } else if (c_max == scaled_green) {
                h = 60.0 * ((scaled_blue - scaled_red) / delta) + 120.0;
            } else {
                h = 60.0 * ((scaled_red - scaled_green) / delta) + 240.0;
            }
            h = std::fmod(h, 360.0);
            if (h < 0.0)
                h = h + 360.0;
            // h = h / 2;
            if (c_max == 0)
                s = 0;
            else
                s = (delta / c_max) * 255.0;
            v = c_max * 255.0;
            hsv[i].hue = h;
            hsv[i].saturation = s;
            hsv[i].value = v;
        }
    }
    return hsv;
}

void HSV_to_RGB(PPMObject &image, std::vector<HSV> &hsv, int threads) {
    size_t size = image.width * image.height;
#pragma omp parallel for num_threads(threads) shared(image, hsv, size)
    for (int i = 0; i < size; i++) {
        double R, G, B;
        double H = hsv[i].hue;
        double S = hsv[i].saturation / 255.0;
        double V = hsv[i].value / 255.0;
        double C = S * V;
        double X = C * (1.0 - std::fabs(std::fmod((H / 60.0), 2.0) - 1.0));
        // std::cout << "X = " << X << "\n";
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
        R = std::min(255.0, std::max(0.0, R));
        G = std::min(255.0, std::max(0.0, G));
        B = std::min(255.0, std::max(0.0, B));

        image.pixels[i].red = R;
        image.pixels[i].green = G;
        image.pixels[i].blue = B;
    }
}

std::vector<HSV> BlurImage(const PPMObject &image, const std::vector<HSV> &original, int threads) {
    std::vector<HSV> hsv(original);
    double fIntens = 0, curIntens = 0;
#pragma omp parallel for num_threads(threads) shared(image, hsv, original) private(fIntens, curIntens) collapse(2)
    for (int i = 2; i < image.height - 2; i++) {
        for (int j = 2; j < image.width - 2; j++) {
            fIntens = 0;
            for (int k = -2; k <= 2; k++) {
                for (int l = -2; l <= 2; l++) {
                    double I = original[((i + k) * image.width) + (j + l)].value;
                    double H = kernel[k + 2][l + 2];
                    curIntens = I * H;
                    fIntens += curIntens;
                }
            }
            hsv[(i * image.width) + j].value = fIntens / 256.0;
        }
    }
    return hsv;
}

std::vector<HSV> GenerateImageMask(const std::vector<HSV> &image, const std::vector<HSV> &blurred_image, int threads) {
    std::vector<HSV> mask(image);
    int n = image.size();
#pragma omp parallel for num_threads(threads) shared(image, blurred_image, mask)
    for (int i = 0; i < n; i++) {
        // mask[i].value = image[i].value - blurred_image[i].value;
        mask[i].value = image[i].value + (image[i].value - blurred_image[i].value);
        mask[i].value = std::max(0.0, std::min(255.0, mask[i].value));
    }
    return mask;
}
std::vector<HSV> SharpenImage(const std::vector<HSV> &image, const std::vector<HSV> &image_mask, int threads) {
    int n = image.size();
    std::vector<HSV> sharpened(image);
    double m = 1.0;
#pragma omp parallel for num_threads(threads) shared(image, image_mask, sharpened) firstprivate(m)
    for (int i = 0; i < n; i++) {
        sharpened[i].value = image[i].value + (m * image_mask[i].value);
        sharpened[i].value = std::max(0.0, std::min(255.0, sharpened[i].value));
    }
    return sharpened;
}

PPMObject RGB_to_Grayscale(const PPMObject &RGB_image, int threads) {
    PPMObject grayscale_image(RGB_image);
    size_t size = RGB_image.width * RGB_image.height;
#pragma omp parallel for num_threads(threads) shared(RGB_image, grayscale_image, size)
    for (int i = 0; i < size; i++) {
        int grayVal = (RGB_image.pixels[i].red * 0.21) + (RGB_image.pixels[i].green * 0.72) + (RGB_image.pixels[i].blue * 0.07);
        grayscale_image.pixels[i].red = grayVal;
        grayscale_image.pixels[i].green = grayVal;
        grayscale_image.pixels[i].blue = grayVal;
    }
    return grayscale_image;
}

PPMObject RGB_Extract_Red(const PPMObject &RGB_image, int threads) {
    PPMObject red_image(RGB_image);
    size_t size = RGB_image.width * RGB_image.height;
#pragma omp parallel for num_threads(threads) shared(red_image, size)
    for (int i = 0; i < size; i++) {
        red_image.pixels[i].green = 0;
        red_image.pixels[i].blue = 0;
    }
    return red_image;
}
PPMObject RGB_Extract_Green(const PPMObject &RGB_image, int threads) {
    PPMObject green_image(RGB_image);
    size_t size = RGB_image.width * RGB_image.height;
#pragma omp parallel for num_threads(threads) shared(green_image, size)
    for (int i = 0; i < size; i++) {
        green_image.pixels[i].red = 0;
        green_image.pixels[i].blue = 0;
    }
    return green_image;
}
PPMObject RGB_Extract_Blue(const PPMObject &RGB_image, int threads) {
    PPMObject blue_image(RGB_image);
    size_t size = RGB_image.width * RGB_image.height;
#pragma omp parallel for num_threads(threads) shared(blue_image, size)
    for (int i = 0; i < size; i++) {
        blue_image.pixels[i].red = 0;
        blue_image.pixels[i].green = 0;
    }
    return blue_image;
}

void convert_to_ppm(std::string file) {
    if (!fs::exists(file)) {
        std::cout << "Input image doesn't exist. Exiting.\n";
        return;
    }
    int index = file.find(".", 0);
    std::string basename = file.substr(0, index);
    std::string command = "convert '" + file + "' -compress none '" + basename + ".ppm'";
    system(command.c_str());
    if (file.find("ppm", 0) == file.npos)
        remove(file.c_str());
}

void convert_all_to_ppm(std::vector<std::string> files, int threads) {
#pragma omp parallel for num_threads(threads)
    for (int i = 0; i < files.size(); i++) {
        std::string file = files[i];
        int index = file.find(".", 0);
        std::string basename = file.substr(0, index);
        std::string command = "convert '" + file + "' -compress none '" + basename + ".ppm'";
        system(command.c_str());
        if (file.find("ppm", 0) == file.npos)
            remove(file.c_str());
    }
}

void convert_from_ppm(std::string file) {
    if (!fs::exists(file)) {
        std::cout << "Input image doesn't exist. Exiting.\n";
        return;
    }
    int index = file.find(".", 0);
    std::string basename = file.substr(0, index);
    std::string command = "convert '" + file + "' '" + basename + ".jpg'";
    system(command.c_str());
    if (file.find("ppm", 0) != file.npos)
        remove(file.c_str());
}

void convert_all_from_ppm(std::vector<std::string> files, int threads) {
#pragma omp parallel for num_threads(threads)
    for (int i = 0; i < files.size(); i++) {
        std::string file = files[i];
        int index = file.find(".", 0);
        std::string basename = file.substr(0, index);
        std::string command = "convert '" + file + "' '" + basename + ".jpg'";
        system(command.c_str());
        if (file.find("ppm", 0) != file.npos)
            remove(file.c_str());
    }
}

#endif