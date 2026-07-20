#include "indexer/TextExtractor.h"
#include <cctype>

std::string TextExtractor::stripTags(std::string_view html) const
{
    // Check if the html body is a JSON response which starts either with '{' or '[' 
    size_t checkPos = 0;
    while (checkPos < html.length() && std::isspace(static_cast<unsigned char>(html[checkPos])))
    {
        checkPos++;
    }
    if (checkPos < html.length() && (html[checkPos] == '{' || html[checkPos] == '['))
    {
        return "";
    }
    std::string text;
    bool inTag = false;
    bool inScriptOrStyle = false;
    std::string currentTagName;
    bool finishedTagName = false;

    for (size_t i = 0; i < html.length(); ++i)
    {
        char c = html[i];
        if (c == '<')
        {
            inTag = true;
            currentTagName.clear();
            finishedTagName = false;
            continue;
        }
        else if (c == '>')
        {
            inTag = false;
            // Check for entry or exit of non-visible blocks
            if (currentTagName == "script" || currentTagName == "style" ||
                currentTagName == "SCRIPT" || currentTagName == "STYLE")
            {
                inScriptOrStyle = true;
            }
            else if (currentTagName == "/script" || currentTagName == "/style" ||
                     currentTagName == "/SCRIPT" || currentTagName == "/STYLE")
            {
                inScriptOrStyle = false;
            }
            currentTagName.clear();
            continue;
        }

        if (inTag)
        {
            if (!finishedTagName)
            {
                if (std::isspace(static_cast<unsigned char>(c)) || c == '>')
                {
                    finishedTagName = true;
                }
                else if (c == '/')
                {
                    if (currentTagName.empty())
                    {
                        currentTagName += c;
                    }
                    else
                    {
                        finishedTagName = true;
                    }
                }
                else
                {
                    currentTagName += c;
                }
            }
        }
        else
        {
            if (!inScriptOrStyle)
            {
                if (c == '\n' || c == '\r' || c == '\t')
                {
                    c = ' ';
                }
                
                if (std::isspace(static_cast<unsigned char>(c)))
                {
                    if (!text.empty() && text.back() != ' ')
                    {
                        text += ' ';
                    }
                }
                else
                {
                    text += c;
                }
            }
        }
    }

    // Trim beginning and end
    while (!text.empty() && text.front() == ' ') text.erase(0, 1);
    while (!text.empty() && text.back() == ' ') text.pop_back();
    return decodeEntities(text);
}

std::string TextExtractor::decodeEntities(std::string text) const
{
    struct EntityReplace {
        std::string entity;
        std::string replacement;
    };

    EntityReplace replacements[] = {
        {"&amp;", "&"},
        {"&lt;", "<"},
        {"&gt;", ">"},
        {"&quot;", "\""},
        {"&apos;", "'"},
        {"&#039;", "'"},
        {"&#39;", "'"},
        {"&nbsp;", " "},
        {"&#160;", " "},
        {"&hellip;", "..."},
        {"&#8230;", "..."},
        {"&ldquo;", "\""},
        {"&rdquo;", "\""},
        {"&#8220;", "\""},
        {"&#8221;", "\""},
        {"&lsquo;", "'"},
        {"&rsquo;", "'"},
        {"&#8216;", "'"},
        {"&#8217;", "'"},
        {"&middot;", "-"},
        {"&#183;", "-"},
        {"&ndash;", "-"},
        {"&#8211;", "-"},
        {"&mdash;", "--"},
        {"&#8212;", "--"},
        {"&copy;", "(c)"},
        {"&#169;", "(c)"},
        {"\xE2\x80\xA6", "..."},
        {"\xC2\xA9", "(c)"},
        {"\xEF\xB8\x8F", ""},
        {"\xC2\xA0", " "},
        {"\xE2\x80\x9C", "\""},
        {"\xE2\x80\x9D", "\""},
        {"\xE2\x80\x98", "'"},
        {"\xE2\x80\x99", "'"},
        {"\xC2\xB7", "-"},
        {"\xE2\x80\x93", "-"},
        {"\xE2\x80\x94", "--"}
    };

    for (const auto& rep : replacements)
    {
        size_t pos = 0;
        while ((pos = text.find(rep.entity, pos)) != std::string::npos)
        {
            text.replace(pos, rep.entity.length(), rep.replacement);
            pos += rep.replacement.length();
        }
    }

    return text;
}

std::string TextExtractor::extractTitle(std::string_view html) const
{
    size_t start = html.find("<title>");
    if (start == std::string_view::npos)
    {
        start = html.find("<TITLE>");
    }
    if (start == std::string_view::npos) return "";

    size_t end = html.find("</title>", start);
    if (end == std::string_view::npos)
    {
        end = html.find("</TITLE>", start);
    }
    if (end == std::string_view::npos) return "";

    size_t titleStart = start + 7;
    std::string rawTitle(html.substr(titleStart, end - titleStart));
    return decodeEntities(rawTitle);
}
