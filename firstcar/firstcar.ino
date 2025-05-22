#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "AFMotor.h"

// ========== HARDWARE DEFINITIONS ==========
LiquidCrystal_I2C lcd(0x27, 16, 2);
AF_DCMotor frontLeftMotor(1);
AF_DCMotor backLeftMotor(2);
AF_DCMotor frontRightMotor(3);
AF_DCMotor backRightMotor(4);
#define bluetooth Serial2 // Using Serial2 (pins 17 RX, 16 TX on Mega)
#define wifi Serial3 // Using Serial3 (pins 15 RX, 14 TX on Mega)
 
// ========== PIN DEFINITIONS ==========
#define ENCODER_BL 18
#define ENCODER_BR 19
#define LEFT_SENSOR 27
#define RIGHT_SENSOR 23
#define TRIG_PIN 29
#define ECHO_PIN 28

// ========== GLOBAL VARIABLES ==========
volatile int countBL = 0;
volatile int countBR = 0;
const float WHEEL_DIAMETER = 6.5;
int defaultSpeed = 100;
unsigned long prevTime = 0;
unsigned long prevTime2 = 0;
const int SLOTS_PER_REV = 20;
float actualSpeed;
String data = "Testing";
float speedBL = 0, speedBR = 0;
float errorspeed;
unsigned long lastPidTime = 0;
#define SAMPLE_INTERVAL_MS 100  // PID update interval in ms
#define WHEEL_DIAMETER_CM 6.5   // wheel diameter in cm
#define WHEEL_CIRCUM_CM (WHEEL_DIAMETER_CM * 3.14159)

volatile unsigned int pulsesLeft = 0;
volatile unsigned int pulsesRight = 0;

float setSpeedCmS = defaultSpeed;  // desired vehicle speed in cm/s




// --- Configuration -----------------------------------
const int BUFFER_SIZE = 10;    // number of samples for rolling average

// --- Rolling Average -----------------------------------------
float distanceBuffer[BUFFER_SIZE];  // circular buffer of last readings
int samplesCollected = 0;           // how many samples we’ve actually filled

// Moving Average Filter (for Stability)
#define FILTER_SIZE 5
float speedBLBuffer[FILTER_SIZE] = { 0 };
float speedBRBuffer[FILTER_SIZE] = { 0 };

float kp = 50.0;
const float ki = 10;  // Integral Gain
const float kd = 5; 

int filterIndex = 0;

// PID Constants
float error = 0;

// function prototypes
float GetAvrgdistance();
float distance_glb;

void setup()
{
    Serial.begin(9600);
    wifi.begin(115200);
    bluetooth.begin(9600); // Match with HC-05

    // Sensor Setup
    pinMode(LEFT_SENSOR, INPUT);
    pinMode(RIGHT_SENSOR, INPUT);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    // Encoder Interrupts
    pinMode(ENCODER_BL, INPUT_PULLUP);
    pinMode(ENCODER_BR, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(ENCODER_BL), countEncoderBL, RISING);
    attachInterrupt(digitalPinToInterrupt(ENCODER_BR), countEncoderBR, RISING);

    // LCD Setup
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Set:");
    lcd.print(defaultSpeed);
    lcd.setCursor(0, 1);
    lcd.print("Act:0.0cm/s D:---");

    // Serial.println("System Ready");
    bluetooth.println("Bluetooth Ready");
    
  // initialize buffer to zero
  for (int i = 0; i < BUFFER_SIZE; i++) {
    distanceBuffer[i] = 0.0;
  }
}

unsigned long Previous_millis = 0;
unsigned long interval = 2000; // 1 second interval

void loop()
{
    handleBluetooth();
    updateSensorsAndControl();
    updateDisplay();
    // Capture values only once
    float distance = getDistance();

    actualSpeed = calculateSpeed(); // Fresh value from encoders

    // Send formatted values to ESP
    String combined = "speed:" + String(actualSpeed, 2) + ",distance:" + String(distance, 1);
    wifi.println(combined);
    // delay(20);

    // Also print to local Serial Monitor
    // Serial.println("Sending to ESP:");
    // Serial.println(combined);
    // Serial.println(distanceMsg);

    //   delay(2000); // Wait before next transmission

    unsigned long current_millis = millis();
    if (current_millis - Previous_millis >= interval)
    {
        Previous_millis = current_millis;

        // Receive from ESP if available
        if (wifi.available())
        {
            String received = wifi.readStringUntil('\n');
            // Serial.println("Received from ESP: " + received);
        }

        // Serial.println(F("\n\n seconds passed"));
        // Add any periodic tasks here
    }
    // Serial.println("");
}

