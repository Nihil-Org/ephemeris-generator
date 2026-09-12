#include "cli/CLI.cpp"

#include <stdint.h>
#include <atomic>
#include <iostream>
#include <csignal>

namespace
{
    std::atomic<bool> is_running{true};

    void signal_handler(int)
    {
        is_running = false;
    }

    void print_usage(const char *prog_name)
    {
        std::cout << "Usage: " << prog_name << " --serial <port> --baud <rate>" << std::endl;
    }
}

int main(int argc, char *argv[])
{
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    std::string serial_port;
    uint16_t baud_rate;

    CLI::parse(argc, argv, serial_port, baud_rate);

    try
    {
        std::cout << "Opening serial port: " << serial_port << " at " << baud_rate << " baud..." << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Failed to init serial port:" << e.what() << std::endl;
        return 1;
    }

    return 0;
}