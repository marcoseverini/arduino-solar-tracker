#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

Servo myservo;
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int SERVO_PIN = 9;
const int PHOTO_SENSOR_1 = A0;
const int PHOTO_SENSOR_2 = A1;
const int CENTER = 90;
const float RC = 0.8;  // Damping factor
const int MAX_ANGLE = 30;  // Maximum tilt angle
const int MIN_ANGLE = -30;  // Minimum tilt angle
const int DEAD_ZONE = 2;  // Dead zone to avoid micro-movements

int currentAngle = CENTER;
float smoothedValue = 0;

void setup() {
  Serial.begin(9600);
  myservo.attach(SERVO_PIN);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(1, 0);
  lcd.print("Photovoltaic");
  delay(2000);
}

void loop() {
  int sensorValue1 = analogRead(PHOTO_SENSOR_1);
  int sensorValue2 = analogRead(PHOTO_SENSOR_2);
  
  float rawDifference = (sensorValue1 - sensorValue2) / 10.0;
  
  // Apply a low-pass filter to smooth the movement
  smoothedValue = (rawDifference * (1 - RC)) + (smoothedValue * RC);
  
  // Implement a dead zone to prevent minor adjustments
  if (abs(smoothedValue) < DEAD_ZONE) {
    smoothedValue = 0;
  }
  
  // Limit the angle value
  int targetAngle = constrain(CENTER - (int)smoothedValue, CENTER - MAX_ANGLE, CENTER + MAX_ANGLE);
  
  // Gradually move the servo towards the target angle
  if (targetAngle != currentAngle) {
    currentAngle += (targetAngle > currentAngle) ? 1 : -1;
    myservo.write(currentAngle);
  }

  // Update the LCD display
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print(currentAngle - CENTER);  // Display deviation from center
  
  delay(50);  // Short delay to control the update speed
}
