#pragma once

#include <string>
#include <string_view>

class TextExtractor
{
public:
    TextExtractor() = default;

    // Strips HTML tags, script segments, style segments and returns visible text
    std::string stripTags(std::string_view html) const;

    // Decodes HTML entities and common Unicode characters
    std::string decodeEntities(std::string text) const;

    // Extracts the title from <title> tags in HTML source
    std::string extractTitle(std::string_view html) const;
};
