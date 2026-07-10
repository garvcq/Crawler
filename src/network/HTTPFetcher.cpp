#include "network/HTTPFetcher.h"
#include <curl/curl.h>

namespace
{
size_t writeCallback(void* contents,size_t size,size_t nmemb,void* userp)
{
    std::string* html = static_cast<std::string*>(userp);
    html->append(static_cast<char*>(contents),size * nmemb);
    return size * nmemb;
}
}

Page HTTPFetcher::fetch(const std::string& url)
{
    Page page;
    page.url = url;
    CURL* curl = curl_easy_init();
    if(!curl)return page;
    curl_easy_setopt(curl,CURLOPT_URL,url.c_str());
    curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1L);
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,writeCallback);
    curl_easy_setopt(curl,CURLOPT_WRITEDATA,&page.html);
    //fix for behaving like browser
    curl_easy_setopt(curl,CURLOPT_USERAGENT,
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) "
    "AppleWebKit/537.36 "
    "(KHTML, like Gecko) "
    "Chrome/137.0.0.0 Safari/537.36");
    CURLcode result =curl_easy_perform(curl);
    if(result == CURLE_OK)
    {
        long status = 0;
        curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&status);
        page.statusCode=static_cast<int>(status);
    }
    curl_easy_cleanup(curl);
    return page;
}