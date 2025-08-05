#include "fontconvert.hpp"
#include <iostream>
#include <string>

void usage(const char* prog) {
    std::cerr << "Usage: " << prog << " <font.ttf> <size_pt> <first_char> <last_char> [output.h]\n";
    std::cerr << "Example: " << prog << " FreeSans.ttf 18 32 126 FreeSans18pt7b.h\n";
}

int main(int argc, char* argv[]) {
    if (argc < 5) {
        usage(argv[0]);
        return 1;
    }

    std::string fontfile = argv[1];
    int size = std::stoi(argv[2]);
    int first = std::stoi(argv[3]); // accepts decimal codepoints
    int last = std::stoi(argv[4]);

    std::string out = (argc >= 6) ? argv[5] : ("font" + std::to_string(size) + "pt7b.h");

    bool ok = convert_font(fontfile, size, first, last, out);
    if (!ok) {
        std::cerr << "Font conversion failed.\n";
        return 2;
    }
    std::cout << "Generated font header: " << out << "\n";
    return 0;
}
