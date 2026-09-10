
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;//LCD ports
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);//setting up LCD


SoftwareSerial mySerial(8, 7);
Adafruit_GPS GPS(&mySerial);


#define GPSECHO  false

bool firstRun = true;

void setup()
{
  Serial.begin(115200);
  mySerial.begin(9600);
  delay(5000);
  Serial.println("Adafruit GPS library basic parsing test!");

  GPS.begin(9600);

  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);

  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);

  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);  

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  delay(5000);
  // Ask for firmware version
  mySerial.println(PMTK_Q_RELEASE);

  lcd.begin(16, 2);
  
}

double latitude = (GPS.latitudeDegrees, 8);
double longitude = (GPS.longitudeDegrees, 8);
double newLat = (GPS.latitudeDegrees, 8);
double newLong = (GPS.longitudeDegrees, 8);
double distance = 0;

uint32_t timer = millis();

void loop()
{
  char c = GPS.read();
  if ((c) && (GPSECHO))
    Serial.write(c);

  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA()))   // sets the newNMEAreceived() flag to false
      return;  
  }

  // every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) {
    timer = millis(); // reset the timer

    Serial.print("\nTime: ");
    if (GPS.hour < 10) { Serial.print('0'); }
    Serial.print(GPS.hour, DEC); Serial.print(':');
    if (GPS.minute < 10) { Serial.print('0'); }
    Serial.print(GPS.minute, DEC); Serial.print(':');
    if (GPS.seconds < 10) { Serial.print('0'); }
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    if (GPS.milliseconds < 10) {
      Serial.print("00");
    } else if (GPS.milliseconds > 9 && GPS.milliseconds < 100) {
      Serial.print("0");
    }
    Serial.println(GPS.milliseconds);
    Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print("/20");
    Serial.println(GPS.year, DEC);
    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
    if (GPS.fix) {
      Serial.print("Location: ");
      Serial.print(GPS.latitudeDegrees, 8); Serial.print(GPS.lat);
      Serial.print(", ");
      Serial.print(GPS.longitudeDegrees, 8); Serial.println(GPS.lon);

      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      Serial.print("Angle: "); Serial.println(GPS.angle);
      Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
      Serial.print("Antenna status: "); Serial.println((int)GPS.antenna);
    }
    if (GPS.fix) {
        //ETCG Notes -- Print to LCD Screen
        lcd.clear();
        lcd.setCursor(7,0);
        lcd.print("Spd:");
        lcd.setCursor(11,0);
        float mph;
        mph = (GPS.speed) * 1.15;
        lcd.print(mph);
        lcd.setCursor(15, 0);
        lcd.print("mph");


        if(firstRun == true){
          firstRun = false;
          latitude = GPS.latitudeDegrees, 8;
          longitude = GPS.longitudeDegrees, 8;
        }
        else{
          newLat = GPS.latitudeDegrees, 8;
          newLong = GPS.longitudeDegrees, 8;
          distance += sqrt(pow(((newLat - latitude) * 69), 2) + pow(((newLong - longitude) * 50), 2));

          latitude = GPS.latitudeDegrees, 8;
          longitude = GPS.longitudeDegrees, 8;

          lcd.setCursor(0,1);
          lcd.print("Dis:");
          lcd.setCursor(4,1);
          lcd.print(distance);
        }

      }  else {
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("No Signal");
      }
      lcd.setCursor(0,0);
      lcd.print("Sats:");
      lcd.setCursor(5,0);
      lcd.print((int)GPS.satellites);
  }
}
