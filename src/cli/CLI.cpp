#include <iostream>
#include <stdint.h>
#include <vector>

class CLI
{
public:
    CLI() = delete;

    static void parse(int argc, char *argv[], std::string &serial_port, uint16_t &baud_rate)
    {
        std::vector<std::string> args{argv + 1, argv + argc};

        for (size_t i = 0; i < args.size(); ++i)
        {
            if (args[i] == "--serial")
            {
                serial_port = args[++i];
            }
            else if (args[i] == "--baud")
            {
                baud_rate = static_cast<uint16_t>(std::stoul(args[++i]));
            }
        }
    }
};