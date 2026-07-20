#include "indexer/Tokenizer.h"
#include <cctype>

DynamicArray<std::string> Tokenizer::tokenize(const std::string& text) const
{
    DynamicArray<std::string> tokens;
    std::string current;

    for (size_t i = 0; i < text.length(); ++i)
    {
        char c = text[i];
        if (std::isalnum(static_cast<unsigned char>(c)))
        {
            current += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
        else
        {
            if (!current.empty())
            {
                tokens.append(current);
                current.clear();
            }
        }
    }

    if (!current.empty())
    {
        tokens.append(current);
    }

    return tokens;
}
