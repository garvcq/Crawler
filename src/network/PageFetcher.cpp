#include "network/PageFetcher.h"

Page PageFetcher::fetch(const std::string& url)
{
    Page page =http_.fetch(url);
    if(needsRendering(page))
    {
        page.html=browser_.render(url);
        page.rendered = true;
    }
    return page;
}

bool PageFetcher::needsRendering(const Page& page)
{
    if(page.statusCode != 200)return false;
    if(page.html.empty())return true;
    auto links=parser_.extractlinks(page.html);
    if(links.size() == 0)return true;
    return false;
}