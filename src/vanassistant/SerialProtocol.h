#include <cstdint>

#include "../Drz_Serial.h"

enum PacketType {
  PT_Unknown = 0,
  PT_Command = 1,
  PT_Response = 2,
  PT_Event = 3,
  PT_Data = 4,
  PT_Error = 5
};

struct J7DashboardPacketData { 
  uint8_t fuelgauge;
  uint8_t battery;
  uint8_t speed;
  uint8_t rpm;
  uint8_t coolant_temp;
  uint16_t odometer;
  bool lamp_preheat;
  bool lamp_notcharging;
  bool lamp_oil;
  bool lamp_turnsignals;
  bool lamp_highbeam;
  bool lamp_lowbeam;
  bool lamp_warnings;
};

class SerialProtocol {
  public:

    char read_buf[256];

    SerialProtocol(Drz_Serial* serial) : serial(serial) {
      //initialize
    }

    void SendData(J7DashboardPacketData* data) {
      //send data
    }

    J7DashboardPacketData* ReceiveData(char* data, int len) {
      //receive data
      return nullptr;
    }
  private:
    Drz_Serial* serial;

};