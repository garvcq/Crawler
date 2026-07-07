#include "CDPConn.h"

using json = nlohmann::json;

bool CDPConnection::connect(const std::string& url)
{
    return websocket_.connect(url);
}

void CDPConnection::disconnect()
{
    websocket_.disconnect();
}

json CDPConnection::sendCommand(
    const std::string& method,
    const json& params)
{
    json request;

    request["id"] = nextId_++;
    request["method"] = method;

    if (!sessionId_.empty())
        request["sessionId"] = sessionId_;

    if (!params.empty())
        request["params"] = params;

    websocket_.send(request.dump());

    while (true)
    {
        json reply = json::parse(websocket_.receive());

        // Ignore events.
        if (!reply.contains("id"))
            continue;

        if (reply["id"] == request["id"])
            return reply;
    }
}

bool CDPConnection::createPage()
{
    auto reply =
        sendCommand(
            "Target.createTarget",
            {
                {"url", "about:blank"}
            });

    if (!reply.contains("result"))
        return false;

    std::string targetId =
        reply["result"]["targetId"];

    reply =
        sendCommand(
            "Target.attachToTarget",
            {
                {"targetId", targetId},
                {"flatten", true}
            });

    if (!reply.contains("result"))
        return false;

    sessionId_ =
        reply["result"]["sessionId"];

    return true;
}

bool CDPConnection::enablePage()
{
    auto reply =
        sendCommand("Page.enable");

    return reply.contains("result");
}

bool CDPConnection::enableRuntime()
{
    auto reply =
        sendCommand("Runtime.enable");

    return reply.contains("result");
}

bool CDPConnection::navigate(const std::string& url)
{
    auto reply =
        sendCommand(
            "Page.navigate",
            {
                {"url", url}
            });

    return reply.contains("result");
}

std::string CDPConnection::getHTML()
{
    auto reply =
        sendCommand(
            "Runtime.evaluate",
            {
                {
                    "expression",
                    "document.documentElement.outerHTML"
                },
                {
                    "returnByValue",
                    true
                }
            });

    return reply["result"]
                ["result"]
                ["value"];
}