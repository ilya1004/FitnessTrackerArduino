// #include "Wire.h"
// // #include <Wire.h>
// // #include <SoftwareSerial.h>
// // #include <StopWatch.h>
// #include <I2Cdev.h>
// // #include <MPU6050.h>


// const int MPU_addr = 0x68; // адрес датчика
// const int MPU_addr1 = 0x69; // адрес датчика
// // массив данных
// // [accX, accY, accZ, temp, gyrX, gyrY, gyrZ]
// // acc - ускорение, gyr - угловая скорость, temp - температура (raw)
// int16_t data[7];
// int16_t data1[7];
// void setup()
// {
//     // инициализация
//     Wire.begin();
//     Wire.beginTransmission(MPU_addr);
//     Wire.write(0x6B); // PWR_MGMT_1 register
//     Wire.write(0);    // set to zero (wakes up the MPU-6050)
//     Wire.endTransmission(true);

//     Wire.begin();
//     Wire.beginTransmission(MPU_addr1);
//     Wire.write(0x6B); // PWR_MGMT_1 register
//     Wire.write(0);    // set to zero (wakes up the MPU-6050)
//     Wire.endTransmission(true);

//     Serial.begin(9600);
// }

// void loop()
// {
//     getData(); // получаем
//     // выводим
//     for (byte i = 0; i < 7; i++)
//     {
//         Serial.print(data[i]);
//         Serial.print('\t');
//     }
//     Serial.println();
//     for (byte i = 0; i < 7; i++)
//     {
//         Serial.print(data1[i]);
//         Serial.print('\t');
//     }
//     Serial.println();
//     Serial.println();
//     delay(200);
// }
// void getData()
// {
//     Wire.beginTransmission(MPU_addr);
//     Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H)
//     Wire.endTransmission(false);
//     Wire.requestFrom(MPU_addr, 14, true); // request a total of 14 registers
//     for (byte i = 0; i < 7; i++)
//     {
//         data[i] = Wire.read() << 8 | Wire.read();
//     }

//     Wire.beginTransmission(MPU_addr1);
//     Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H)
//     Wire.endTransmission(false);
//     Wire.requestFrom(MPU_addr1, 14, true); // request a total of 14 registers
//     for (byte i = 0; i < 7; i++)
//     {
//         data1[i] = Wire.read() << 8 | Wire.read();
//     }
// }