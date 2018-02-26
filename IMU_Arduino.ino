/*  
  Hardware setup:
  MPU9250 Breakout --------- Arduino
  VDD ---------------------- 3.3V
  VDDI --------------------- 3.3V
  SDA ----------------------- A4
  SCL ----------------------- A5
  GND ---------------------- GND
*/

#include <Wire.h>

long ACC_X_raw, ACC_Y_raw, ACC_Z_raw;
float ACC_X, ACC_Y, ACC_Z;

long GYR_X_raw, GYR_Y_raw, GYR_Z_raw;
float GYR_X, GYR_Y, GYR_Z;

float temperatureData, temperature;

boolean ACC = true;
boolean GYR = false;

boolean DEBUG = false;

unsigned long time;

void setup()
{
  Serial.begin(19200);
  Wire.begin();     // initilize I2C communication
  setupMPU();       // Setup function for this script
}


void loop()
{
  time = millis();
  recordAccRegisters();
  recordGyr_Registers();
  recordTemperatureRegister();
  printData();
  delay(1);
}

void setupMPU() {
  Wire.beginTransmission(0x68);  // This is the I2C address of the MPU. 0x68 for logic AD0 and 0x69 for logic AD1
  Wire.write(0x6B);              // Accessing the register - Power management
  Wire.write(0b00000000);        // Setting sleep registor to 0
  Wire.endTransmission();

  // Setup ACC
  Wire.beginTransmission(0x68);  // I2C address of MPU
  Wire.write(0x1C);              // Access 1C register - ACC
  Wire.write(0b00000000);        // Set ACC to +/- 2G
  Wire.endTransmission();

  // Setup GYR
  Wire.beginTransmission(0x68);  // I2C address of MPU
  Wire.write(0x1B);              // Access 1B register - GYR
  Wire.write(0b00000000);        // Set GYR to +/- 250 deg / second
  Wire.endTransmission();
}


void recordAccRegisters() {
  Wire.beginTransmission(0x68);  // I2C address of MPU
  Wire.write(0x3B);              // Starting register for ACC readings
  Wire.endTransmission();

  Wire.requestFrom(0x68, 6);      // Request registers 59 to 64
  while (Wire.available() < 6);
  ACC_X_raw = Wire.read() << 8 | Wire.read(); // Store first two bytes into ACC_X_raw
  ACC_Y_raw = Wire.read() << 8 | Wire.read(); // Store middle two bytes into ACC_Y_raw
  ACC_Z_raw = Wire.read() << 8 | Wire.read(); // Store last two bytes into ACC_Z_raw
  processAccData();
}

void processAccData() {
  ACC_X = ACC_X_raw * 9.81 / 16384.0;     // ACC Sensitivity Scale Factor = 16384 LSB/g
  ACC_Y = ACC_Y_raw * 9.81 / 16384.0;
  ACC_Z = ACC_Z_raw * 9.81 / 16384.0;
}

void recordGyr_Registers() {
  Wire.beginTransmission(0x68);  // I2C address of MPU
  Wire.write(0x43);              // Starting register for gyr readings
  Wire.endTransmission();

  Wire.requestFrom(0x68, 6);     // Request registers 67 to 72
  while (Wire.available() < 6);
  GYR_X_raw = Wire.read() << 8 | Wire.read(); // Store first two bytes into GYR_X_raw
  GYR_Y_raw = Wire.read() << 8 | Wire.read(); // Store middle two bytes into GYR_Y_raw
  GYR_Z_raw = Wire.read() << 8 | Wire.read(); // Store last two bytes into GYR_Z_raw
  processGyrData();
}

void processGyrData() {
  GYR_X = GYR_X_raw / 131.0;               // gyr Sensitivity = 131 LSB/( degree/s )
  GYR_Y = GYR_Y_raw / 131.0;
  GYR_Z = GYR_Z_raw / 131.0;
}

void recordTemperatureRegister() {
  Wire.beginTransmission(0x68);       // I2C address of MPU
  Wire.write(0x41);                   // Starting register for temperature readings
  Wire.endTransmission();

  Wire.requestFrom(0x68, 2);
  while (Wire.available() < 2);
  temperatureData = Wire.read() << 8 | Wire.read(); // Store temperature
  processTemperatureData();
}

void processTemperatureData() {
  temperature = temperatureData / 333.87 + 21;   // Formula: ((TEMP_OUT – RoomTemp_Offset)/Temp_Sensitivity)+ 21degC
}

void printData() {

  if (DEBUG == true) {
    // Time
    Serial.print("Time "); Serial.print((float) time / 1000);
    
    // Temperature
    Serial.print(" Temperature (degree) "); Serial.print(temperature);

    Serial.print(" ACC (N)");
    Serial.print(" X= "); Serial.print(ACC_X);
    Serial.print(" Y= "); Serial.print(ACC_Y);
    Serial.print(" Z= "); Serial.print(ACC_Z);

    Serial.print(" GYR (deg)");
    Serial.print(" X= "); Serial.print(GYR_X);
    Serial.print(" Y= "); Serial.print(GYR_Y);
    Serial.print(" Z= "); Serial.println(GYR_Z);
    
  } else {
    // Time
    Serial.print((float) time / 1000);
    
    // Temperature
    Serial.print(" "); Serial.print(temperature);

    // ACC
    Serial.print(" "); Serial.print(ACC_X);
    Serial.print(" "); Serial.print(ACC_Y);
    Serial.print(" "); Serial.print(ACC_Z);

    // GYR
    Serial.print(" "); Serial.print(GYR_X);
    Serial.print(" "); Serial.print(GYR_Y);
    Serial.print(" "); Serial.println(GYR_Z);

  }

}
