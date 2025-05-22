// === Pin Definitions ===
#define leftSensorPin A0     // Left IR sensor
#define rightSensorPin A1    // Right IR sensor
#define middleSensorPin A4   // Middle IR sensor

// Motor control pins
const int IN1 = 2;
const int IN2 = 3;
const int IN3 = 4;
const int IN4 = 5;
const int ENA = 9;   // Left motor speed
const int ENB = 10;  // Right motor speed

// === Motor Speed Settings ===
int baseSpeed = 100;   // Normal speed
int turnSpeed = 120;   // Turning speed
// Threshold for IR sensor (tune as needed)
const int threshold = 512;

void setup() {
  // Start serial communication
  Serial.begin(9600);

  // Set motor pins as outputs
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // IR sensor pins as input
  pinMode(leftSensorPin, INPUT);
  pinMode(rightSensorPin, INPUT);
  pinMode(middleSensorPin, INPUT);

  // Stop motors at start
  stopMoving();
}

void loop() {
  // Read IR sensor values
  int leftSensorValue = analogRead(leftSensorPin);
  int rightSensorValue = analogRead(rightSensorPin);
  int middleSensorValue = analogRead(middleSensorPin);

  // Determine if sensors detect the line (black)
  bool leftOnLine = leftSensorValue < threshold;
  bool rightOnLine = rightSensorValue < threshold;
  bool middleOnLine = middleSensorValue < threshold;

  // Debug prints
  Serial.print("Left: ");
  Serial.print(leftSensorValue);
  Serial.print(" | Middle: ");
  Serial.print(middleSensorValue);
  Serial.print(" | Right: ");
  Serial.println(rightSensorValue);

  if (!middleOnLine) {
    Serial.println("Moving Forward");
    moveForward2();
  } 
    else if (!middleOnLine && !leftOnLine && rightOnLine) {
      Serial.println("Correcting Right");
      turnRight();
    } else if (!middleOnLine && !rightOnLine && leftOnLine) {
      Serial.println("Correcting Left");
      turnLeft();
    } else if (middleOnLine && !rightOnLine && leftOnLine) {
      Serial.println("Correcting Left");
      turnLeft();
    } else if (middleOnLine && rightOnLine && !leftOnLine) {
      Serial.println("Correcting Left");
      turnRight();
    }
    else if (leftOnLine && rightOnLine) {
      Serial.println("At junction - stopping or deciding");
      stopMoving(); // Optional: you can go forward too
    } else {
      Serial.println("Middle sensor off the line - stopping");
      stopMoving();
    }

  // Optional: small delay
  delay(50);
}




// === Motor Control Functions ===
void moveForward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);  // Left forward
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);   // Right forward
  analogWrite(ENA, baseSpeed);
  analogWrite(ENB, baseSpeed);
}

void moveForward2() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);  // Left forward
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);   // Right forward
  analogWrite(ENA, baseSpeed);
  analogWrite(ENB, baseSpeed);
}

void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);   // Left forward
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);   // Right backward
  analogWrite(ENA, baseSpeed);
  analogWrite(ENB, turnSpeed);
}

void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);  // Left forward
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);  // Right backward
  analogWrite(ENA, turnSpeed);
  analogWrite(ENB, baseSpeed);
}

void stopMoving() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}
