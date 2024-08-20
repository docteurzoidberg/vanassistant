#include <Arduino.h>
#include <EEPROM.h>
#include "LCD_Test.h"
#include "SerialProtocol.h"
#include <vector>
#include <numeric>
#include <cmath>

UWORD Imagesize = LCD_1IN28_HEIGHT * LCD_1IN28_WIDTH * 2;
UWORD *BlackImage;

SerialProtocol *serialProtocol;

struct CalibrationData {
  float accMinX, accMaxX, accMinY, accMaxY, accMinZ, accMaxZ;
  float gyroMinX, gyroMaxX, gyroMinY, gyroMaxY, gyroMinZ, gyroMaxZ;
  float levelOffsetX, levelOffsetY, levelOffsetZ;
};

CalibrationData calibData;

void loadCalibrationData() {
  EEPROM.get(0, calibData);
  EEPROM.get(sizeof(CalibrationData), calibData.levelOffsetX);
  EEPROM.get(sizeof(CalibrationData) + sizeof(float), calibData.levelOffsetY);
  EEPROM.get(sizeof(CalibrationData) + 2 * sizeof(float), calibData.levelOffsetZ);
}

void initializeLCD() {
	LCD_1IN28_Init(HORIZONTAL);
	LCD_1IN28_Clear(BLACK);   
	Paint_NewImage((UBYTE *)BlackImage, LCD_1IN28.WIDTH, LCD_1IN28.HEIGHT, 0, BLACK);
	Paint_SetScale(65);
	Paint_SetRotate(ROTATE_0);
}

void drawInitialScreen() {
	Paint_DrawRectangle(0, 47, 240, 120, 0X4F30, DOT_PIXEL_2X2, DRAW_FILL_FULL);
	Paint_DrawRectangle(0, 120, 240, 195, 0XF410, DOT_PIXEL_2X2, DRAW_FILL_FULL);
	Paint_DrawString_EN(45, 30, "IMU Sender", &Font16, WHITE, BLACK);
	Paint_DrawString_EN(45, 75, "sent = ", &Font16, WHITE, BLACK);
	Paint_DrawString_EN(45, 145, "recv = ", &Font16, WHITE, BLACK);
	LCD_1IN28_Display(BlackImage);
}

bool ReadSerial() {

  if (Serial.available() <= 0) 
    return false;
        
  size_t bytesRead = Serial.readBytes(serialProtocol->read_buf, sizeof(serialProtocol->read_buf));
  if (bytesRead < sizeof(J7PacketHeader)) 
    return false;

  J7PacketHeader header;
  memcpy(&header, serialProtocol->read_buf, sizeof(J7PacketHeader)); 

  switch (header.type) {
    case J7_SAY_TEXT: {
      if (bytesRead >= sizeof(J7PacketHeader) + sizeof(J7SayTextPacketData)) {
        J7SayTextPacketData sayTextData;
        if (serialProtocol->ReadJ7SayTextPacketData(serialProtocol->read_buf, &sayTextData)) {
          //Serial.print("Received Text: ");
          //Serial.println(sayTextData.text);
        }
      }
      break;
    }
    case J7_DASHBOARD: {
      if (bytesRead >= sizeof(J7PacketHeader) + sizeof(J7DashboardPacketData)) {
        J7DashboardPacketData dashboardData;
        if (serialProtocol->ReadJ7DashboardPacketData(serialProtocol->read_buf, &dashboardData)) {
          //Serial.println("Received Dashboard Data:");
          //Serial.print("Fuel Gauge: "); Serial.println(dashboardData.fuelgauge);
          //Serial.print("Speed: "); Serial.println(dashboardData.speed);
          //Serial.print("Battery: "); Serial.println(dashboardData.battery);
          //Serial.print("RPM: "); Serial.println(dashboardData.rpm);
          //Serial.print("Coolant Temp: "); Serial.println(dashboardData.coolant_temp);
          //Serial.print("Odometer: "); Serial.println(dashboardData.odometer);
          //Serial.print("Trip: "); Serial.println(dashboardData.trip);
          // Print other fields as needed
        }
      }
      break;
    }
    case J7_IMU_DATA: {
      if (bytesRead >= sizeof(J7PacketHeader) + sizeof(J7IMUPacketData)) {
        J7IMUPacketData sensorData;
        if (serialProtocol->ReadJ7IMUPacketData(serialProtocol->read_buf, &sensorData)) {
          //Serial.println("Received Sensor Data:");
          //Serial.print("Acc: (");
          //Serial.print(sensorData.acc_x);
          //Serial.print(", ");
          //Serial.print(sensorData.acc_y);
          //Serial.print(", ");
          //Serial.print(sensorData.acc_z);
          //Serial.print("), Gyro: (");
          //Serial.print(sensorData.gyro_x);
          //Serial.print(", ");
          //Serial.print(sensorData.gyro_y);
          //Serial.print(", ");
          //Serial.print(sensorData.gyro_z);
          //Serial.println(")");
        }
      }
      break;
    }
    default: {
        //Serial.println("Unknown packet type or incomplete packet received");
        return false;
    }
  }
  return true;
}

