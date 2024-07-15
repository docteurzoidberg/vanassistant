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

  uint8_t fuelgauge;      //0-255 (0-100%)
  uint8_t speed;          //in km/h
  uint8_t battery;        //in volts x10 (134 = 13.4V)
  uint8_t rpm;            //rpm x100 (30 = 3000rpm)
  uint16_t coolant_temp;  //temperature x10 in celsius (276 = 27.6 degrees)
  uint32_t odometer;      //total distance in km
  uint32_t trip;          //trip distance in 1/10 km

  bool lamp_preheat;
  bool lamp_notcharging;
  bool lamp_oil;
  bool lamp_turnsignals;
  bool lamp_highbeam;
  bool lamp_lowbeam;
  bool lamp_warnings;
  bool lamp_problem;
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