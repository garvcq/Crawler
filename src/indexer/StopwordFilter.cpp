#include "indexer/StopwordFilter.h"
#include <fstream>
#include <iostream>
#include <cctype>

bool StopwordFilter::loadStopwords(const std::string& filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        std::cerr << "[StopwordFilter] Failed to open: " << filepath << "\n";
        return false;
    }
    std::string line;
    while (std::getline(file, line))
    {
        // Trim whitespace
        while (!line.empty() && std::isspace(static_cast<unsigned char>(line.front()))) line.erase(0, 1);
        while (!line.empty() && std::isspace(static_cast<unsigned char>(line.back()))) line.pop_back();

        if (!line.empty())
        {
            stopwords_.insert(line, true);
        }
    }
    return true;
}

bool StopwordFilter::isStopword(const std::string& token) const
{
    return stopwords_.exists(token);
}
