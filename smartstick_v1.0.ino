#include "Adafruit_FONA.h"
#include <SoftwareSerial.h>

#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4

#define ECHOPIN_1 8// Pin to receive echo pulse
#define TRIGPIN_1 9// Pin to send trigger pulse

#define buzzer 7
#define vibrator 6
#define button 5
#define THRESHOLD 10

SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);
uint8_t type;
int distance = 0;
int buttonval = 0;
int returnval = 0;

int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;

void setup() {
  Serial.begin(115200);

  fonaSerial->begin(4800);
  if (! fona.begin(*fonaSerial)) {
    Serial.println(F("Couldn't find FONA"));
    while (1);
  }
  Serial.println(F("FONA is OK"));
  pinMode(buzzer, OUTPUT);
  pinMode(vibrator, OUTPUT);
  pinMode(ECHOPIN_1, INPUT);
  pinMode(button, INPUT);
  pinMode(TRIGPIN_1, OUTPUT);
  digitalWrite(ECHOPIN_1, HIGH);
  analogWrite(vibrator, 0);
}

void sendSMS() {
  char sendto[21] = "4379831994", message[141] = "Help Me Bhai";
  // flushSerial();
  Serial.print(F("Sending to "));
  Serial.println(sendto);
  Serial.print(F("With Message: "));
  Serial.println(message);
  if (!fona.sendSMS(sendto, message)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("Sent!"));
  }
}

void loop() {
  analogWrite(vibrator, 0);
  digitalWrite(TRIGPIN_1, LOW); // Set the trigger pin to low for 2uS
  delayMicroseconds(2);
  digitalWrite(TRIGPIN_1, HIGH); // Send a 10uS high to trigger ranging
  delayMicroseconds(10);
  digitalWrite(TRIGPIN_1, LOW); // Send pin low again
  distance = pulseIn(ECHOPIN_1, HIGH, 26000); // Read in times pulse
  distance = distance / 58;

  Serial.print(distance);
  Serial.println("   cm");

  if (distance <= THRESHOLD)
  {
    analogWrite(vibrator, 143);
    tone(buzzer, 494, 500);
    delay(300);
    noTone(6);
  }

//Read Emergency Button
  int reading = digitalRead(button);
  
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == HIGH) {
        sendSMS();
        Serial.println("Button Pressed.");
      }
    }
  }
  lastButtonState = reading;
}
