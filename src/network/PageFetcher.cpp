#include "network/PageFetcher.h"

Page PageFetcher::fetch(const std::string& url)
{
    Page page =http_.fetch(url);
    if(needsRendering(page))
    {
        page.html=browser_.render(url);
        page.rendered = true;

        // If rendering succeeded, treat it as a successful fetch.
        if(!page.html.empty())page.statusCode = 200;
    }
    return page;
}

void PageFetcher::initialize()
{
    browser_.start(false);   // headful (headless = false)
}

bool PageFetcher::needsRendering(const Page& page)
{
    // HTTP failed
    if(page.statusCode != 200)return false;

    // No HTML
    if(page.html.empty())return true;

    // Very small page
    if(page.html.size() < 300)return true;

     // These won't be fixed by rendering.
    if(page.statusCode == 401)
        return false;

    if(page.statusCode == 403)
        return false;

    if(page.statusCode == 404)
        return false;

    if(page.statusCode >= 500)
        return false;

    // JS shell
    if(page.html.find("id=\"root\"") != std::string::npos)return true;

    if(page.html.find("id=\"app\"") != std::string::npos)return true;

    if(page.html.find("id=\"__next\"") != std::string::npos)return true;

    if(page.html.find("id=\"__nuxt\"") != std::string::npos)return true;
    auto links = parser_.extractlinks(page.html);
    if(links.size() == 0)
        return true;
        
    return false;
}