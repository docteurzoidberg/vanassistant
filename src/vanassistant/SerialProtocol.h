#pragma once

#include <cstdint>
#include <cstring>
#include <iostream>

#include <DrzSerial.h>
#include <IDrzSerial.h>
#include <string>

using namespace drz;

enum J7PacketType {
  J7_SAY_TEXT = 0,
  J7_DASHBOARD = 1,
  J7_IMU = 2,
  VICTRON_SMART_SHUNT = 3,
  VICTRON_ORION = 4
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
  float mag_x;
  float mag_y;
  float mag_z;
};

struct VictronSmartShuntData {
  float timeToGo;
  float battVoltage;
  float auxVoltage;
  float battCurrent;
  float consumedAh;
  int state;
  bool alarm;
  std::string alarmReason;
}; 

struct VictronOrionData {
  bool chargerError;
  int state;
  bool deviceStateOff; 
  bool deviceStateFault;
  float inputVoltage;
  float outputVoltage;
  std::string chargerErrorText;
  std::string deviceStateText; 
  std::string offReasonText;
};

class ISerialProtocolReceiver {
  public:
    virtual void ProcessJ7DashboardPacketData(J7DashboardPacketData* data) = 0;
    virtual void ProcessJ7IMUPacketData(J7IMUPacketData* data) = 0;
    virtual void ProcessJ7SayTextPacketData(J7SayTextPacketData* data) = 0;
    virtual void ProcessVictronSmartShuntData(VictronSmartShuntData* data) = 0;
    virtual void ProcessVictronOrionData(VictronOrionData* data) = 0;
};

class SerialProtocol {
  public:

    char read_buf[256];

    #pragma region WRITE

    static void Use(ISerialProtocolReceiver* receiver) {
      SerialProtocol::receiver = receiver;
    }

    static bool WriteJ7SayTextPacketData(J7SayTextPacketData* data) {
      J7PacketHeader header;
      header.type = J7_SAY_TEXT;
      header.len = sizeof(J7SayTextPacketData);

      // Create a buffer to hold the entire packet
      char packet[sizeof(J7PacketHeader) + sizeof(J7SayTextPacketData)];
      memcpy(packet, &header, sizeof(J7PacketHeader));
      memcpy(packet + sizeof(J7PacketHeader), data, sizeof(J7SayTextPacketData));

      // Send the entire packet
      return DrzSerial::Get()->Write(packet, sizeof(packet));
    }

    static bool WriteJ7DashboardPacketData(J7DashboardPacketData* data) {
      J7PacketHeader header;
      header.type = J7_DASHBOARD;
      header.len = sizeof(J7DashboardPacketData);

      // Create a buffer to hold the entire packet
      char packet[sizeof(J7PacketHeader) + sizeof(J7DashboardPacketData)];
      memcpy(packet, &header, sizeof(J7PacketHeader));
      memcpy(packet + sizeof(J7PacketHeader), data, sizeof(J7DashboardPacketData));

      // Send the entire packet
      return DrzSerial::Get()->Write(packet, sizeof(packet));
    }

    static bool WriteJ7IMUPacketData(J7IMUPacketData* data) {
      J7PacketHeader header;
      header.type = J7_IMU;
      header.len = sizeof(J7IMUPacketData);

      // Create a buffer to hold the entire packet
      char packet[sizeof(J7PacketHeader) + sizeof(J7IMUPacketData)];
      memcpy(packet, &header, sizeof(J7PacketHeader));
      memcpy(packet + sizeof(J7PacketHeader), data, sizeof(J7IMUPacketData));

      // Send the entire packet
      return DrzSerial::Get()->Write(packet, sizeof(packet));
    }

    static bool WriteVictronSmartShuntData(VictronSmartShuntData* data) {
      J7PacketHeader header;
      header.type = VICTRON_SMART_SHUNT;
      header.len = sizeof(VictronSmartShuntData);

      // Create a buffer to hold the entire packet
      char packet[sizeof(J7PacketHeader) + sizeof(VictronSmartShuntData)];
      memcpy(packet, &header, sizeof(J7PacketHeader));
      memcpy(packet + sizeof(J7PacketHeader), data, sizeof(VictronSmartShuntData));

      // Send the entire packet
      return DrzSerial::Get()->Write(packet, sizeof(packet));
    }

    static bool WriteVictronOrionData(VictronOrionData* data) {
      J7PacketHeader header;
      header.type = VICTRON_ORION;
      header.len = sizeof(VictronOrionData);

      // Create a buffer to hold the entire packet
      char packet[sizeof(J7PacketHeader) + sizeof(VictronOrionData)];
      memcpy(packet, &header, sizeof(J7PacketHeader));
      memcpy(packet + sizeof(J7PacketHeader), data, sizeof(VictronOrionData));

      // Send the entire packet
      return DrzSerial::Get()->Write(packet, sizeof(packet));
    }
    
