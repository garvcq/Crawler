#pragma once
#include <winsock2.h>
#include <windows.h>
#include <string>

class ChromeProcess
{
public:
    ChromeProcess();
    ~ChromeProcess();

    bool start(bool headless = false);
    void stop();

    bool isRunning() const;

private:
    PROCESS_INFORMATION processInfo_{};
    bool running_ = false;

    std::string chromePath_;
    std::string profilePath_;

    std::string buildCommand(bool headless) const;
};