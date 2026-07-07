#include "ChromeProcess.h"
#include "HTTPclient.h"
#include "WebSocketConn.h"
#include "CDPconn.h"
#include <iostream>
#include <nlohmann/json.hpp>

int main()
{
ChromeProcess chrome;
chrome.start();
HTTPClient http;
std::string response =
    http.get(
        L"127.0.0.1",
        9222,
        L"/json/version");
auto versionInfo =
    nlohmann::json::parse(response);
std::string websocketURL =
    versionInfo["webSocketDebuggerUrl"];
CDPConnection cdp;
cdp.connect(websocketURL);
if (!cdp.createPage())
{
    std::cout << "Failed to create page\n";
    return 1;
}

std::cout << "Page created\n";

cdp.enablePage();

cdp.enableRuntime();

cdp.navigate("https://codequotient.com");
Sleep(3000);
std::string html =
    cdp.getHTML();

std::cout << html << std::endl;
cdp.navigate("https://ssipmt.edu.in");
Sleep(3000);
 html =
    cdp.getHTML();

std::cout << html << std::endl;
cdp.navigate("https://youtube.in");
Sleep(3000);
 html =
    cdp.getHTML();

std::cout << html << std::endl;
cdp.disconnect();
chrome.stop();
}