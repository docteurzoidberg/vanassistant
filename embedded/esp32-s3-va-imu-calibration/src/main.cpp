#include <Arduino.h>
#include <EEPROM.h>
#include "DEV_Config.h"
#include "QMI8658.h"
#include <float.h>

struct CalibrationData {
  float accMinX, accMaxX, accMinY, accMaxY, accMinZ, accMaxZ;
  float gyroMinX, gyroMaxX, gyroMinY, gyroMaxY, gyroMinZ, gyroMaxZ;
  float levelOffsetX, levelOffsetY, levelOffsetZ;
};

CalibrationData calibData;
bool calibrating = false;

void startCalibration() {
  Serial.println("Starting calibration... Move the IMU through its range of motion.");
  calibrating = true;

  // Initialize calibration values
  calibData.accMinX = calibData.accMinY = calibData.accMinZ = FLT_MAX;
  calibData.accMaxX = calibData.accMaxY = calibData.accMaxZ = FLT_MIN;
  calibData.gyroMinX = calibData.gyroMinY = calibData.gyroMinZ = FLT_MAX;
  calibData.gyroMaxX = calibData.gyroMaxY = calibData.gyroMaxZ = FLT_MIN;
}

void stopCalibration() {
  calibrating = false;
  Serial.println("Calibration done. Saving to EEPROM...");
  EEPROM.put(0, calibData);
  EEPROM.commit();
  Serial.println("Calibration saved.");
}

void performCalibration() {
  float acc[3], gyro[3];
  unsigned int tim_count;
  QMI8658_read_xyz(acc, gyro, &tim_count);

  // Update min/max values for accelerometer
  calibData.accMinX = min(calibData.accMinX, acc[0]);
  calibData.accMaxX = max(calibData.accMaxX, acc[0]);
  calibData.accMinY = min(calibData.accMinY, acc[1]);
  calibData.accMaxY = max(calibData.accMaxY, acc[1]);
  calibData.accMinZ = min(calibData.accMinZ, acc[2]);
  calibData.accMaxZ = max(calibData.accMaxZ, acc[2]);

  // Update min/max values for gyroscope
  calibData.gyroMinX = min(calibData.gyroMinX, gyro[0]);
  calibData.gyroMaxX = max(calibData.gyroMaxX, gyro[0]);
  calibData.gyroMinY = min(calibData.gyroMinY, gyro[1]);
  calibData.gyroMaxY = max(calibData.gyroMaxY, gyro[1]);
  calibData.gyroMinZ = min(calibData.gyroMinZ, gyro[2]);
  calibData.gyroMaxZ = max(calibData.gyroMaxZ, gyro[2]);
}

void setLevel() {
  float acc[3], gyro[3];
  unsigned int tim_count;
  QMI8658_read_xyz(acc, gyro, &tim_count);

  calibData.levelOffsetX = acc[0];
  calibData.levelOffsetY = acc[1];
  calibData.levelOffsetZ = acc[2];

  Serial.println("Level set. Saving to EEPROM...");
  EEPROM.put(sizeof(CalibrationData), calibData.levelOffsetX);
  EEPROM.put(sizeof(CalibrationData) + sizeof(float), calibData.levelOffsetY);
  EEPROM.put(sizeof(CalibrationData) + 2 * sizeof(float), calibData.levelOffsetZ);
  EEPROM.commit();
  Serial.println("Level offset saved.");
}

void eraseEEPROM() {
  for (int i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  Serial.println("EEPROM erased.");
}

void setup() {

  if (DEV_Module_Init() != 0) {
		Serial.println("GPIO Init Fail!");
	} else {
		Serial.println("GPIO Init successful!"); 
	}

  QMI8658_init();

	Serial.begin(115200);

  EEPROM.begin(sizeof(CalibrationData) + 3 * sizeof(float));

  // Load saved calibration data
  EEPROM.get(0, calibData);
  EEPROM.get(sizeof(CalibrationData), calibData.levelOffsetX);
  EEPROM.get(sizeof(CalibrationData) + sizeof(float), calibData.levelOffsetY);
  EEPROM.get(sizeof(CalibrationData) + 2 * sizeof(float), calibData.levelOffsetZ);

  //show the calibration data if any
  Serial.print("accMinX: ");
  Serial.println(calibData.accMinX);
  Serial.print("accMaxX: ");
  Serial.println(calibData.accMaxX);
  Serial.print("accMinY: ");
  Serial.println(calibData.accMinY);
  Serial.print("accMaxY: ");
  Serial.println(calibData.accMaxY);
  Serial.print("accMinZ: ");
  Serial.println(calibData.accMinZ);
  Serial.print("accMaxZ: ");
  Serial.println(calibData.accMaxZ);
  Serial.print("gyroMinX: ");
  Serial.println(calibData.gyroMinX);
  Serial.print("gyroMaxX: ");
  Serial.println(calibData.gyroMaxX);
  Serial.print("gyroMinY: ");
  Serial.println(calibData.gyroMinY);
  Serial.print("gyroMaxY: ");
  Serial.println(calibData.gyroMaxY);
  Serial.print("gyroMinZ: ");
  Serial.println(calibData.gyroMinZ);
  Serial.print("gyroMaxZ: ");
  Serial.println(calibData.gyroMaxZ);
  Serial.print("levelOffsetX: ");
  Serial.println(calibData.levelOffsetX);
  Serial.print("levelOffsetY: ");
  Serial.println(calibData.levelOffsetY);
  Serial.print("levelOffsetZ: ");
  Serial.println(calibData.levelOffsetZ);
  
  Serial.println("IMU Initialized. Enter commands to start calibration, set level, or erase EEPROM.");
}

void loop() {
  if (Serial.available()) {
    char command = Serial.read();
    if (command == 'c') {
      startCalibration();
    } else if (command == 't') {
      stopCalibration();
    } else if (command == 'l') {
      setLevel();
    } else if (command == 'e') {
      eraseEEPROM();
    }
  }

  if (calibrating) {
    performCalibration();
  }
}
