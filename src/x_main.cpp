// #include <PulseSensorPlayground.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <StopWatch.h>
#include <I2Cdev.h>
#include <MPU6050.h>
// #include <ArduinoBLE.h>

const int PULSE_SENSOR_PIN_0 = 0;
const int PULSE_SENSOR_PIN_1 = 1;

const int PulseSensorCheckIntervalMs = 10;
const int AccelGyroSensorCheckIntervalMs = 5;
const int BluetoothSendIntervalMs = 1000;

const int MaxPulse = 150;

const double MinPulseInterval = (60.0 / MaxPulse) * 1000; // ms
const int THRESHOLD_BASE = 510;
const int LastIntervalsSize = 10;
const int PulseSignalStatisticsForHESize = 20;

const int MaxCorrectSignalDelta = 30; // ACCURACY

int Threshold[2] = {510, 510};
int Signal[2] = {0, 0};
int BPM[2] = {0, 0};
int BPM_Average = 0;

bool IsPulse[2] = {false, false};
unsigned long TimeCounter[2] = {0, 0};
unsigned long LastBeatTime[2] = {0, 0};
int LastBeatInterval[2] = {0, 0};
int TimeFromLastBeat[2] = {0, 0};
int Amplitude[2] = {100, 100};
int SignalFloor[2] = {THRESHOLD_BASE, THRESHOLD_BASE};
int SignalPeak[2] = {THRESHOLD_BASE, THRESHOLD_BASE};
int LastIntervals[2][LastIntervalsSize];

// bool IsFirstBeat[2] = {true, true};
bool IsStartBeating[2] = {true, true};
int CountStartingBeats[2] = {0, 0};

double AmplitudeCoefficient[2] = {0.65, 0.65}; // чем он больше, тем выше Threshold от разницы (SignalPeak - SignalFloor)

int PulseErrorNo[2] = {-1, 2};
int PulseSignalStatisticsForHE[2][PulseSignalStatisticsForHESize];

// ----------------------------------------------------------------------------------------------------------------------------------

const int AxelSignalStatisticsForHESize = 30;

int CurrentSteps = 0;

int AxelErrorNo[2] = {-1, -1};
int AxelSignalStatisticsForHE[2][AxelSignalStatisticsForHESize];

// ----------------------------------------------------------------------------------------------------------------------------------

StopWatch PulseSensorTimer;
StopWatch BluetoothSendTimer;
StopWatch AccelGyroTimer;
StopWatch LedDeviceOffTimer;
SoftwareSerial Bluetooth(3, 2); // RX, TX

const int buttonPin = 8; // Пин, к которому подключена кнопка
const int ledPin = 12;   // Пин, к которому подключен светодиод
int buttonState = 0;     // Переменная для хранения состояния кнопки
bool IsDeviceWorking = true;

// MPU6050 AccelGyroSensor[2] = {MPU6050(MPU6050_ADDRESS_AD0_LOW), MPU6050(MPU6050_ADDRESS_AD0_HIGH)};

void ResetVars(int sensorNum);
void RecalculateThreshold(int sensorNum);
void HandleErrors();
void PrintVars(int sensorNum);
void PrintSignalStatisticsForHE(int sensorNum);
void ProcessPulseSignal(int indx);
double CalculateVariance(int arr[LastIntervalsSize]);
void CountAverageBPM();

void setup()
{
    Serial.begin(9600);
    Bluetooth.begin(9600);

    pinMode(buttonPin, INPUT_PULLUP); // Настройка пина как вход с подтягивающим резистором
    pinMode(ledPin, OUTPUT);          // Настройка пина светодиода как выход

    // AccelGyroSensor[0].initialize();
    // AccelGyroSensor[1].initialize();

    // if (AccelGyroSensor[0].testConnection() == true)
    // {
    //     Serial.println("AccelGyroSensor[0]");
    // }
    // else
    // {
    //     Serial.println("q123123");
    // }
    // if (AccelGyroSensor[1].testConnection() == true)
    // {
    //     Serial.println("AccelGyroSensor[1]");
    // }
    // else
    // {
    //     Serial.println("ergergerg");
    // }

    PulseSensorTimer.start();
    BluetoothSendTimer.start();
    AccelGyroTimer.start();
    // ResetVars(-1);
    for (int sensorNum = 0; sensorNum < 2; sensorNum++)
    {
        for (int i = 0; i < PulseSignalStatisticsForHESize; i++)
        {
            PulseSignalStatisticsForHE[sensorNum][i] = -1;
        }
        for (int i = 0; i < LastIntervalsSize; i++)
        {
            LastIntervals[sensorNum][i] = 0;
        }
    }
}

