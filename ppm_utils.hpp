#include <fstream>
#include <iostream>
#include <string>

struct PPMObject {
    std::string magicNum;
    int width, height, maxColVal;
    char *m_Ptr;
};

std::istream &operator>>(std::istream &inputStream, PPMObject &other) {
    inputStream >> other.magicNum;
    inputStream >> other.width >> other.height >> other.maxColVal;
    inputStream.get(); // skip the trailing white space
    size_t size = other.width * other.height * 3;
    other.m_Ptr = new char[size];
    inputStream.read(other.m_Ptr, size);
    return inputStream;
}

std::ostream &operator<<(std::ostream &outputStream, const PPMObject &other) {
    outputStream << "P6"
                 << "\n"
                 << other.width << " "
                 << other.height << "\n"
                 << other.maxColVal << "\n";
    size_t size = other.width * other.height * 3;
    outputStream.write(other.m_Ptr, size);
    return outputStream;
}