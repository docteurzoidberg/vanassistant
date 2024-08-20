#include <Arduino.h>

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

J7IMUPacketData imuData;
J7DashboardPacketData dashboardData;
J7SayTextPacketData saytextData;

unsigned long pingLastSent = 0;
unsigned long imuLastSent = 0;
unsigned long dashboardLastSent = 0;
unsigned long sayTextLastSent = 0;

const unsigned int pingSendEvery = 1000;
const unsigned int imuSendEvery = 0;
const unsigned int dashboardSendEvery = 0;

#define RX_BUFFER_SIZE 1024
#define TX_BUFFER_SIZE 1024

char readBuffer[RX_BUFFER_SIZE];
char writeBuffer[TX_BUFFER_SIZE];

unsigned int serialOutput = 0; //read from dipswitches

HardwareSerial* outputSerial = nullptr;

void readHeader(char* data, J7PacketHeader *header) {
  memcpy(header, data, sizeof(J7PacketHeader));
}

bool readDashboardPacket(char* data, J7DashboardPacketData *dashboard_data) {
  J7PacketHeader header;
  memcpy(&header, data, sizeof(J7PacketHeader));
  if (header.type != J7_DASHBOARD) { 
    //std::cerr << "ReadJ7DashboardPacketData: Invalid packet type" << std::endl;
    return false;
  }
  memcpy(dashboard_data, data + sizeof(J7PacketHeader), sizeof(J7DashboardPacketData));
  return true;
}

bool readSayTextPacket(char* data, J7SayTextPacketData *saytext_data) {
  J7PacketHeader header;
  memcpy(&header, data, sizeof(J7PacketHeader));
  if (header.type != J7_SAY_TEXT) { 
    //std::cerr << "ReadJ7SayTextPacketData: Invalid packet type" << std::endl;
    return false;
  }
  memcpy(saytext_data, data + sizeof(J7PacketHeader), sizeof(J7SayTextPacketData));
  return true;
}

bool readImuPacket(char* data, J7IMUPacketData *imu_data) {
  J7PacketHeader header;
  memcpy(&header, data, sizeof(J7PacketHeader));
  if (header.type != J7_IMU) { 
    //std::cerr << "ReadJ7IMUDataPacket: Invalid packet type" << std::endl;
    return false;
  }
  memcpy(imu_data, data + sizeof(J7PacketHeader), sizeof(J7IMUPacketData));
  return true;
}

void sendPingPacket() {
  if(outputSerial == nullptr) return;
  outputSerial->println("P");
  pingLastSent = millis();
}

void sendSayTextPacket() {
  if(outputSerial == nullptr) return;
  outputSerial->println("T");
  J7PacketHeader header;
  header.type = J7_SAY_TEXT;
  header.len = sizeof(J7SayTextPacketData);
  memcpy(writeBuffer, &header, sizeof(J7PacketHeader));
  memcpy(writeBuffer + sizeof(J7PacketHeader), &saytextData, sizeof(J7SayTextPacketData));
  outputSerial->write(writeBuffer, sizeof(J7PacketHeader) + sizeof(J7SayTextPacketData));
  sayTextLastSent=millis();
}

void sendImuPacket() {
  if(outputSerial == nullptr) return;
  outputSerial->println("I");
  J7PacketHeader header;
  header.type = J7_IMU;
  header.len = sizeof(J7IMUPacketData);
  memcpy(writeBuffer, &header, sizeof(J7PacketHeader));
  memcpy(writeBuffer + sizeof(J7PacketHeader), &imuData, sizeof(J7IMUPacketData));
  outputSerial->write(writeBuffer, sizeof(J7PacketHeader) + sizeof(J7IMUPacketData));
  imuLastSent=millis();
}

void sendDashboardPacket() {
  if(outputSerial == nullptr) return;
  outputSerial->println("D");
  J7PacketHeader header;
  header.type = J7_DASHBOARD;
  header.len = sizeof(J7DashboardPacketData);
  memcpy(writeBuffer, &header, sizeof(J7PacketHeader));
  memcpy(writeBuffer + sizeof(J7PacketHeader), &imuData, sizeof(J7DashboardPacketData));
  outputSerial->write(writeBuffer, sizeof(J7PacketHeader) + sizeof(J7DashboardPacketData));
  dashboardLastSent=millis();
}

void processDashboardPacket(J7DashboardPacketData* data) {
  dashboardData = *data;
  sendDashboardPacket();
}

void processImuPacket(J7IMUPacketData* data) {
  imuData = *data;
  sendImuPacket();
}

void processSayTextPacket(J7SayTextPacketData* data) {
  saytextData = *data;
  sendSayTextPacket();
}

bool readSerial(HardwareSerial* serial) {
  
  if(!serial->available()) 
    return false;

  unsigned int bytesRead = serial->readBytes(readBuffer, RX_BUFFER_SIZE);

  if(bytesRead<=sizeof(J7PacketHeader))
    return false;
  
  //read header
  J7PacketHeader header;
  readHeader(readBuffer, &header);

  switch (header.type) {
    case J7_IMU: {
      J7IMUPacketData data;
      if(readImuPacket(readBuffer, &data)) {
        processImuPacket(&data);
        return true;
      } else {
        //std::cerr << "Failed to read J7SayTextPacketData" << std::endl;
      }
      break;
    }
    case J7_SAY_TEXT: {
      J7SayTextPacketData data;
      if(readSayTextPacket(readBuffer, &data)) {
        processSayTextPacket(&data);
        return true;
      } else {
        //std::cerr << "Failed to read J7SayTextPacketData" << std::endl;
      }
      break;
    }
    case J7_DASHBOARD: { 
      J7DashboardPacketData data;
      if(readDashboardPacket(readBuffer, &data)) {
        processDashboardPacket(&data);
        return true;
      } else {
        //std::cerr << "Failed to read J7DashboardPacketData" << std::endl;
      }
      break;
    }
    default:
      //std::cerr << "Unknown packet type: " << header.type << std::endl;
      break;
  }
  return false;
}

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial2.begin(115200);
  Serial3.begin(115200);

  //TODO: read dipswitches to determine serial port
  serialOutput = 0; 

  if(serialOutput == 0) outputSerial = &Serial;
  if(serialOutput == 1) outputSerial = &Serial1;
  if(serialOutput == 2) outputSerial = &Serial2;
  if(serialOutput == 3) outputSerial = &Serial3;
}

void loop() {
  readSerial(&Serial);
  readSerial(&Serial1);
  readSerial(&Serial2);
  readSerial(&Serial3);

  if(millis()-pingLastSent > pingSendEvery) {
    sendPingPacket();
  }
  if(millis()-imuLastSent > imuSendEvery) {
    sendImuPacket();
  }
  if(millis()-dashboardLastSent > dashboardSendEvery) {
    sendDashboardPacket();
  }
}