void loop()
{
    buttonState = digitalRead(buttonPin);

    if (buttonState == LOW)
    {
        IsDeviceWorking = !IsDeviceWorking;
        digitalWrite(ledPin, HIGH);
        delay(1000);
        digitalWrite(ledPin, LOW);
        if (!IsDeviceWorking)
        {
            LedDeviceOffTimer.start();
        }
    }

    if (IsDeviceWorking)
    {
        if (PulseSensorTimer.elapsed() >= PulseSensorCheckIntervalMs)
        {
            PulseSensorTimer.reset();
            Signal[0] = analogRead(PULSE_SENSOR_PIN_0);
            Signal[1] = analogRead(PULSE_SENSOR_PIN_1);

            HandleErrors();
            // Serial.println("Signal[0] = " + String(Signal[0]));
            // Serial.println(Threshold_0);

            if (PulseErrorNo[0] == -1)
            {
                ProcessPulseSignal(0);
            }
            if (PulseErrorNo[1] == -1)
            {
                ProcessPulseSignal(1);
            }

            if (PulseErrorNo[0] != -1)
            {
                IsStartBeating[0] = false;
                CountStartingBeats[0] = 0;
                Serial.println("Error[0] = " + String(PulseErrorNo[0]));
                PrintSignalStatisticsForHE(0);
            }
            if (PulseErrorNo[1] != -1)
            {
                IsStartBeating[1] = false;
                CountStartingBeats[1] = 0;
                Serial.println("Error[1] = " + String(PulseErrorNo[1]));
                PrintSignalStatisticsForHE(1);
            }

            PulseSensorTimer.start();
        }

        // if (AccelGyroTimer.elapsed() > AccelGyroSensorCheckIntervalMs)
        // {
        //     AccelGyroTimer.reset();
        //     int q = AccelGyroSensor[0].getAccelerationX();
        //     Serial.println("AccelGyroSensor[0] X = " + String(q));
        //     int w = AccelGyroSensor[1].getAccelerationX();
        //     Serial.println("AccelGyroSensor[1] X = " + String(w));
        //     AccelGyroTimer.start();
        // }

        if (BluetoothSendTimer.elapsed() > BluetoothSendIntervalMs)
        {
            BluetoothSendTimer.reset();

            /*
            |FT|BPM|steps|error num|
            |FT|xxx|xxxxx|XX|err_XX_XX_XX|
            */

            CountAverageBPM();

            char buffer[100];
            String response = "|FT|";

            if (BPM_Average < 100)
            {
                response += "0";
            }
            response += String(BPM_Average);
            response += "|";

            if (CurrentSteps < 10)
            {
                response += "0000";
            }
            else if (10 <= CurrentSteps && CurrentSteps < 100)
            {
                response += "000";
            }
            else if (100 <= CurrentSteps && CurrentSteps < 1000)
            {
                response += "00";
            }
            else if (1000 <= CurrentSteps && CurrentSteps < 10000)
            {
                response += "0";
            }
            response += String(CurrentSteps);
            response += "|";

            int errorNum = 0;
            String errors = "";
            for (int i = 0; i < 2; i++)
            {
                if (PulseErrorNo[i] == 1 || PulseErrorNo[i] == 2)
                {
                    errors += "err_ps_0" + String(i + 1) + "_0" + String(PulseErrorNo[i]) + "|";
                    errorNum++;
                }

                if (AxelErrorNo[i] == 1)
                {
                    errors += "err_ax_0" + String(i + 1) + "_0" + String(AxelErrorNo[i]) + "|";
                    errorNum++;
                }
            }

            response += "0";
            response += String(errorNum);
            response += "|";

            response += errors;
            response += "\n";

            response.toCharArray(buffer, 100);

            Bluetooth.write(buffer);
            Serial.println(response);

            BluetoothSendTimer.start();
        }
    }
    else
    {
        if (LedDeviceOffTimer.elapsed() >= 2000)
        {
            LedDeviceOffTimer.reset();
            digitalWrite(ledPin, HIGH);
            delay(100);
            digitalWrite(ledPin, LOW);
            LedDeviceOffTimer.start();
        }
    }
}

void CountAverageBPM()
{
    if (PulseErrorNo[0] == -1 && PulseErrorNo[1] == -1)
    {
        double Weight_0 = 1 / CalculateVariance(LastIntervals[0]);
        double Weight_1 = 1 / CalculateVariance(LastIntervals[1]);

        BPM_Average = (int)floor((Weight_0 * BPM[0] + Weight_1 * BPM[1]) / (Weight_0 + Weight_1));
    }
    else if (PulseErrorNo[0] == -1 && PulseErrorNo[1] != -1)
    {
        BPM_Average = BPM[0];
    }
    else if (PulseErrorNo[0] != -1 && PulseErrorNo[1] == 1)
    {
        BPM_Average = BPM[1];
    }
    else
    {
        BPM_Average = 0;
    }
}