// ========== NON-BLOCKING BLUETOOTH HANDLING ==========
void handleBluetooth()
{
    static String command = "";

    while (bluetooth.available())
    {
        char c = bluetooth.read();

        if (c == '\n')
        {
            command.trim();
            processBluetoothCommand(command);
            command = "";
        }
        else
        {
            command += c;
        }
    }
}

void processBluetoothCommand(String cmd)
{
    // Serial.println("BT Received: " + cmd);

    if (cmd.startsWith("S") && cmd.length() > 1)
    {
        int newSpeed = cmd.substring(1).toInt();
        if (newSpeed >= 0 && newSpeed <= 255)
        {
            defaultSpeed = newSpeed;

            lcd.setCursor(4, 0);
            lcd.print("    ");
            lcd.setCursor(4, 0);
            lcd.print(defaultSpeed);

            bluetooth.print("Speed Set:");
            bluetooth.println(defaultSpeed);
        }
        else
        {
            bluetooth.println("Error: Speed 0-255");
        }
    }
}

// ========== SENSOR & MOTOR CONTROL ==========
void updateSensorsAndControl()
{
    // float distance = getDistance();
    distance_glb = GetAvrgdistance();
    bool left = digitalRead(LEFT_SENSOR);
    bool right = digitalRead(RIGHT_SENSOR);

    if (!left && !right)
    {
        // moveForward(distance);
        moveForward(distance_glb);
        // adjustSpeed();
    }
    else if (!left && right)
    {
        turnLeft(defaultSpeed);
    }
    else if (left && !right)
    {
        turnRight(defaultSpeed);
    }
    else
    {
        stopMotors();
    }

    // PID error monitoring (optional)
    error = countBL - countBR;
    // Serial.print("Encoder Counts - BL: ");
    // Serial.print(countBL);
    // Serial.print(" BR: ");
    // Serial.print(countBR);
}

// ========== DISPLAY UPDATE ==========
void updateDisplay()
{
    if (millis() - prevTime >= 1000)
    {
        prevTime = millis();
        actualSpeed = calculateSpeed();

        lcd.setCursor(4, 1);
        lcd.print("    ");
        lcd.setCursor(4, 1);
        lcd.print(actualSpeed, 1);
        lcd.print("cm/s");

        float distance = distance_glb ;
        //  float distance = getDistance();
        lcd.setCursor(12, 1);
        if (distance < 0 || distance > 400)
        {
            lcd.print("---");
        }
        else
        {
            lcd.print(distance, 0);
            lcd.print("cm ");
        }

        // Serial.print("Actual Speed: ");
        // Serial.print(actualSpeed);
        // Serial.println(" m/s");
    }
}


// ========== MOTOR FUNCTIONS ==========
void moveForward(float distance)
{
    actualSpeed  = calculateSpeed();
  float error = (defaultSpeed / 255.0) - actualSpeed;
  int correction = error * 50;  // Proportional Gain (Tune this value)
  int correctedSpeed = constrain(defaultSpeed + correction, 0, 255);
  errorspeed = 10 * correctedSpeed / kd;
  
    int speed = defaultSpeed;
  // if (distance > 2 && distance <= 10) speed = 0;
  if (distance <= 10) speed = 0;

  // when more thn 10 
  else if (distance > 11 && distance <= 25) speed = 40;
  else if (distance > 26 && distance <= 40) speed = 70;
  else speed = correctedSpeed; // distance >= 40
//   setMotorSpeed(correctedSpeed, correctedSpeed, correctedSpeed, correctedSpeed);
    setMotorSpeed(speed, speed, speed, speed);
    Serial.print("Wheel Speed: ");
    Serial.println(speed);
    // setMotorSpeed(90, 90, 90, 90);
}

void turnLeft(int speed)
{
    setMotorSpeed(-speed, -speed, speed, speed);
}

void turnRight(int speed)
{
    setMotorSpeed(speed, speed, -speed, -speed);
}

