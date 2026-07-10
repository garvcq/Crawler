#include "browser/WebSocketConn.h"

#include <boost/beast/core/buffers_to_string.hpp>

#include <stdexcept>

using tcp = boost::asio::ip::tcp;
namespace websocket = boost::beast::websocket;

WebSocketConnection::WebSocketConnection(): resolver_(io_),websocket_(io_),connected_(false){}
WebSocketConnection::~WebSocketConnection()
{
    disconnect();
}
WebSocketConnection::Endpoint
WebSocketConnection::parseURL(const std::string& url)
{
    const std::string prefix = "ws://";
    if (url.rfind(prefix, 0) != 0)throw std::runtime_error("Invalid websocket URL.");
    std::string remaining = url.substr(prefix.size());
    size_t slash = remaining.find('/');
    if (slash == std::string::npos)throw std::runtime_error("Invalid websocket URL.");
    std::string hostPort = remaining.substr(0, slash);
    Endpoint endpoint;
    endpoint.target = remaining.substr(slash);
    size_t colon = hostPort.find(':');
    if (colon == std::string::npos)
    {
        endpoint.host = hostPort;
        endpoint.port = "80";
    }
    else
    {
        endpoint.host = hostPort.substr(0, colon);
        endpoint.port = hostPort.substr(colon + 1);
    }
    return endpoint;
}

bool WebSocketConnection::connect(const std::string& url)
{
    if (connected_)return true;
    Endpoint endpoint = parseURL(url);
    auto results = resolver_.resolve(endpoint.host, endpoint.port);
    boost::asio::connect(websocket_.next_layer(),results.begin(),results.end());
    websocket_.handshake(endpoint.host +":"+ endpoint.port,endpoint.target);
    connected_ = true;
    return true;
}

bool WebSocketConnection::send(const std::string& message)
{
    if (!connected_)return false;
    websocket_.write(boost::asio::buffer(message));
    return true;
}

std::string WebSocketConnection::receive()
{
    if (!connected_)return "";
    buffer_.clear();
    websocket_.read(buffer_);
    return boost::beast::buffers_to_string(buffer_.data());
}

void WebSocketConnection::disconnect()
{
    if (!connected_)return;
    boost::system::error_code ec;
    websocket_.close(websocket::close_code::normal,ec);
    connected_ = false;
}

bool WebSocketConnection::isConnected() const
{
    return connected_;
}