void HandleErrors()
{
    for (int sensorNum = 0; sensorNum < 2; sensorNum++)
    {
        for (int i = 0; i < 19; i++)
        {
            PulseSignalStatisticsForHE[sensorNum][i] = PulseSignalStatisticsForHE[sensorNum][i + 1];
        }
        PulseSignalStatisticsForHE[sensorNum][19] = Signal[sensorNum];

        for (int i = 0; i < PulseSignalStatisticsForHESize; i++)
        {
            if (PulseSignalStatisticsForHE[sensorNum][i] == -1)
            {
                return;
            }
        }

        int countInvalidNums = 0;
        for (int i = 0; i < PulseSignalStatisticsForHESize; i++)
        {
            if (abs(Threshold[sensorNum] - THRESHOLD_BASE) < MaxCorrectSignalDelta) // correct Threshold[]
            {
                if (abs(PulseSignalStatisticsForHE[sensorNum][i] - Threshold[sensorNum]) >= MaxCorrectSignalDelta &&
                    (PulseSignalStatisticsForHE[sensorNum][i] < SignalFloor[sensorNum] || PulseSignalStatisticsForHE[sensorNum][i] > SignalPeak[sensorNum]))
                {
                    countInvalidNums++;
                }
            }
            else // incorrect Threshold[]
            {
                if (abs(PulseSignalStatisticsForHE[sensorNum][i] - THRESHOLD_BASE) >= MaxCorrectSignalDelta &&
                    (PulseSignalStatisticsForHE[sensorNum][i] < SignalFloor[sensorNum] || PulseSignalStatisticsForHE[sensorNum][i] > SignalPeak[sensorNum]))
                {
                    countInvalidNums++;
                }
            }
        }

        if (countInvalidNums <= 1)
        {
            PulseErrorNo[sensorNum] = -1;
        }
        else if (2 <= countInvalidNums && countInvalidNums <= 5)
        {
            PulseErrorNo[sensorNum] = 1;
        }
        else if (6 <= countInvalidNums)
        {
            PulseErrorNo[sensorNum] = 2;
        }
    }
}

void PrintSignalStatisticsForHE(int sensorNum)
{
    for (int i = 0; i < PulseSignalStatisticsForHESize; i++)
    {
        Serial.print(String(PulseSignalStatisticsForHE[sensorNum][i]) + " | ");
    }
    Serial.println();
    Serial.println();
}

void ProcessPulseSignal(int indx)
{
    TimeCounter[indx] += PulseSensorCheckIntervalMs;
    TimeFromLastBeat[indx] = TimeCounter[indx] - LastBeatTime[indx];

    if (Signal[indx] < Threshold[indx] && TimeFromLastBeat[indx] > LastBeatInterval[indx] * 0.6)
    {
        if (Signal[indx] < SignalFloor[indx])
        {
            SignalFloor[indx] = Signal[indx];
        }
    }

    if (Signal[indx] > Threshold[indx] && Signal[indx] > SignalPeak[indx])
    {
        SignalPeak[indx] = Signal[indx];
    }

    if (TimeFromLastBeat[indx] > MinPulseInterval)
    {
        if (Signal[indx] > Threshold[indx] && IsPulse[indx] == false && TimeFromLastBeat[indx] > LastBeatInterval[indx] * 0.6)
        {
            IsPulse[indx] = true;
            LastBeatInterval[indx] = TimeCounter[indx] - LastBeatTime[indx];
            LastBeatTime[indx] = TimeCounter[indx];

            int totalSumIntervals = 0;
            for (int i = 0; i < 9; i++)
            {
                LastIntervals[indx][i] = LastIntervals[indx][i + 1];
                totalSumIntervals += LastIntervals[indx][i];
            }
            LastIntervals[indx][9] = LastBeatInterval[indx];

            if (LastIntervals[indx][0] == 0)
            {
                CountStartingBeats[indx]++;
                BPM[indx] = -1;
                Serial.println("Counting BPM... | " + String(10 - CountStartingBeats[indx]) + " beats left");
            }
            else
            {
                totalSumIntervals += LastBeatInterval[indx];
                BPM[indx] = 600000 / totalSumIntervals;
                Serial.println("=======BEAT=======");
                PrintVars(indx);
            }
        }
    }

    if (Signal[indx] < Threshold[indx] && IsPulse[indx] == true)
    {
        IsPulse[indx] = false;
        RecalculateThreshold(indx);
    }

    if (TimeFromLastBeat[indx] > 2000)
    {
        Serial.println("Reset!\n");
        ResetVars(indx);
        RecalculateThreshold(indx);
    }
}

