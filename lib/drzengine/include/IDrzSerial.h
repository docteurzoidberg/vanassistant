#pragma once

class IDrzSerial {
  public:
    virtual bool Setup() = 0;
    virtual int Read() = 0;
    virtual bool Write(const char* data, int length) = 0;
    virtual bool Close() = 0;
    virtual char* GetReadBuffer() = 0;
};

class DrzSerial {
public:
  static IDrzSerial* Get() {
    return DrzSerial::serial;
  }
  static void Set(IDrzSerial* serial) {
    DrzSerial::serial = serial;
  }

private:
  inline static IDrzSerial* serial;

};