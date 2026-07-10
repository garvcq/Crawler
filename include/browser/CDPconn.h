#pragma once

#include "WebSocketConn.h"

#include <nlohmann/json.hpp>

#include <string>

class CDPConnection
{
public:
    bool connect(const std::string& websocketURL);
    void disconnect();
    bool createPage();
    bool enablePage();
    bool enableRuntime();
    bool waitForLoad();
    bool navigate(const std::string& url);
    std::string getHTML();
private:
    nlohmann::json sendCommand(const std::string& method,const nlohmann::json& params = {});
private:
    int nextId_ = 1;
    std::string sessionId_;
    WebSocketConnection websocket_;
};