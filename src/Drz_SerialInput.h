	

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

class Drz_SerialInput {

  public:

    int serial_port;
    
    // Create new termios struct, we call it 'tty' for convention
    // No need for "= {0}" at the end as we'll immediately write the existing
    // config to this struct
    struct termios tty;

     // Allocate memory for read buffer, set size according to your needs
    char read_buf [256];

    Drz_SerialInput() {

    }

    bool Setup() {

      serial_port = open("/dev/ttyAMA0", O_RDWR);

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

      //cfsetispeed(&tty, B9600);
      //cfsetospeed(&tty, B9600);

      cfsetispeed(&tty, B115200); 
      cfsetospeed(&tty, B115200);

      if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
          printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
          return 1;
      }

      return true;
    }

    int Read() {

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

    char* GetReadBuffer() {
      return read_buf;
    }

    void Close() {
      close(serial_port);
    }
};