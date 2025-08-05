#include "fontconvert.hpp"

#include <cstdint>
#include <ft2build.h>

#include FT_FREETYPE_H

#include <algorithm>
#include <cctype>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

// Adafruit GFX structures (must match expected layout)
struct GFXglyph {
  uint32_t bitmapOffset; ///< Pointer into bitmap array
  uint8_t width;         ///< Bitmap dimensions
  uint8_t height;
  uint8_t xAdvance; ///< Distance to advance cursor
  int8_t xOffset;   ///< X offset from cursor to UL corner
  int8_t yOffset;   ///< Y offset from cursor to UL corner
};

struct GFXfontHeader {
  // This is just for human readability; the real header is emitted as C
};

static std::string sanitize_name(const std::string &in) {
  std::string out;
  for (char c : in) {
    if (std::isalnum(static_cast<unsigned char>(c)))
      out += c;
    else
      out += '_';
  }
  return out;
}

bool convert_font(const std::string &font_path, int size_pt, int first_char,
                  int last_char, const std::string &output_header,
                  bool /*include_unicode*/) {
  FT_Library library;
  if (FT_Init_FreeType(&library)) {
    std::cerr << "ERROR: Could not initialize FreeType library\n";
    return false;
  }

  FT_Face face;
  if (FT_New_Face(library, font_path.c_str(), 0, &face)) {
    std::cerr << "ERROR: Could not open font: " << font_path << "\n";
    FT_Done_FreeType(library);
    return false;
  }

  // Adafruit assumes 141 DPI (as in original) to map point size to pixel
  // height.
  const int DPI = 141;
  FT_Set_Char_Size(face, 0, size_pt * 64, DPI, DPI); // size in 1/64th points

  // Collect per-glyph bitmap and metrics
  std::vector<uint8_t> bitmap_data;
  struct GlyphMeta {
    GFXglyph glyph;
    std::vector<uint8_t> bitmap; // padded per glyph then concatenated
  };
  std::vector<GlyphMeta> glyphs;
  int max_above_baseline = 0;
  int max_below_baseline = 0;

  for (int c = first_char; c <= last_char; ++c) {
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      std::cerr << "Warning: Could not load character code " << c << "\n";
      // create empty placeholder
      GFXglyph g{};
      g.bitmapOffset = 0;
      g.width = 0;
      g.height = 0;
      g.xAdvance = 0;
      g.xOffset = 0;
      g.yOffset = 0;
      glyphs.push_back({g, {}});
      continue;
    }
    FT_GlyphSlot slot = face->glyph;
    FT_Bitmap &bmp = slot->bitmap;

    // Copy bitmap row-wise; Adafruit expects 1 bit per pixel packed MSB first
    // We'll convert the grayscale bitmap to 1-bit (threshold at >128)
    int w = bmp.width;
    int h = bmp.rows;

    int bytes_per_row = (w + 7) / 8;
    std::vector<uint8_t> glyph_bitmap(bytes_per_row * h, 0);

    for (int row = 0; row < h; ++row) {
      for (int col = 0; col < w; ++col) {
        unsigned char pixel = 0;
        if (bmp.buffer)
          pixel = bmp.buffer[row * bmp.pitch + col];
        bool set = pixel > 128; // threshold
        if (set) {
          int bit_index = col;
          int byte_index = row * bytes_per_row + (bit_index / 8);
          glyph_bitmap[byte_index] |= (0x80 >> (bit_index % 8));
        }
      }
    }

    GFXglyph g{};
    g.bitmapOffset = 0; // to be filled when concatenating
    g.width = static_cast<uint8_t>(w);
    g.height = static_cast<uint8_t>(h);
    g.xAdvance = static_cast<uint8_t>((slot->advance.x + 32) >> 6); // round
    g.xOffset = static_cast<int8_t>(slot->bitmap_left);
    g.yOffset = static_cast<int8_t>(
        -(slot->bitmap_top - (face->size->metrics.ascender >> 6)));

    int above = slot->bitmap_top;
    int below = h - slot->bitmap_top;
    max_above_baseline = std::max(max_above_baseline, above);
    max_below_baseline = std::max(max_below_baseline, below);

    glyphs.push_back({g, std::move(glyph_bitmap)});
  }

  // Concatenate bitmap and fix offsets
  uint32_t cursor = 0;
  for (auto &gm : glyphs) {
    gm.glyph.bitmapOffset = cursor;
    bitmap_data.insert(bitmap_data.end(), gm.bitmap.begin(), gm.bitmap.end());
    cursor += static_cast<uint32_t>(gm.bitmap.size());
  }

  int yAdvance = (face->size->metrics.height + 32) >> 6; // round

  std::string base = font_path;
  auto slash = base.find_last_of("/\\");
  if (slash != std::string::npos)
    base = base.substr(slash + 1);
  auto dot = base.find_last_of('.');
  if (dot != std::string::npos)
    base = base.substr(0, dot);
  std::ostringstream struct_name_ss;
  struct_name_ss << sanitize_name(base) << size_pt << "pt7b";
  std::string font_struct_name = struct_name_ss.str();
  std::string bitmap_array_name = font_struct_name + "Bitmaps";
  std::string glyph_array_name = font_struct_name + "Glyphs";
  std::string font_name = font_struct_name;

  std::ofstream out(output_header);
  if (!out) {
    std::cerr << "ERROR: Cannot open output file for writing: " << output_header
              << "\n";
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    return false;
  }

  out << "#ifndef " << font_struct_name << "_H\n";
  out << "#define " << font_struct_name << "_H\n\n";
  out << "#include <stdint.h>\n\n";
  out << "// Generated from: " << font_path << " size " << size_pt << "pt\n\n";

  out << "const uint8_t " << bitmap_array_name << "[] = {\n    ";
  size_t cnt = 0;
  std::ios oldState(nullptr);
  oldState.copyfmt(out);
  out << std::hex << std::setfill('0');
  for (size_t i = 0; i < bitmap_data.size(); ++i) {
    out << "0x" << std::setw(2) << static_cast<int>(bitmap_data[i]);
    if (i + 1 != bitmap_data.size())
      out << ", ";
    if (++cnt % 12 == 0)
      out << "\n    ";
  }
  out << std::dec << "\n};\n\n";
  out.copyfmt(oldState);

  out << "const GFXglyph " << glyph_array_name << "[] = {\n";
  int idx = 0;
  for (int c = first_char; c <= last_char; ++c, ++idx) {
    const GFXglyph &g = glyphs[idx].glyph;
    out << "  { " << g.bitmapOffset << ", " << int(g.width) << ", "
        << int(g.height) << ", " << int(g.xAdvance) << ", " << int(g.xOffset)
        << ", " << int(g.yOffset) << " }";
    out << " // 0x" << std::hex << std::uppercase << c << std::nouppercase
        << std::dec;
    if (c + 1 <= last_char)
      out << ",";
    out << "\n";
  }
  out << "};\n\n";

  out << "const GFXfont " << font_name << " = {\n";
  out << "  (uint8_t  *)" << bitmap_array_name << ",\n";
  out << "  (GFXglyph *)" << glyph_array_name << ",\n";
  out << "  0x" << std::hex << first_char << ", 0x" << last_char << ",  "
      << std::dec << yAdvance << "\n";
  out << "};\n\n";

  out << "#endif // " << font_struct_name << "_H\n";

  FT_Done_Face(face);
  FT_Done_FreeType(library);
  return true;
}
