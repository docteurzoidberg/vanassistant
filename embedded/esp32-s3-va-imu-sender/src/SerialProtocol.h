#pragma once

#include <Arduino.h>

enum J7PacketType {
  J7_SAY_TEXT = 0,
  J7_DASHBOARD = 1,
  J7_IMU_DATA = 2
};

struct J7PacketHeader {
  uint8_t type;
  uint32_t len;
};

struct J7SayTextPacketData {
  uint8_t len;
  char text[256];
};

struct J7DashboardPacketData { 
  uint8_t fuelgauge;
  uint8_t speed;
  uint8_t battery;
  uint8_t rpm;
  uint16_t coolant_temp;
  uint32_t odometer;
  uint32_t trip;

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

    SerialProtocol(HardwareSerial* serial) : serial(serial) {
      // Initialize
    }

    bool SendJ7IMUPacketData(J7IMUPacketData* data) {
      J7PacketHeader header;
      header.type = J7_IMU_DATA;
      header.len = sizeof(J7IMUPacketData);
      char packet[sizeof(J7PacketHeader) + sizeof(J7IMUPacketData)];
      memcpy(packet, &header, sizeof(J7PacketHeader));
      memcpy(packet + sizeof(J7PacketHeader), data, sizeof(J7IMUPacketData));
      return serial->write((uint8_t*)packet, sizeof(packet)) == sizeof(packet);
    }

    bool SendJ7SayTextPacketData(J7SayTextPacketData* data) {
      J7PacketHeader header;
      header.type = J7_SAY_TEXT;
      header.len = sizeof(J7SayTextPacketData);
      char packet[sizeof(J7PacketHeader) + sizeof(J7SayTextPacketData)];
      memcpy(packet, &header, sizeof(J7PacketHeader));
      memcpy(packet + sizeof(J7PacketHeader), data, sizeof(J7SayTextPacketData));
      return serial->write((uint8_t*)packet, sizeof(packet)) == sizeof(packet);
    }

    bool SendJ7DashboardPacketData(J7DashboardPacketData* data) {
      J7PacketHeader header;
      header.type = J7_DASHBOARD;
      header.len = sizeof(J7DashboardPacketData);
      char packet[sizeof(J7PacketHeader) + sizeof(J7DashboardPacketData)];
      memcpy(packet, &header, sizeof(J7PacketHeader));
      memcpy(packet + sizeof(J7PacketHeader), data, sizeof(J7DashboardPacketData));
      return serial->write((uint8_t*)packet, sizeof(packet)) == sizeof(packet);
    }

    bool ReadJ7IMUPacketData(char* data, J7IMUPacketData *imu_data) {
      J7PacketHeader header;
      memcpy(&header, data, sizeof(J7PacketHeader));
      if (header.type != J7_IMU_DATA) { 
        return false;
      }
      memcpy(imu_data, data + sizeof(J7PacketHeader), sizeof(J7IMUPacketData));
      return true;
    }

    bool ReadJ7DashboardPacketData(char* data, J7DashboardPacketData *dashboard_data) {
      J7PacketHeader header;
      memcpy(&header, data, sizeof(J7PacketHeader));
      if (header.type != J7_DASHBOARD) { 
        return false;
      }
      memcpy(dashboard_data, data + sizeof(J7PacketHeader), sizeof(J7DashboardPacketData));
      return true;
    }

    bool ReadJ7SayTextPacketData(char* data, J7SayTextPacketData *saytext_data) {
      J7PacketHeader header;
      memcpy(&header, data, sizeof(J7PacketHeader));
      if (header.type != J7_SAY_TEXT) { 
        return false;
      }
      memcpy(saytext_data, data + sizeof(J7PacketHeader), sizeof(J7SayTextPacketData));
      return true;
    }

    bool ReadHeader(char* data, J7PacketHeader *header) {
      memcpy(header, data, sizeof(J7PacketHeader));
      return true;
    }

  private:
    HardwareSerial* serial;
};
