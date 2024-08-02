#pragma once

#include "../../../ressources/asmtext.h"

#include <cstring>
#include <string>
#include <vector>

extern const char asm_text_buffer[];

class AsmText {
public:
  AsmText() {
    //count lines from asm buffer
    lineIndex = 0;
    lineCount = 0;
    linepos.push_back(0);
    for (int i = 0; i < STRLEN_P(asm_text_buffer); i++) {
      if (READ_BYTE(asm_text_buffer + i) == '\n') {
        //store index into line positions vector
        linepos.push_back(i);
        lineCount++;
      }
    }
  }
  std::string GetLine() {
    //return the current line at lineIndex and increment lineIndex
    std::string line;

    auto lineStartIndex = linepos[lineIndex];

    //find end of line
    auto lineEndIndex = lineIndex < lineCount - 1 ? linepos[lineIndex + 1] : STRLEN_P(&asm_text_buffer[lineStartIndex]);

    //copy line to string
    int j = 0;
    for (int i = lineStartIndex; i < lineEndIndex; i++) {
      char character = READ_BYTE(asm_text_buffer + i);
      if (character == '\n' || character == '\r') {
        //dont copy newline characters
      } else if (character == '\t') {
        line += "    ";
        j += 4;
      } else if (j > 0 && character == ';') {
        //skip comments
        break;
      } else {
        line += character;
        j++;
      }
    }
    lineIndex++;
    if(lineIndex >= lineCount) {
      lineIndex = 0;
    }
    return line;
  }
private:
  int lineCount;
  int lineIndex;
  std::vector<unsigned int> linepos;
};