#include <Arduino.h> //
#include <Wire.h>    // Including Wire,h to comunicate with mpu6050 (I2C comunication)

int Roll, Pitch, Yaw;                  // Roll Pitch and Yaw Variable Initialization
float calibRoll, calibPitch, calibYaw; // Roll Pitch and Yaw calibration Variable Initialization

void gyro_signals(void)
{
  Wire.beginTransmission(0x68); // begin comunication to 0x68 which is the address of our sensor mpu6050
  Wire.write(0x1A);             // setting low pass filter (register)
  Wire.write(0x05);             // 0x05 means 10Hz of filtering (value)
  Wire.endTransmission();       // end transmission to 0x68 at its register 0x1A
  Wire.beginTransmission(0x68); //
  Wire.write(0x1B);             // setting sensitivity (register)
  Wire.write(0x08);             // 0x08 means a sensitivity of 65.5 LSB/°/s (value)
  Wire.endTransmission();       // end transmission to 0x68 at its register 0x1B
  Wire.beginTransmission(0x68); //
  Wire.write(0x43);             // make the access where the measurement are stored
  Wire.endTransmission();       //
  Wire.requestFrom(0x68, 6);    // requesting 6 bytes from the gyroscope

  // the sensor will give us for all axis an unsigned int of 16-bit 2's complement value
  int16_t GyroX = Wire.read() << 8 | Wire.read(); // getting x value
  int16_t GyroY = Wire.read() << 8 | Wire.read(); // getting y value
  int16_t GyroZ = Wire.read() << 8 | Wire.read(); // getting z value

  Roll = (float)GyroX / 65.5;  // converting roll in °/s
  Pitch = (float)GyroY / 65.5; // converting pitch in °/s
  Yaw = (float)GyroZ / 65.5;   // converting yaw in °/s
}

void setup()
{
  Serial.begin(115200);                                       // Serial Monitor Initialization at 115200 baud rate
  Wire.setClock(400000);                                      // setting clock speed of I2C comunication
  Wire.begin();                                               // starting comunication
  delay(250);                                                 // waiting that mpu6050 starts
  Wire.beginTransmission(0x68);                               //
  Wire.write(0x6B);                                           // sending value to the power managment register
  Wire.write(0x00);                                           // 0x00 means that the device will starts
  Wire.endTransmission();                                     //
  for (int measurements; measurements < 2000; measurements++) // starting measurement in cycle for 2000 times
  {                                                           //
    gyro_signals();                                           // getting results
    calibRoll += Roll;                                        // adding to calibration values
    calibPitch += Pitch;                                      //
    calibYaw += Yaw;                                          //
    delay(1);                                                 // waiting 1 millisecond before requesting another value
  }                                                           //
  calibRoll /= 2000;                                          // obtaining the offsets to make the real values to be 0
  calibPitch /= 2000;                                         // when not moving
  calibYaw /= 2000;                                           //
}

void loop()
{
  gyro_signals();

  // apply corrections
  Roll -= calibRoll;
  Pitch -= calibPitch;
  Yaw -= calibYaw;

  // printing them on serial monitor
  Serial.print("Roll= ");
  Serial.printf("%4i\t", Roll);
  Serial.print("Pitch= ");
  Serial.printf("%4i\t", Pitch);
  Serial.print("Yaw= ");
  Serial.printf("%4i\n", Yaw);

  // adding delay to make readable all values on serial monitor
  delay(50);
}