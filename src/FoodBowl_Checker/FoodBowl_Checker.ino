// Cat Food Bowl Checker
// Introduction to IoT - PBL
// Author: C. Mendes
// Date: April 24th, 2025

/* ───── Library ───── */
#include <NewPing.h> // Handles HC-SR04 timing and distance math

/* ───── PIN DEFINITIONS ───── */
const int triggerPin = 11;
const int echoPin    = 10;
const int ledPin     = 6;
const int buzzerPin  = 5;
const int buttonPin  = 4;    // push-button override (active-LOW)

/* ───── CONSTANTS ───── */
const int maxDistance = 30;   // maximum distance to measure (cm)
const int threshold   = 10;   // bowl considered empty if distance > threshold (cm)

/* ───── GLOBAL STATE ───── */
NewPing sonar(triggerPin, echoPin, maxDistance);

bool alertSilenced   = false;   // true → alert muted until bowl is refilled
bool prevButtonState = HIGH;    // for edge detection (HIGH = not pressed)

/* ───── FUNCTION DECLARATIONS ───── */
long checkDistance();
void triggerAlert(bool alert);
void handleButton(bool bowlEmpty);

/* ───── SETUP ───── */
void setup() {
  pinMode(ledPin,    OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);   // internal pull-up
  Serial.begin(9600);
}

/* ───── MAIN LOOP ───── */
void loop() {
  /* 1. Measure distance */
  long distance = checkDistance();     // in cm

  /* 2. Determine if the bowl is empty */
  bool bowlEmpty;
  if (distance > threshold && distance != 0) {
    bowlEmpty = true;                  // bowl is empty / low
  } else {
    bowlEmpty = false;                 // bowl has food
  }

  /* 3. Handle push-button toggle (mutes alert while bowl is empty) */
  handleButton(bowlEmpty);

  /* 4. Alert logic */
  if (bowlEmpty && !alertSilenced) {
    triggerAlert(true);                // buzzer + LED ON
  } else {
    triggerAlert(false);               // buzzer + LED OFF
  }

  /* 5. Auto-re-arm alert once bowl is refilled */
  if (!bowlEmpty) {
    alertSilenced = false;             // alerts enabled for next empty event
  }

  delay(200);  // loop delay
}

/* ─────────────────────────────────── */
/* Measure distance (cm) with NewPing  */
long checkDistance() {
  delay(50);                             // sensor settling time
  long dist = sonar.ping_cm();           // 0 if out of range
  Serial.print("Distance: ");
  Serial.print(dist);
  Serial.println(" cm");
  return dist;
}

/* Turn LED & buzzer ON or OFF */
void triggerAlert(bool alert) {
  if (alert) {
    digitalWrite(ledPin, HIGH);
    digitalWrite(buzzerPin, HIGH);  // active buzzer sounds
  } else {
    digitalWrite(ledPin, LOW);
    digitalWrite(buzzerPin, LOW);   // silence buzzer
  }
}

/* Edge-detect push-button and set alertSilenced */
void handleButton(bool bowlEmpty) {
  bool buttonPressed;

  /*Read buttonPin (LOW when pressed) */
  int rawState = digitalRead(buttonPin);
  if (rawState == LOW) {
    buttonPressed = true;
  } else {
    buttonPressed = false;
  }

  /* rising-edge detection: pressed now, was not pressed last loop */
  if (buttonPressed && prevButtonState == HIGH && bowlEmpty) {
    alertSilenced = true;               // mute alert until bowl refilled
    Serial.println("Alert silenced by button.");
  }

  prevButtonState = buttonPressed;      // update state for next loop
}
