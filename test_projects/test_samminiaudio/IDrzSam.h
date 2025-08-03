#pragma once

#include <string>

namespace drz {

/**
 * @brief IDrzSam: interface for DrzSam platform libraries
 * 
 */
class IDrzSam
{
  public:
    virtual bool Say(std::string text) = 0;
};

} // namespace

