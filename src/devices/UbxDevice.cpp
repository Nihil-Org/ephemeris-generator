#include <iostream>
#include <cstring>
#include <stdint.h>
#include <termios.h>
#include <fcntl.h>

namespace
{
    speed_t to_termios_baud(uint32_t baud_rate)
    {
        switch (baud_rate)
        {
        case 1200:
            return B1200;
        case 2400:
            return B2400;
        case 4800:
            return B4800;
        case 9600:
            return B9600;
        case 19200:
            return B19200;
        case 38400:
            return B38400;
        case 57600:
            return B57600;
        case 115200:
            return B115200;
        case 230400:
            return B230400;
        case 460800:
            return B460800;
        case 921600:
            return B921600;
        default:
            throw std::invalid_argument("Unsupported baud rate" + std::to_string(baud_rate));
        }
    }
}

class UbxDevice
{
private:
    std::string serial_port;
    uint16_t baud_rate;
    int file_descriptor;

public:
    UbxDevice(std::string serial_port, uint16_t baud_rate)
        : serial_port{serial_port}, baud_rate{baud_rate}
    {

        file_descriptor = open(serial_port.c_str(), O_RDWR | O_NOCTTY | O_SYNC);

        if (file_descriptor < 0)
        {
            throw std::runtime_error("Failed to open serial port '" + serial_port + "': " + std::strerror(errno));
        }

        struct termios tty;

        std::memset(&tty, 0, sizeof(tty));

        if (tcgetattr(file_descriptor, &tty) != 0)
        {
            close_file_descriptor();
            throw std::runtime_error("Failed to get control attributes from '" + serial_port + "': " + std::strerror(errno));
        }

        const speed_t speed = to_termios_baud(baud_rate);
        cfsetospeed(&tty, speed);
        cfsetispeed(&tty, speed);

        // Modos de controle de hardware?
        // TODO: Aprofundar
        tty.c_cflag = (tty.c_cflag & ~static_cast<tcflag_t>(CSIZE)) | CS8;
        tty.c_cflag &= static_cast<tcflag_t>(~PARENB);
        tty.c_cflag &= static_cast<tcflag_t>(~CSTOPB);
        tty.c_cflag &= static_cast<tcflag_t>(~CRTSCTS); // No hardware flow control
        tty.c_cflag |= (CLOCAL | CREAD);

        // Modo local, tirando todas os comportamentos desnecessarios?
        // TODO: Aprodundar
        tty.c_lflag &= static_cast<tcflag_t>(~(ICANON | ECHO | ECHOE | ISIG));
        tty.c_iflag &= static_cast<tcflag_t>(~(IXON | IXOFF | IXANY | IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL));
        tty.c_oflag &= static_cast<tcflag_t>(~OPOST);

        // Config de timeout e latencia na leitura dos dados?
        // TODO: Aprofundar
        tty.c_cc[VMIN] = 1;
        tty.c_cc[VTIME] = 1;

        if (tcsetattr(file_descriptor, TCSANOW, &tty) != 0)
        {
            close_file_descriptor();
            throw std::runtime_error("Failed to set control attributes from '" + serial_port + "': " + std::strerror(errno));
        }

        tcflush(file_descriptor, TCIOFLUSH);
    }

    void close_file_descriptor()
    {
        if (file_descriptor >= 0)
        {
            close(file_descriptor);
            file_descriptor = -1;
        }
    }
};