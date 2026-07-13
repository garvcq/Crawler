#include "crawler/Crawler.h"

#include <iostream>

Crawler::Crawler()
    : linkfilter(
        "config/blockeddomains.txt",
        "config/blockedextensions.txt")
{
    fetcher.initialize();
}

void Crawler::addSeed(const std::string& url)
{
    ParsedURL parsed = parser.parse(url);

    if(!parsed.valid)
    {
        std::cout << "[Seed] Invalid URL\n";
        return;
    }

    if(seedHost.empty())seedHost = parsed.host;

    NormalizedURL normalized =
        normalizer.normalize(parsed);

    if(!linkfilter.shouldvisit(normalized))
    {
        std::cout << "[Seed] Blocked by LinkFilter\n";
        return;
    }

    std::string finalurl =
        normalizer.tostring(normalized);

    if(seen.tryAdd(finalurl,0))
    {
        frontier.enqueue({finalurl,0});

        std::cout
            << "[Seed] "
            << finalurl
            << '\n';
    }
}

void Crawler::crawl()
{
    std::cout
        << "\n========== CRAWLER START ==========\n\n";
    while(!frontier.empty())
    {
        if(crawledpages >= maxpages)
            break;

        URLDepth current =
            frontier.dequeue();

        crawlPage(current);

        crawledpages++;
    }

    std::cout
        << "\n========== CRAWLER SUMMARY ==========\n";

    std::cout
        << "Pages Crawled : "
        << crawledpages
        << '\n';

    std::cout
        << "HTTP Pages    : "
        << httpPages
        << '\n';

    std::cout
        << "Rendered      : "
        << renderedPages
        << '\n';

    std::cout
        << "Failed        : "
        << failedPages
        << "\n\n";

    std::cout
        << "Links Found   : "
        << extractedLinks
        << '\n';

    std::cout
        << "Queued        : "
        << queuedLinks
        << '\n';

    std::cout
        << "Filtered      : "
        << filteredLinks
        << '\n';

    std::cout
        << "Duplicates    : "
        << duplicateLinks
        << "\n\n";

    std::cout
        << "Seen URLs     : "
        << seen.size()
        << '\n';

    std::cout
        << "Queue Left    : "
        << frontier.size()
        << '\n';

    std::cout
        << "====================================\n";
}

void Crawler::crawlPage(const URLDepth& page)
{
    std::cout
        << "\n------------------------------------\n";

    std::cout
        << "[" << crawledpages + 1 << "]\n\n";

    std::cout
        << "URL      : "
        << page.URL
        << '\n';

    std::cout
        << "Depth    : "
        << page.depth
        << '\n';

    Page fetched =
        fetcher.fetch(page.URL);

    if(fetched.rendered)
        renderedPages++;
    else
        httpPages++;

    std::cout
        << "Status   : "
        << fetched.statusCode
        << '\n';

    std::cout
        << "Source   : "
        << (fetched.rendered ? "CDP" : "HTTP")
        << '\n';

    std::cout
        << "HTML     : "
        << fetched.html.size()
        << " bytes\n";

    if(fetched.html.empty())
    {
        failedPages++;

        seen.updateState(
            page.URL,
            URLState::Failed);

        std::cout
            << "Result   : Failed\n";

        std::cout
            << "------------------------------------\n";

        return;
    }

    seen.updateState(
        page.URL,
        URLState::Completed);

    ParsedURL base =
        parser.parse(page.URL);

    processLinks(
        fetched.html,
        base,
        page.depth + 1);

    std::cout
        << "Frontier : "
        << frontier.size()
        << '\n';

    std::cout
        << "Seen     : "
        << seen.size()
        << '\n';

    std::cout
        << "------------------------------------\n";
}
void Crawler::processLinks(
    std::string_view html,
    const ParsedURL& base,
    int nextDepth)
{
    auto links = htmlparser.extractlinks(html);

    extractedLinks += links.size();

    int queued = 0;
    int filtered = 0;
    int duplicate = 0;
    int invalid = 0;
    int maxDepthReached = 0;

    for(int i = 0; i < links.size(); i++)
    {
        EnqueueResult result =
            enqueue(
                links[i],
                base,
                nextDepth);

        switch(result)
        {
            case EnqueueResult::Queued:
                queued++;
                queuedLinks++;
                break;

            case EnqueueResult::Filtered:
                filtered++;
                filteredLinks++;
                break;

            case EnqueueResult::Duplicate:
                duplicate++;
                duplicateLinks++;
                break;

            case EnqueueResult::Invalid:
                invalid++;
                break;

            case EnqueueResult::MaxDepth:
                maxDepthReached++;
                break;
        }
    }

    std::cout
        << "\nLinks Found           : " << links.size()
        << "\nQueued into Frontier  : " << queued
        << "\nFiltered these links  : " << filtered
        << "\nDuplicate links       : " << duplicate
        << "\nInvalid links         : " << invalid
        << "\nMax Depth Reached     : " << maxDepthReached
        << "\n";
}

EnqueueResult Crawler::enqueue(
    const std::string& url,
    const ParsedURL& base,
    int depth)
{
    if(depth > maxdepth)
        return EnqueueResult::MaxDepth;

    if(!QuickFilter::shouldparse(url))
        return EnqueueResult::Filtered;

    ParsedURL parsed = parser.parse(url);

    NormalizedURL normalized;

    if(parsed.valid)
    {
        normalized =
            normalizer.normalize(parsed);
    }
    else if(QuickFilter::isrelative(url))
    {
        normalized =
            normalizer.resolverelative(
                url,
                base);
    }
    else
    {
        return EnqueueResult::Invalid;
    }

    // Stay on same domain (optional)
    if(sameDomainOnly)
    {
        if(normalized.host != seedHost)
            return EnqueueResult::Filtered;
    }

    if(!linkfilter.shouldvisit(normalized))
        return EnqueueResult::Filtered;

    std::string finalurl =
        normalizer.tostring(normalized);

    if(!seen.tryAdd(finalurl, depth))
        return EnqueueResult::Duplicate;

    frontier.enqueue(
    {
        finalurl,
        depth
    });

    return EnqueueResult::Queued;
}

void Crawler::samedomain(bool enable)
{
    sameDomainOnly = enable;
}

void Crawler::setmaxdepth(int depth)
{
    if(depth >= 0)maxdepth = depth;
}

void Crawler::setmaxpages(int pages)
{
    if(pages > 0)maxpages = pages;
}