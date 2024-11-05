// // #include "I2Cdev.h"
// #include "MPU6050.h"

// #define TIME_OUT 100

// MPU6050 accgyro = MPU6050(MPU6050_ADDRESS_AD0_HIGH);
// unsigned long int t1;

// void setup() {
//     Serial.begin(9600);
//     accgyro.initialize();

    
// }

// void loop() {
//     long int t = millis();
//     if( t1 < t ){
//         int16_t ax, ay, az, gx, gy, gz;

//         t1 = t + TIME_OUT;
//         accgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

//         Serial.println(ax);
//     }
// }
