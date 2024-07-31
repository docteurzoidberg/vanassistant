#pragma once

#include <iostream>
#include <windows.h>
#include <string>

#include <IDrzSerial.h>

class Drz_Serial_Windows : public IDrzSerial {

public:
	HANDLE hSerial;
	std::string serial_port_name;
	int bauds;
	DCB dcbSerialParams;
	COMMTIMEOUTS timeouts;
	char read_buf[1024];

	Drz_Serial_Windows(std::string serial_port="COM1", int bauds=115200) : serial_port_name(serial_port), bauds(bauds) {
		hSerial = INVALID_HANDLE_VALUE;

		// Set the baud rate
		dcbSerialParams.BaudRate = bauds;
	}

	bool Setup() override {
		hSerial = CreateFile(serial_port_name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hSerial == INVALID_HANDLE_VALUE) {
			std::cerr << "Error opening serial port: " << GetLastError() << std::endl;
			return false;
		}

		dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

		if (!GetCommState(hSerial, &dcbSerialParams)) {
			std::cerr << "Error getting serial port state: " << GetLastError() << std::endl;
			return false;
		}

		dcbSerialParams.ByteSize = 8;
		dcbSerialParams.StopBits = ONESTOPBIT;
		dcbSerialParams.Parity = NOPARITY;

		if (!SetCommState(hSerial, &dcbSerialParams)) {
			std::cerr << "Error setting serial port state: " << GetLastError() << std::endl;
			return false;
		}

		timeouts.ReadIntervalTimeout = 50;
		timeouts.ReadTotalTimeoutConstant = 50;
		timeouts.ReadTotalTimeoutMultiplier = 10;
		timeouts.WriteTotalTimeoutConstant = 50;
		timeouts.WriteTotalTimeoutMultiplier = 10;

		if (!SetCommTimeouts(hSerial, &timeouts)) {
			std::cerr << "Error setting serial port timeouts: " << GetLastError() << std::endl;
			return false;
		}

		DrzSerial::Set(this);
		return true;
	}

	int Read() override {
		DWORD bytesRead;
		if (!ReadFile(hSerial, read_buf, sizeof(read_buf), &bytesRead, NULL)) {
			std::cerr << "Error reading from serial port: " << GetLastError() << std::endl;
			return -1;
		}
		return bytesRead;
	}

	bool Write(const char* data, int len) override {
		DWORD bytesWritten;
		if (!WriteFile(hSerial, data, len, &bytesWritten, NULL)) {
			std::cerr << "Error writing to serial port: " << GetLastError() << std::endl;
			return false;
		}
		return bytesWritten == len;
	}

	char* GetReadBuffer() override {
		return read_buf;
	}

	bool Close() override {
		if (hSerial != INVALID_HANDLE_VALUE) {
			CloseHandle(hSerial);
			hSerial = INVALID_HANDLE_VALUE;
			return true;
		}
		return false;
	}
};
