#include "filter/QuickFilter.h"


bool QuickFilter::iswhitespace(char c)
{
    return c==' '||c=='\t'||c=='\n'||c=='\r';
}

bool QuickFilter::shouldparse(std::string_view url)
{
    //remove leading whitespace
    while(!url.empty()&&iswhitespace(url.front()))
        url.remove_prefix(1);

    //remove trailing whitespace
    while(!url.empty()&&iswhitespace(url.back()))
        url.remove_suffix(1);
    
    if(url.empty())return false;

    if(url.front()=='#')return false;

    return true;
}