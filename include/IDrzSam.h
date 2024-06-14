#pragma once

#include <string>

class IDrzSam
{
public:
    virtual ~IDrzSam() {}

    virtual bool Say(std::string text) = 0;
};