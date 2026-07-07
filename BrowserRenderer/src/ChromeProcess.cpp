#include "ChromeProcess.h"

#include <chrono>
#include <filesystem>

ChromeProcess::ChromeProcess()
{
    chromePath_ =
        R"(C:\Users\garvc\Desktop\CQ\chrome\win64-150.0.7871.46\chrome-win64\chrome.exe)";

    auto id = std::chrono::steady_clock::now()
                  .time_since_epoch()
                  .count();

    profilePath_ =
        (std::filesystem::temp_directory_path() /
         ("crawler_profile_" + std::to_string(id)))
            .string();

    std::filesystem::create_directories(profilePath_);
}

ChromeProcess::~ChromeProcess()
{
    stop();

    try
    {
        std::filesystem::remove_all(profilePath_);
    }
    catch (...)
    {
    }
}

std::string ChromeProcess::buildCommand(bool headless) const
{
    std::string command;

    command += "\"" + chromePath_ + "\" ";

    command += "--remote-debugging-port=9222 ";

    command += "--user-data-dir=\"" + profilePath_ + "\" ";

    if (headless)
        command += "--headless=new ";

    command += "--no-first-run ";
    command += "--no-default-browser-check ";
    command += "--disable-extensions ";

    return command;
}

bool ChromeProcess::start(bool headless)
{
    if (running_)
        return true;

    STARTUPINFOA startup{};
    startup.cb = sizeof(startup);

    std::string command = buildCommand(headless);

    BOOL success =
        CreateProcessA(
            nullptr,
            command.data(),
            nullptr,
            nullptr,
            FALSE,
            0,
            nullptr,
            nullptr,
            &startup,
            &processInfo_);

    if (!success)
        return false;

    running_ = true;

    Sleep(1000);

    return true;
}

void ChromeProcess::stop()
{
    if (!running_)
        return;

    TerminateProcess(processInfo_.hProcess, 0);


    CloseHandle(processInfo_.hThread);
    CloseHandle(processInfo_.hProcess);

    running_ = false;
}

bool ChromeProcess::isRunning() const
{
    return running_;
}