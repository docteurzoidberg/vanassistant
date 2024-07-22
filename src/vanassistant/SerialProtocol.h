#pragma once

#include <cstdint>
#include <cstring>
#include <iostream>

#include <IDrzSerial.h>

enum J7PacketType {
  J7_SAY_TEXT = 0,
  J7_DASHBOARD = 1,
  J7_IMU = 2,
};

struct J7PacketHeader {
  uint8_t type;
  uint32_t len;
};

//Packet sent to VanAssistant for TTS
//contains the length of the text and the text itself
struct J7SayTextPacketData {
  uint8_t len;
  char text[256];
};

//Packet sent from dashboard-io board to VanAssistant and VanDashboard
//contains all physical data acquired from van's original dashboard wiring
//*van does not have rpm sensor but could be added in the future
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

struct J7IMUPacketData {
  float acc_x;
  float acc_y;
  float acc_z;
  float gyro_x;
  float gyro_y;
  float gyro_z;
};

class SerialProtocol {
  public:

    char read_buf[256];

    SerialProtocol(IDrzSerial* serial) : serial(serial) {
      //initialize
    }

    #pragma region SEND

    bool SendJ7SayTextPacketData(J7SayTextPacketData* data) {
      J7PacketHeader header;
      header.type = J7_SAY_TEXT;
      header.len = sizeof(J7SayTextPacketData);

      // Create a buffer to hold the entire packet
      char packet[sizeof(J7PacketHeader) + sizeof(J7SayTextPacketData)];
      memcpy(packet, &header, sizeof(J7PacketHeader));
      memcpy(packet + sizeof(J7PacketHeader), data, sizeof(J7SayTextPacketData));

      // Send the entire packet
      return serial->Write(packet, sizeof(packet));
    }

    bool SendJ7DashboardPacketData(J7DashboardPacketData* data) {
      J7PacketHeader header;
      header.type = J7_DASHBOARD;
      header.len = sizeof(J7DashboardPacketData);

      // Create a buffer to hold the entire packet
      char packet[sizeof(J7PacketHeader) + sizeof(J7DashboardPacketData)];
      memcpy(packet, &header, sizeof(J7PacketHeader));
      memcpy(packet + sizeof(J7PacketHeader), data, sizeof(J7DashboardPacketData));

      // Send the entire packet
      return serial->Write(packet, sizeof(packet));
    }

    bool SendJ7IMUPacketData(J7IMUPacketData* data) {
      J7PacketHeader header;
      header.type = J7_IMU;
      header.len = sizeof(J7IMUPacketData);

      // Create a buffer to hold the entire packet
      char packet[sizeof(J7PacketHeader) + sizeof(J7IMUPacketData)];
      memcpy(packet, &header, sizeof(J7PacketHeader));
      memcpy(packet + sizeof(J7PacketHeader), data, sizeof(J7IMUPacketData));

      // Send the entire packet
      return serial->Write(packet, sizeof(packet));
    }
    
    #pragma region READ

    bool ReadJ7DashboardPacketData(char* data, J7DashboardPacketData *dashboard_data) {
      J7PacketHeader header;
      memcpy(&header, data, sizeof(J7PacketHeader));
      if (header.type != J7_DASHBOARD) { 
        std::cerr << "ReadJ7DashboardPacketData: Invalid packet type" << std::endl;
        return false;
      }
      memcpy(dashboard_data, data + sizeof(J7PacketHeader), sizeof(J7DashboardPacketData));
      return true;
    }

    bool ReadJ7SayTextPacketData(char* data, J7SayTextPacketData *saytext_data) {
      J7PacketHeader header;
      memcpy(&header, data, sizeof(J7PacketHeader));
      if (header.type != J7_SAY_TEXT) { 
        std::cerr << "ReadJ7SayTextPacketData: Invalid packet type" << std::endl;
        return false;
      }
      memcpy(saytext_data, data + sizeof(J7PacketHeader), sizeof(J7SayTextPacketData));
      return true;
    }

    bool ReadJ7IMUPacketData(char* data, J7IMUPacketData *imu_data) {
      J7PacketHeader header;
      memcpy(&header, data, sizeof(J7PacketHeader));
      if (header.type != J7_IMU) { 
        std::cerr << "ReadJ7IMUDataPacket: Invalid packet type" << std::endl;
        return false;
      }
      memcpy(imu_data, data + sizeof(J7PacketHeader), sizeof(J7IMUPacketData));
      return true;
    }

    bool ReadHeader (char* data, J7PacketHeader *header) {
      memcpy(header, data, sizeof(J7PacketHeader));
      return true;
    }

    #pragma endregion
    

  private:
    IDrzSerial* serial;

};