void ResetVars(int sensorNum)
{
    if (sensorNum == 0 || sensorNum == 1)
    {
        LastBeatInterval[sensorNum] = 750;
        IsPulse[sensorNum] = false;
        TimeCounter[sensorNum] = 0;
        LastBeatTime[sensorNum] = 0;
    }
    else if (sensorNum == -1)
    {
        for (int i = 0; i < 2; i++)
        {
            LastBeatInterval[i] = 750;
            IsPulse[i] = false;
            TimeCounter[i] = 0;
            LastBeatTime[i] = 0;
        }
    }
}

void RecalculateThreshold(int sensorNum)
{
    if (sensorNum == 0 || sensorNum == 1)
    {
        Amplitude[sensorNum] = SignalPeak[sensorNum] - SignalFloor[sensorNum];
        int NewThreshold = SignalFloor[sensorNum] + (int)floor(Amplitude[sensorNum] * AmplitudeCoefficient[sensorNum]);
        if (abs(NewThreshold - Threshold[sensorNum]) <= MaxCorrectSignalDelta)
        {
            Threshold[sensorNum] = SignalFloor[sensorNum] + (int)floor(Amplitude[sensorNum] * AmplitudeCoefficient[sensorNum]);
        }
        SignalFloor[sensorNum] = Threshold[sensorNum];
        SignalPeak[sensorNum] = Threshold[sensorNum];
    }
    else if (sensorNum == -1)
    {
        for (int i = 0; i < 2; i++)
        {
            Amplitude[sensorNum] = SignalPeak[sensorNum] - SignalFloor[sensorNum];
            int NewThreshold = SignalFloor[sensorNum] + (int)floor(Amplitude[sensorNum] * AmplitudeCoefficient[sensorNum]);
            if (abs(NewThreshold - Threshold[sensorNum]) <= MaxCorrectSignalDelta)
            {
                Threshold[sensorNum] = SignalFloor[sensorNum] + (int)floor(Amplitude[sensorNum] * AmplitudeCoefficient[sensorNum]);
            }
            Threshold[sensorNum] = SignalFloor[sensorNum] + (int)floor(Amplitude[sensorNum] * AmplitudeCoefficient[sensorNum]);
            SignalFloor[sensorNum] = Threshold[sensorNum];
            SignalPeak[sensorNum] = Threshold[sensorNum];
        }
    }
}

double CalculateVariance(int arr[LastIntervalsSize])
{
    double sum = 0.0;
    double mean = 0.0;
    double variance = 0.0;

    for (int i = 0; i < LastIntervalsSize; i++)
    {
        sum += arr[i];
    }

    mean = sum / LastIntervalsSize;

    for (int i = 0; i < LastIntervalsSize; i++)
    {
        variance += (arr[i] - mean) * (arr[i] - mean);
    }

    variance /= LastIntervalsSize;

    return variance;
}

void CalculateAverageBPM()
{
    double vars[2] = {0.0, 0.0};

    for (int i = 0; i < 2; i++)
    {
        vars[i] = CalculateVariance(LastIntervals[i]);
    }

    Serial.println(vars[0]);
}

void PrintVars(int sensorNum)
{
    if (sensorNum == 0 || sensorNum == 1)
    {
        Serial.println("TimeFromLastBeat = " + String(TimeFromLastBeat[sensorNum]));
        Serial.println("Signal = " + String(Signal[sensorNum]));
        Serial.println("SignalFloor = " + String(SignalFloor[sensorNum]));
        Serial.println("SignalPeak = " + String(SignalPeak[sensorNum]));
        Serial.println("BPM = " + String(BPM[sensorNum]));
        Serial.print("Rate: ");
        for (int i = 0; i < LastIntervalsSize; i++)
        {
            Serial.print(String(LastIntervals[sensorNum][i]) + ", ");
        }
        Serial.println();
        Serial.println();
    }
    else if (sensorNum == -1)
    {
        for (int i = 0; i < 2; i++)
        {
            Serial.println("TimeFromLastBeat = " + String(TimeFromLastBeat[i]));
            Serial.println("Signal = " + String(Signal[i]));
            Serial.println("SignalFloor = " + String(SignalFloor[sensorNum]));
            Serial.println("SignalPeak = " + String(SignalPeak[sensorNum]));
            Serial.println("BPM = " + String(BPM[i]));
            Serial.print("Rate: ");
            for (int j = 0; j < LastIntervalsSize; j++)
            {
                Serial.print(String(LastIntervals[sensorNum][j]) + ", ");
            }
            Serial.println();
            Serial.println();
        }
    }
}