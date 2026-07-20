#pragma once

#include <string>
#include "DynamicArray.h"

class Tokenizer
{
public:
    Tokenizer() = default;

    // Splits lowercased alphanumeric tokens from text
    DynamicArray<std::string> tokenize(const std::string& text) const;
};
