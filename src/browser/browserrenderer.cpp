#include "browser/BrowserRenderer.h"

#include <windows.h>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

BrowserRenderer::BrowserRenderer(){}

BrowserRenderer::~BrowserRenderer()
{
    stop();
}

bool BrowserRenderer::initializeCDP()
{
    std::string response =http_.get(L"127.0.0.1",9222,L"/json/version");
    json version =json::parse(response);
    std::string websocketURL =version["webSocketDebuggerUrl"];
    if(!cdp_.connect(websocketURL))return false;
    if(!cdp_.createPage())return false;
    if(!cdp_.enablePage())return false;
    if(!cdp_.enableRuntime())return false;
    return true;
}

bool BrowserRenderer::start(bool headless)
{
    if(initialized_)return true;
    if(!chrome_.start(headless))return false;
    if(!initializeCDP())return false;
    initialized_ = true;
    return true;
}

void BrowserRenderer::stop()
{
    if(!initialized_)return;
    cdp_.disconnect();
    chrome_.stop();
    initialized_ = false;
}

std::string BrowserRenderer::render(
    const std::string& url)
{
    if(!initialized_)
    {
        if(!start())return "";
    }

    if(!cdp_.navigate(url))return "";
    cdp_.waitForLoad();
    return cdp_.getHTML();
}

