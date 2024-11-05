// #include <PulseSensorPlayground.h>
// #include <I2Cdev.h>
// #include <StopWatch.h>

// int PulseSensorPin = 0; // Pulse Sensor PURPLE WIRE connected to ANALOG PIN 0
// int LED = LED_BUILTIN;  // 13. The on-board Arduion LED

// int ButtonPin = 8;
// bool lastButtonState = HIGH;
// bool buttonPressed = false;
// bool PulseCountingState = true;

// int Signal_value;    // holds the incoming raw data. Signal value can range from 0-1024
// int Threshold = 511; // Determine which Signal to "count as a beat", and which to ingore.
// int TopBound = 520;  // Верхняя граница

// int DelayDefault = 470; // Пульс до 120 (60/120) по умолчанию.
// int DelayCheck = 30;
// // DelayDefault + DelayCheck = 500ms

// unsigned int lastBeatTime = 0;
// int pulseRate = 0;

// StopWatch stopwatch;
// bool runnig = false;

// void setup()
// {
//     pinMode(LED, OUTPUT);
//     pinMode(ButtonPin, INPUT_PULLUP);
//     // attachInterrupt(digitalPinToInterrupt(ButtonPin), handleButtonPress, FALLING);
//     Serial.begin(115200);
// }

// void loop()
// {
//     bool currentButtonState = digitalRead(ButtonPin);

//     if (lastButtonState == HIGH && currentButtonState == LOW)
//     {
//         buttonPressed = true;
//         PulseCountingState = !PulseCountingState;
//         delay(100);
//     }
//     if (currentButtonState == HIGH)
//     {
//         buttonPressed = false;
//     }
//     lastButtonState = currentButtonState;

//     // Serial.println("State = " + String(PulseCountingState));

//     if (PulseCountingState == true)
//     {
//         buttonPressed = false;

//         Signal_value = analogRead(PulseSensorPin);
//         // Serial.println("Signal " + String(Signal_value));

//         if (Signal_value > Threshold && Signal_value <= TopBound)
//         {
//             unsigned long currentTime = millis(); // Get current time

//             // Check if it's a new beat
//             if (currentTime - lastBeatTime > (DelayDefault + DelayCheck))
//             {
//                 if (currentTime < lastBeatTime)
//                 {
//                     // Handle case where millis() overflowed
//                     lastBeatTime = currentTime; // Reset lastBeatTime
//                 }

//                 // Calculate time between beats
//                 unsigned long long timeBetweenBeats = currentTime - lastBeatTime;

//                 // Calculate pulse rate (in beats per minute)
//                 pulseRate = 60000 / timeBetweenBeats; // Convert ms to min

//                 Serial.println("\n=======BEAT=======: " + String(Signal_value));
//                 Serial.println("Pulse Rate: " + String(pulseRate) + " bpm");
//                 Serial.println("Current Time:" + String(currentTime));

//                 lastBeatTime = currentTime; // Update last beat time

//                 digitalWrite(LED, HIGH);
//                 delay(100);
//                 digitalWrite(LED, LOW);
//                 delay(DelayDefault - 100);
//             }
//         }
//         else
//         {
//             Serial.print(".");
//             digitalWrite(LED, LOW);
//         }
//         delay(DelayCheck);
//     }
// }