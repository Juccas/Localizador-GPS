#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
static const int RXPin = 4, TXPin = 3;
TinyGPSPlus gps;// The TinyGPSPlus object
SoftwareSerial ss(27, 14);// The serial connection to the GPS device

void setup()
{
  Serial.begin(9600);
  ss.begin(9600);
  Serial.println("VAMOS LA");
  Serial.println();
  while(true){
    if (millis() > 5000 && gps.charsProcessed() < 10){
      Serial.println("GPS N conectado");
    }
    else{
      break;
    }
  }
}

void loop()
{
  if (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();
}

void displayInfo()
{
  Serial.println("Location: "); 
  if (gps.location.isValid())
  { 
    Serial.print("Lat:");
    Serial.print(gps.location.lat(), 6);
    Serial.print("/ Long: ");
    Serial.println(gps.location.lng(), 6);
  }
  else
  {
    Serial.print("Lat e long invalido");
  }

  Serial.println("Hora:");
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) {
      Serial.print(F("0"));
    }
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10){ 
      Serial.print(F("0"));
    }
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10){ 
      Serial.print(F("0"));
      }
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10){ 
      Serial.print(F("0"));
    }
    Serial.print(gps.time.centisecond());
  }
  else{
    Serial.print("Hora invalida");
  }
  Serial.println();
}