void stopMotors()
{
    setMotorSpeed(0, 0, 0, 0);
}

void setMotorSpeed(int fl, int bl, int fr, int br)
{
    frontLeftMotor.setSpeed(abs(fl));
    backLeftMotor.setSpeed(abs(bl));
    frontRightMotor.setSpeed(abs(fr));
    backRightMotor.setSpeed(abs(br));

    frontLeftMotor.run(fl >= 0 ? FORWARD : BACKWARD);
    backLeftMotor.run(bl >= 0 ? FORWARD : BACKWARD);
    frontRightMotor.run(fr >= 0 ? FORWARD : BACKWARD);
    backRightMotor.run(br >= 0 ? FORWARD : BACKWARD);
}

// ========== ENCODER FUNCTIONS ==========
void countEncoderBL()
{
    countBL++;
    pulsesLeft++;
}

void countEncoderBR()
{
    countBR++;
    pulsesRight++;
}

float calculateSpeed()
{
  unsigned long now = millis();
  unsigned long dt = now - lastPidTime;

  if (dt >= SAMPLE_INTERVAL_MS) {
    noInterrupts();
    unsigned int cntL = pulsesLeft;
    unsigned int cntR = pulsesRight;
    pulsesLeft = 0;
    pulsesRight = 0;
    interrupts();

    // calculate wheel speed
    float secs = dt / 1000.0;
    float speedLeft = (cntL / (float)SLOTS_PER_REV) * WHEEL_CIRCUM_CM / secs;
    float speedRight = (cntR / (float)SLOTS_PER_REV) * WHEEL_CIRCUM_CM / secs;
    actualSpeed = (speedLeft + speedRight);
 

    // debug output
    // Serial.print("Set(cm/s): ");
    // Serial.print(setSpeedCmS);
    // Serial.print(" | Actual(cm/s): ");
    // Serial.print(actualSpeed, 1);
    // Serial.print(" | PWM: ");
    // Serial.print(lastPwm); 

    lastPidTime = now;



    speedBLBuffer[filterIndex] = speedLeft;
    speedBRBuffer[filterIndex] = speedRight;
    filterIndex = (filterIndex + 1) % FILTER_SIZE;

    speedBL = computeAverage(speedBLBuffer);
    speedBR = computeAverage(speedBRBuffer);

    // int realSpeed = (speedBL + speedBR) / 2.0;
    // Serial.print("\n\n realSpeed");
    // Serial.print(realSpeed);
    return actualSpeed;
    }
}


// void adjustSpeed() {
//   float error = (defaultSpeed / 255.0) * 2.0 - actualSpeed;
//   int correction = error * 50;  // Proportional Gain (Tune this value)
//   int correctedSpeed = constrain(defaultSpeed + correction, 0, 255);
//   errorspeed = 10 * correctedSpeed / kd;
//   setMotorSpeed(correctedSpeed, correctedSpeed, correctedSpeed, correctedSpeed);
// }

float computeAverage(float buffer[]) {
  float sum = 0;
  for (int i = 0; i < FILTER_SIZE; i++) {
    sum += buffer[i];
  }
  return sum / FILTER_SIZE;
}

// ========== ULTRASONIC SENSOR ==========
float getDistance()
{
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    long duration = pulseIn(ECHO_PIN, HIGH, 30000);
    float distance = duration * 0.034 / 2;
    return (distance < 2.0 || distance > 400) ? -1 : distance;
}


float GetAvrgdistance(){
  float newDistance = getDistance();

  // 2. Shift buffer right, dropping the oldest value at the end
  for (int i = BUFFER_SIZE - 1; i > 0; i--) {
    distanceBuffer[i] = distanceBuffer[i - 1];
  }
  // 3. Insert newest reading at buffer[0]
  distanceBuffer[0] = newDistance;

  // 4. Track how many real samples we have (up to BUFFER_SIZE)
  if (samplesCollected < BUFFER_SIZE) {
    samplesCollected++;
  }

  // 5. Compute average of the values we have
  float sum = 0;
  for (int i = 0; i < samplesCollected; i++) {
    sum += distanceBuffer[i];
  }
  float averageDistance = sum / samplesCollected;

  // 6. Print the averaged distance
  Serial.print("averageDistance = ");
//   Serial.println(averageDistance, 2);  // 2 decimal places

  return averageDistance;
}
