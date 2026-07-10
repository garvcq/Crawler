#pragma once
#include <string>
#include "browser/ChromeProcess.h"
#include "browser/HTTPClient.h"
#include "browser/CDPconn.h"

class BrowserRenderer
{
public:
    BrowserRenderer();
    ~BrowserRenderer();
    bool start(bool headless = true);
    void stop();
    std::string render(const std::string& url);
private:
    ChromeProcess chrome_;
    HTTPClient http_;
    CDPConnection cdp_;
    bool initialized_ = false;
    bool initializeCDP();
};