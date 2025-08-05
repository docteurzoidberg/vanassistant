#pragma once

#include <string>

// Convertit la police et écrit un header Adafruit GFX dans output_header.
// first_char et last_char sont code points (ASCII typiquement, ex 32..126).
// Retourne vrai si succès.
bool convert_font(const std::string& font_path,
                  int size_pt,
                  int first_char,
                  int last_char,
                  const std::string& output_header,
                  bool include_unicode = false);
