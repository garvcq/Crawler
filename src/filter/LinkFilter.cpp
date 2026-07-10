#include "filter/LinkFilter.h"
#include <fstream>
#include <cctype>

LinkFilter::LinkFilter(const std::string& domainfile,const std::string& extensionfile)
{
    loadfile(domainfile,blockedDomain);
    loadfile(extensionfile,blockedExtension);
}

void LinkFilter::trim (std::string& str)
{
    while(!str.empty()&&std::isspace(static_cast<unsigned char>(str.front())))
        str.erase(str.begin());
        
    while(!str.empty()&&std::isspace(static_cast<unsigned char>(str.back())))
        str.pop_back();
}   

void LinkFilter::loadfile(const std::string& filename,HashMap<std::string,bool>& table)
{
    std::ifstream file(filename);
    if(!file.is_open())return;
    std::string line;
    while(std::getline(file,line))
    {
        trim(line);
        if(line.empty())continue;
        if(line[0]=='#')continue;
        for(char& c :line)
        {
            c=static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
        table.insert(line,true);
    }
}

bool LinkFilter::hasblockeddomain(const NormalizedURL& url)const
{
    return blockedDomain.exists(url.host);
}

bool LinkFilter::hasblockedextension(const NormalizedURL& url)const
{
    const std::string& path = url.path;
    std::size_t slash = path.find_last_of('/');
    std::size_t dot = path.find_last_of('.');
    if(dot == std::string::npos)return false;
    if(slash!=std::string::npos && dot<slash)
    return false;
    std::size_t end = path.find_first_of("?#",dot);
    if(end == std::string::npos)end = path.length();
    std::string extension = path.substr(dot+1,end-dot-1);
    for(char& c :extension)
    {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return blockedExtension.exists(extension);
}

bool LinkFilter::hasblockedscheme(const NormalizedURL& url)const
{
    return url.scheme == "mailto"||url.scheme=="javascript"||url.scheme=="tel"||url.scheme=="data"||url.scheme=="blob"||url.scheme=="ftp"||url.scheme=="file";
}

bool LinkFilter::shouldvisit(const NormalizedURL& url)const{
    if(hasblockedscheme(url))return false;
    if(hasblockeddomain(url))return false;
    if(hasblockedextension(url))return false;
    return true;
}