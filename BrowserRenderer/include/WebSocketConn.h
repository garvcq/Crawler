#pragma once

#include <string>

#include <winsock2.h>
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

class WebSocketConnection
{
public:
    WebSocketConnection();
    ~WebSocketConnection();
    bool connect(const std::string& url);
    bool send(const std::string& message);
    std::string receive();
    void disconnect();
    bool isConnected() const;
private:
    struct Endpoint
    {
        std::string host;
        std::string port;
        std::string target;
    };
    Endpoint parseURL(const std::string& url);
private:
    boost::asio::io_context io_;
    boost::asio::ip::tcp::resolver resolver_;
    boost::beast::websocket::stream<boost::asio::ip::tcp::socket> websocket_;
    boost::beast::flat_buffer buffer_;
    bool connected_;
};