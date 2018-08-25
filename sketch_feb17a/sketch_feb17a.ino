// Program odczytuje temperaturę kilku czujników

#include <OneWire.h>
#include <DS18B20.h>

// Numer pinu do którego podłaczasz czujnik
#define ONEWIRE_PIN 2

// Ilość czujników
#define SENSORS_NUM 2

// Adresy czujników
//const byte address[SENSORS_NUM][8] PROGMEM = {
//  0x28, 0xB1, 0x6D, 0xA1, 0x3, 0x0, 0x0, 0x11,
//  0x28, 0x87, 0x6A, 0xA1, 0x3, 0x0, 0x0, 0x1F
//};

const byte address [SENSORS_NUM][8] PROGMEM = {
  0x28, 0xFF, 0x38, 0x8E, 0x70, 0x17, 0x3, 0xB1,
  0x28, 0xFF, 0xE2, 0x9, 0xA3, 0x16, 0x5, 0xE9};

OneWire onewire(ONEWIRE_PIN);
DS18B20 sensors(&onewire);

void setup() {
  while(!Serial);
  Serial.begin(9600);

  sensors.begin();
  sensors.request();
}

void loop() {
  if (sensors.available())
  {
    for (byte i=0; i<SENSORS_NUM; i++)
    {
      float temperature = sensors.readTemperature(FA(address[i]));

      Serial.print(F("#"));
      Serial.print(i);
      Serial.print(F(": "));
      Serial.print(temperature);
      Serial.println(F(" 'C"));
    }

    sensors.request();
  }

  // tu umieść resztę twojego programu
  // Będzie działał bez blokowania
}