float normalize(float value, float minVal, float maxVal) {
  return (value - minVal) / (maxVal - minVal) * 2.0 - 1.0;
}

void setup() {
	double fps = 0;  
	unsigned long lastUpdate = millis();
	unsigned int frame = 0;
	float acc[3], gyro[3];
	unsigned int tim_count = 0;     

  uint32_t sentPackets = 0;
  uint32_t receivedPackets = 0;

	Serial.begin(115200);
  
  // Initialize SerialProtocol
  serialProtocol = new SerialProtocol(&Serial);

	// PSRAM Initialize
	if (psramInit()) {
		Serial.println("\nPSRAM is correctly initialized");
	} else {
		Serial.println("PSRAM not available");
	}
	if ((BlackImage = (UWORD *)ps_malloc(Imagesize)) == NULL) {
		Serial.println("Failed to apply for black memory...");
		exit(0);
	}

	if (DEV_Module_Init() != 0) {
		Serial.println("GPIO Init Fail!");
	} else {
		Serial.println("GPIO Init successful!"); 
	}

	QMI8658_init();
	Serial.println("QMI8658_init\r\n");

  EEPROM.begin(sizeof(CalibrationData) + 3 * sizeof(float));
  loadCalibrationData();
  Serial.println("IMU Initialized with calibration data. Normalized values will be sent over Serial.");

	

	initializeLCD();
	drawInitialScreen();

	while (true) {
		unsigned long elapsed = millis() - lastUpdate;
		if (elapsed >= 1000) {
			lastUpdate = millis();
			fps = ((float)(frame) / (float)elapsed) * 1000;
			frame = 0;
		} 

    if(ReadSerial()) {
      receivedPackets++;
    }

		QMI8658_read_xyz(acc, gyro, &tim_count);

    // Normalize accelerometer values
    float normAccX = normalize(acc[0], calibData.accMinX, calibData.accMaxX);
    float normAccY = normalize(acc[1], calibData.accMinY, calibData.accMaxY);
    float normAccZ = normalize(acc[2], calibData.accMinZ, calibData.accMaxZ);

    // Normalize gyroscope values
    float normGyroX = normalize(gyro[0], calibData.gyroMinX, calibData.gyroMaxX);
    float normGyroY = normalize(gyro[1], calibData.gyroMinY, calibData.gyroMaxY);
    float normGyroZ = normalize(gyro[2], calibData.gyroMinZ, calibData.gyroMaxZ);

    // Apply level offsets to accelerometer values
    normAccX -= calibData.levelOffsetX;
    normAccY -= calibData.levelOffsetY;
    normAccZ -= calibData.levelOffsetZ;

     // Send sensor data packet
    J7IMUPacketData sensorData;
    sensorData.acc_x = normAccX;
    sensorData.acc_y = normAccY;
    sensorData.acc_z = normAccZ;
    sensorData.gyro_x = normGyroX;
    sensorData.gyro_y = normGyroY;
    sensorData.gyro_z = normGyroZ;
    serialProtocol->SendJ7IMUPacketData(&sensorData); 
    sentPackets++;

    Paint_DrawRectangle(0, 47, 240, 120, 0X4F30, DOT_PIXEL_2X2, DRAW_FILL_FULL);
    Paint_DrawRectangle(0, 120, 240, 195, 0XF410, DOT_PIXEL_2X2, DRAW_FILL_FULL);
    Paint_DrawNum(120, 75, sentPackets, &Font16, 0, BLACK, WHITE);
		Paint_DrawNum(120, 145, receivedPackets, &Font16, 0, BLACK, WHITE);
    LCD_1IN28_DisplayWindows(120, 75, 210, 95, BlackImage);
    LCD_1IN28_DisplayWindows(120, 145, 210, 165, BlackImage);
		frame++;
	}
}

void loop() {
	delay(2); // allow the CPU to switch to other tasks
}
