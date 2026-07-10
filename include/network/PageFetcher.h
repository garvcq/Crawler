#pragma once
#include <string>
#include "network/Page.h"
#include "network/HTTPFetcher.h"
#include "browser/BrowserRenderer.h"
#include "parser/HTMLParser.h"

class PageFetcher
{
    public:
        Page fetch(const std::string& url);
    private:
        HTTPFetcher http_;
        BrowserRenderer browser_;
        HTMLParser parser_;
        bool needsRendering(const Page& page);
};