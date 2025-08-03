#include <algorithm>
#include <png.h>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>

void abort_(const char *s) {
    std::cerr << "libpng error: " << s << std::endl;
    std::exit(EXIT_FAILURE);
}

void read_png_file(const std::string& filename, int& width, int& height, std::vector<unsigned char>& out_data) {
    FILE *fp = fopen(filename.c_str(), "rb");
    if (!fp) abort_("Cannot open PNG file");

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png) abort_("png_create_read_struct failed");

    png_infop info = png_create_info_struct(png);
    if (!info) abort_("png_create_info_struct failed");

    if (setjmp(png_jmpbuf(png))) abort_("Error during init_io");

    png_init_io(png, fp);
    png_read_info(png, info);

    width  = png_get_image_width(png, info);
    height = png_get_image_height(png, info);
    png_byte color_type = png_get_color_type(png, info);
    png_byte bit_depth  = png_get_bit_depth(png, info);

    if (bit_depth == 16) png_set_strip_16(png);
    if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png);
    if (png_get_valid(png, info, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png);

    if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
    }
    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
        png_set_gray_to_rgb(png);
    }

    png_read_update_info(png, info);

    std::vector<png_bytep> row_pointers(height);
    out_data.resize(width * height * 4);
    for (int y = 0; y < height; y++) {
        row_pointers[y] = &out_data[y * width * 4];
    }

    png_read_image(png, row_pointers.data());
    fclose(fp);
    png_destroy_read_struct(&png, &info, nullptr);
}

void write_header(const std::string& output_file, const std::string& png_file, int width, int height, const std::vector<unsigned char>& data) {
    std::ofstream ofs(output_file);
    if (!ofs) {
        std::cerr << "Cannot open output file\n";
        return;
    }

    std::string sprite_name = std::filesystem::path(png_file).stem().string();
    std::transform(sprite_name.begin(), sprite_name.end(), sprite_name.begin(), ::toupper);

    std::string guard = sprite_name + "_SPRITE_H";
    ofs << "#ifndef " << guard << "\n";
    ofs << "#define " << guard << "\n\n";
    ofs << "// Generated sprite data for " << sprite_name << "\n";
    ofs << "const int " << sprite_name << "_SPRITE_WIDTH = " << width << ";\n";
    ofs << "const int " << sprite_name << "_SPRITE_HEIGHT = " << height << ";\n";
    ofs << "const unsigned int " << sprite_name << "_SPRITE_DATA[] = {\n";

    for (size_t i = 0; i < data.size(); i += 4) {
        ofs << "    0x"
            << std::hex << std::uppercase
            << std::setw(2) << std::setfill('0') << static_cast<int>(data[i])
            << std::setw(2) << std::setfill('0') << static_cast<int>(data[i+1])
            << std::setw(2) << std::setfill('0') << static_cast<int>(data[i+2])
            << std::setw(2) << std::setfill('0') << static_cast<int>(data[i+3])
            << ",\n";
    }

    ofs << "};\n\n#endif //" << guard << "\n";
    std::cout << "Header file '" << output_file << "' generated successfully.\n";
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input.png> <output.h>\n";
        return 1;
    }

    int width = 0, height = 0;
    std::vector<unsigned char> img_data;

    read_png_file(argv[1], width, height, img_data);
    write_header(argv[2], argv[1], width, height, img_data);

    return 0;
}
