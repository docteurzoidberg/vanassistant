#pragma once

// C library headers
#include <iostream>
#include <stdio.h>
#include <string.h>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

#include <sys/file.h>

#include <IDrzSerial.h>

class Drz_Serial_Linux : public IDrzSerial {

  public:

    int serial_port;
    std::string serial_port_name;
    int bauds;
    
    // Create new termios struct, we call it 'tty' for convention
    // No need for "= {0}" at the end as we'll immediately write the existing
    // config to this struct
    struct termios tty;

     // Allocate memory for read buffer, set size according to your needs
    char read_buf[1024];

    Drz_Serial_Linux(std::string serial_port="/dev/ttyUSB0", int bauds=115200) : serial_port_name(serial_port), bauds(bauds) {
      switch (bauds) {
        case 9600:
          this->bauds = B9600;
          break;
        case 115200:
          this->bauds = B115200;
          break;
        default:
          std::cerr << "Invalid baud rate. Defaulting to 115200." << std::endl;
          this->bauds = B115200;
          break;
      }
    }

    bool Setup() override {

      serial_port = open(serial_port_name.c_str(), O_RDWR);

      // Check for errors
      if (serial_port < 0) {
        //printf("Error %i from open: %s\n", errno, strerror(errno));
        std::cerr << "Error " << errno << " from open: " << strerror(errno) << std::endl;
        return false;
      }

        // Acquire non-blocking exclusive lock
      if(flock(serial_port, LOCK_EX | LOCK_NB) == -1) {
        throw std::runtime_error("Serial port with file descriptor " + 
          std::to_string(serial_port) + " is already locked by another process.");
      }

      // Read in existing settings, and handle any error
      // NOTE: This is important! POSIX states that the struct passed to tcsetattr()
      // must have been initialized with a call to tcgetattr() overwise behaviour
      // is undefined
      if(tcgetattr(serial_port, &tty) != 0) {
        //printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        std::cerr << "Error " << errno << " from tcgetattr: " << strerror(errno) << std::endl;
        return false;
      }

      tty.c_cflag &= ~PARENB;
      tty.c_cflag &= ~CSTOPB;
      tty.c_cflag &= ~CSIZE;
      tty.c_cflag |= CS8;
      tty.c_cflag &= ~CRTSCTS;
      tty.c_cflag |= CREAD | CLOCAL;

      tty.c_lflag &= ~ICANON;
      tty.c_lflag &= ~ECHO;
      tty.c_lflag &= ~ECHOE;
      tty.c_lflag &= ~ECHONL;
      tty.c_lflag &= ~ISIG;
      tty.c_iflag &= ~(IXON | IXOFF | IXANY);
      tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);

      tty.c_oflag &= ~OPOST;
      tty.c_oflag &= ~ONLCR;

      tty.c_cc[VTIME] = 0;
      tty.c_cc[VMIN] = 0;

      cfsetispeed(&tty, bauds); 
      cfsetospeed(&tty, bauds);

      if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
          printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
          return 1;
      }

      // Set this instance as the current one
      DrzSerial::Set(this);

      return true;
    }

    int Read() override {

      // Read bytes. The behaviour of read() (e.g. does it block?,
      // how long does it block for?) depends on the configuration
      // settings above, specifically VMIN and VTIME

      memset(&read_buf, '\0', sizeof(read_buf));

      int n = read(serial_port, &read_buf, sizeof(read_buf));

      // n is the number of bytes read. n may be 0 if no bytes were received, and can also be negative to signal an error.
      if (n < 0) {
        //printf("Error reading: %s\n", strerror(errno));
        std::cerr << "Error reading: " << strerror(errno) << std::endl;
      }
      return n;
    }

    bool Write(const char* data, int len) override {
      int n = write(serial_port, data, len);
      if (n < 0) {
        std::cerr << "Error writing to serial port: " << strerror(errno) << std::endl;
        return false;
      }
      if(n != len) {
        std::cerr << "Error writing to serial port: length mismatch" << std::endl;
        return false;
      }
      return true;
    }

    char* GetReadBuffer() override {
      return read_buf;
    }

    bool Close() override {
      return close(serial_port);
    }
};