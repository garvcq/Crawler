#include "browser/HTTPClient.h"

#include <windows.h>
#include <winhttp.h>

#include <string>
#include <stdexcept>

#pragma comment(lib, "winhttp.lib")

std::string HTTPClient::get(const std::wstring& host,
                            int port,
                            const std::wstring& path)
{
    HINTERNET session =
        WinHttpOpen(
            L"CDP Crawler/1.0",
            WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
            WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS,
            0);

    if (!session)
        throw std::runtime_error("Failed to open WinHTTP session.");

    HINTERNET connection =
        WinHttpConnect(
            session,
            host.c_str(),
            port,
            0);

    if (!connection)
    {
        WinHttpCloseHandle(session);
        throw std::runtime_error("Failed to connect.");
    }

    HINTERNET request =
        WinHttpOpenRequest(
            connection,
            L"GET",
            path.c_str(),
            nullptr,
            WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            0);

    if (!request)
    {
        WinHttpCloseHandle(connection);
        WinHttpCloseHandle(session);
        throw std::runtime_error("Failed to create request.");
    }

    BOOL ok =
        WinHttpSendRequest(
            request,
            WINHTTP_NO_ADDITIONAL_HEADERS,
            0,
            WINHTTP_NO_REQUEST_DATA,
            0,
            0,
            0);

    if (!ok)
    {
        WinHttpCloseHandle(request);
        WinHttpCloseHandle(connection);
        WinHttpCloseHandle(session);
        throw std::runtime_error("Send failed.");
    }

    ok = WinHttpReceiveResponse(request, nullptr);

    if (!ok)
    {
        WinHttpCloseHandle(request);
        WinHttpCloseHandle(connection);
        WinHttpCloseHandle(session);
        throw std::runtime_error("Receive failed.");
    }

    std::string response;

    DWORD available = 0;

    do
    {
        available = 0;

        WinHttpQueryDataAvailable(request, &available);

        if (available == 0)
            break;

        std::string buffer(available, '\0');

        DWORD read = 0;

        WinHttpReadData(
            request,
            buffer.data(),
            available,
            &read);

        response.append(buffer.data(), read);

    } while (available > 0);

    WinHttpCloseHandle(request);
    WinHttpCloseHandle(connection);
    WinHttpCloseHandle(session);

    return response;
}