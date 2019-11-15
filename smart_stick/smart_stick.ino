#include <DFRobot_sim808.h>

#define MESSAGE_LENGTH 160
#define PHONE_NUMBER "5197221095"
#define ECHOPIN_1 4// Pin to receive echo pulse
#define TRIGPIN_1 5// Pin to send trigger pulse

#define button 6
#define buzzer 7
#define THRESHOLD 10

char message[MESSAGE_LENGTH];
int messageIndex = 0;
char MESSAGE[300];
char lat[12];
char lon[12];
char wspeed[12];

int distance = 0;
int buttonval = 0;
int returnval = 0;
DFRobot_SIM808 sim808(&Serial);

void setup() {
  //mySerial.begin(9600);
  Serial.begin(9600);

  pinMode(button, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(ECHOPIN_1, INPUT);
  pinMode(TRIGPIN_1, OUTPUT);
  digitalWrite(ECHOPIN_1, HIGH);

  //******** Initialize sim808 module *************
  while (!sim808.init()) {
    delay(1000);
    Serial.print("Sim808 init error\r\n");
  }

  //************* Turn on the GPS power************
  if ( sim808.attachGPS())
    Serial.println("Open the GPS power success");
  else
    Serial.println("Open the GPS power failure");

}

void loop() {

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
    for (int i = 0; i < 5; i++) {
      digitalWrite(buzzer, HIGH);
      delay(200);
      digitalWrite(buzzer, LOW);
      delay(200);
    }
  }
  delay(500);

  if (!digitalRead(button))
  {
    //    buttonval = 1;
    Serial.print("Button Pressed.");
    while (!GetLocation()) {

    }
  }
}

bool GetLocation(void)
{
  //************** Get GPS data *******************
  if (sim808.getGPS()) {
    Serial.print(sim808.GPSdata.year);
    Serial.print("/");
    Serial.print(sim808.GPSdata.month);
    Serial.print("/");
    Serial.print(sim808.GPSdata.day);
    Serial.print(" ");
    Serial.print(sim808.GPSdata.hour);
    Serial.print(":");
    Serial.print(sim808.GPSdata.minute);
    Serial.print(":");
    Serial.print(sim808.GPSdata.second);
    Serial.print(":");
    Serial.println(sim808.GPSdata.centisecond);

    Serial.print("latitude :");
    Serial.println(sim808.GPSdata.lat, 6);

    sim808.latitudeConverToDMS();
    Serial.print("latitude :");
    Serial.print(sim808.latDMS.degrees);
    Serial.print("\^");
    Serial.print(sim808.latDMS.minutes);
    Serial.print("\'");
    Serial.print(sim808.latDMS.seconeds, 6);
    Serial.println("\"");
    Serial.print("longitude :");
    Serial.println(sim808.GPSdata.lon, 6);
    sim808.LongitudeConverToDMS();
    Serial.print("longitude :");
    Serial.print(sim808.longDMS.degrees);
    Serial.print("\^");
    Serial.print(sim808.longDMS.minutes);
    Serial.print("\'");
    Serial.print(sim808.longDMS.seconeds, 6);
    Serial.println("\"");

    Serial.print("speed_kph :");
    Serial.println(sim808.GPSdata.speed_kph);
    Serial.print("heading :");
    Serial.println(sim808.GPSdata.heading);

    float la = sim808.GPSdata.lat;
    float lo = sim808.GPSdata.lon;
    float ws = sim808.GPSdata.speed_kph;

    dtostrf(la, 6, 2, lat); //put float value of la into char array of lat. 6 = number of digits before decimal sign. 2 = number of digits after the decimal sign.
    dtostrf(lo, 6, 2, lon); //put float value of lo into char array of lon
    dtostrf(ws, 6, 2, wspeed);  //put float value of ws into char array of wspeed

    sprintf(MESSAGE, "Latitude : %s\nLongitude : %s\nWind Speed : %s kph\nNeed Help. Ritu Nesargi. Locate me.\nhttp://www.latlong.net/Show-Latitude-Longitude.html\nhttp://maps.google.com/maps?q=%s,%s\n", lat, lon, wspeed, lat, lon);

    Serial.println("Sim808 init success");
    Serial.println("Start to send message ...");
    Serial.print("\nMessaege: ");
    Serial.println(MESSAGE);

    while (!sim808.sendSMS(PHONE_NUMBER, MESSAGE))
    {
    }
    //    int returnval = 2;
    Serial.println("Message Sent.");
    //*************  ************
    sim808.detachGPS();
    return false;
  }
}
