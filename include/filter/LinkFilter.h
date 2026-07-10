#pragma once

#include <string>
#include "HashMap.h"
#include "parser/NormalizedURL.h"
class LinkFilter
{
    public:
        bool shouldvisit(const NormalizedURL& url)const;
        LinkFilter(const std::string& domainfile="config/blockeddomains.txt",const std::string& extensionfile="config/blockedextensions.txt");

    private:
        HashMap<std::string,bool> blockedDomain;
        HashMap<std::string,bool> blockedExtension;
        void loadfile(const std::string& filename,HashMap<std::string,bool>& table);
        static void trim(std::string& str);
        bool hasblockeddomain(const NormalizedURL&)const;
        bool hasblockedextension(const NormalizedURL&)const;
        bool hasblockedscheme(const NormalizedURL&)const;
};