    #pragma region READ

    static bool ReadSerial() {
      //std::cout << "SerialProtocol::ReadSerial called" << std::endl;

      IDrzSerial* serial = DrzSerial::Get();
      if(serial==nullptr) {
        return false;
      }

      //parse data
      J7PacketHeader header;
      int bytesRead = serial->Read();
      if(bytesRead<=0) {
        //no data
        return false;
      }

      if (bytesRead < sizeof(J7PacketHeader)) {
        std::cerr << "Incorrect packet len" << std::endl;
        return false;
      }

      if(!ReadHeader(serial->GetReadBuffer(), &header)) {
        std::cerr << "Failed to read header" << std::endl;
        return false;
      }

      switch (header.type) {
        case J7_IMU: {
          J7IMUPacketData data;
          if(ReadJ7IMUPacketData(serial->GetReadBuffer(), &data)) {
            receiver->ProcessJ7IMUPacketData(&data);
          } else {
            std::cerr << "Failed to read J7SayTextPacketData" << std::endl;
          }
          break;
        }
        case J7_SAY_TEXT: {
          J7SayTextPacketData data;
          if(ReadJ7SayTextPacketData(serial->GetReadBuffer(), &data)) {
            receiver->ProcessJ7SayTextPacketData(&data);
          } else {
            std::cerr << "Failed to read J7SayTextPacketData" << std::endl;
          }
          break;
        }
        case J7_DASHBOARD: { 
          J7DashboardPacketData data;
          if(ReadJ7DashboardPacketData(serial->GetReadBuffer(), &data)) {
            receiver->ProcessJ7DashboardPacketData(&data);
          } else {
            std::cerr << "Failed to read J7DashboardPacketData" << std::endl;
          }
          break;
        }
        default:
          std::cerr << "Unknown packet type: " << header.type << std::endl;
          return false;
          break;
      }
      return true;
    }

    static bool ReadJ7DashboardPacketData(char* data, J7DashboardPacketData *dashboard_data) {
      J7PacketHeader header;
      memcpy(&header, data, sizeof(J7PacketHeader));
      if (header.type != J7_DASHBOARD) { 
        std::cerr << "ReadJ7DashboardPacketData: Invalid packet type" << std::endl;
        return false;
      }
      memcpy(dashboard_data, data + sizeof(J7PacketHeader), sizeof(J7DashboardPacketData));
      return true;
    }

    static bool ReadJ7SayTextPacketData(char* data, J7SayTextPacketData *saytext_data) {
      J7PacketHeader header;
      memcpy(&header, data, sizeof(J7PacketHeader));
      if (header.type != J7_SAY_TEXT) { 
        std::cerr << "ReadJ7SayTextPacketData: Invalid packet type" << std::endl;
        return false;
      }
      memcpy(saytext_data, data + sizeof(J7PacketHeader), sizeof(J7SayTextPacketData));
      return true;
    }

    static bool ReadJ7IMUPacketData(char* data, J7IMUPacketData *imu_data) {
      J7PacketHeader header;
      memcpy(&header, data, sizeof(J7PacketHeader));
      if (header.type != J7_IMU) { 
        std::cerr << "ReadJ7IMUDataPacket: Invalid packet type" << std::endl;
        return false;
      }
      memcpy(imu_data, data + sizeof(J7PacketHeader), sizeof(J7IMUPacketData));
      return true;
    }

    static bool ReadVictronSmartShuntData(char* data, VictronSmartShuntData *shunt_data) {
      J7PacketHeader header;
      memcpy(&header, data, sizeof(J7PacketHeader));
      if (header.type != VICTRON_SMART_SHUNT) { 
        std::cerr << "ReadVictronSmartShuntData: Invalid packet type" << std::endl;
        return false;
      }
      memcpy(shunt_data, data + sizeof(J7PacketHeader), sizeof(VictronSmartShuntData));
      return true;
    }

    static bool ReadVictronOrionData(char* data, VictronOrionData *orion_data) {
      J7PacketHeader header;
      memcpy(&header, data, sizeof(J7PacketHeader));
      if (header.type != VICTRON_ORION) { 
        std::cerr << "ReadVictronOrionData: Invalid packet type" << std::endl;
        return false;
      }
      memcpy(orion_data, data + sizeof(J7PacketHeader), sizeof(VictronOrionData));
      return true;
    }

    static bool ReadHeader (char* data, J7PacketHeader *header) {
      memcpy(header, data, sizeof(J7PacketHeader));
      return true;
    }

    #pragma endregion
  private:
    inline static ISerialProtocolReceiver* receiver;

};

