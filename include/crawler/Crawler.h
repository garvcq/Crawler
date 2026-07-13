#pragma once

#include <string>
#include "crawler/Frontier.h"
#include "crawler/SeenStore.h"
#include "network/PageFetcher.h"
#include "parser/HTMLParser.h"
#include "parser/URLParser.h"
#include "parser/URLNormalizer.h"
#include "filter/QuickFilter.h"
#include "filter/LinkFilter.h"

enum class EnqueueResult
{
    Queued,
    Filtered,
    Duplicate,
    Invalid,
    MaxDepth
};

class Crawler
{
public:
    Crawler();
    void addSeed(const std::string& url);
    void crawl();
    void setmaxdepth(int depth);
    void setmaxpages(int pages);
    int getMaxDepth() const;
    int getMaxPages() const;
    int pagesCrawled() const;
    void samedomain(bool enable);
private:
    void crawlPage(const URLDepth& page);
    void processLinks(std::string_view html,const ParsedURL& base,int nextDepth);
    EnqueueResult enqueue(const std::string& url,const ParsedURL& base,int depth);
private:
    Frontier frontier;
    SeenStore seen;
    PageFetcher fetcher;
    HTMLParser htmlparser;
    URLParser parser;
    URLNormalizer normalizer;
    LinkFilter linkfilter;
    int maxdepth =2;
    int maxpages = 1000;
    int crawledpages=0;
    bool sameDomainOnly=false;
    std::string seedHost;
    int httpPages = 0;
    int renderedPages = 0;
    int failedPages = 0;
    int extractedLinks = 0;
    int queuedLinks = 0;
    int filteredLinks = 0;
    int duplicateLinks = 0;
};