// #include <PulseSensorPlayground.h>
// #include <I2Cdev.h>
// #include <StopWatch.h>

// int PulseSensorPin = 0; // Pulse Sensor PURPLE WIRE connected to ANALOG PIN 0
// int LED = LED_BUILTIN;  // 13. The on-board Arduion LED

// int Signal;    // holds the incoming raw data. Signal value can range from 0-1024
// int Threshold = 512; // Determine which Signal to "count as a beat", and which to ingore (включая)
// int TopBound = 520;  // Верхняя граница (включая)

// int DelayDefault = 270; // Пульс до 120 (60/120) по умолчанию.
// int DelayCheck = 30;
// // DelayDefault + DelayCheck = 500ms

// unsigned int lastBeatTime = 0;
// int pulseRate = 0;

// PulseSensorPlayground pulseSensor;

// void setup()
// {
//     pulseSensor.analogInput(PulseSensorPin, 1);
//     pulseSensor.blinkOnPulse(LED, 1);
//     pulseSensor.setThreshold(Threshold);
//     // pinMode(LED, OUTPUT);
//     Serial.begin(115200);
// }

// void loop()
// {
//     Signal = analogRead(PulseSensorPin);
//     Serial.println("Signal " + String(Signal));

//     if (Threshold <= Signal && Signal <= TopBound)
//     {
//         unsigned long currentTime = millis(); // Get current time

//         if (currentTime - lastBeatTime > (DelayDefault + DelayCheck))
//         {
//             unsigned long timeBetweenBeats = currentTime - lastBeatTime;

//             pulseRate = 60000 / timeBetweenBeats; // Convert ms to min

//             Serial.println("\n=======BEAT=======: " + String(Signal));
//             Serial.println("Pulse Rate: " + String(pulseRate) + " bpm");
//             Serial.println("Time Between Beats: " + String(timeBetweenBeats));

//             lastBeatTime = currentTime;

//             digitalWrite(LED, HIGH);
//             delay(100);
//             digitalWrite(LED, LOW);
//             delay(DelayDefault - 100);
//         }
//     }
//     else
//     {
//         digitalWrite(LED, LOW);
//     }
//     delay(DelayCheck);
// }
