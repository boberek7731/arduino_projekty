#include "Countimer.h"
#include <OneWire.h>
#include <DS18B20.h>
#include <LiquidCrystal.h>

//Liczba sensorow temperatury
#define TEMPERATURE_SENSORS_NUM 3
//Pin odczytujacy temperature z czujnikow
#define ONEWIRE_PIN 2

//Sterowanie przekaznikiem nr 1
#define IN_ONE_PIN 4

//Przycisk 1
#define BUTTON_NEXT 13
#define BUTTON_PLUS 12
#define BUTTON_MINUS 11
#define BUTTON_PREVIOUS 10

//Piny LCD
#define RS_PIN A0
#define E_PIN A1
#define D4_PIN A2
#define D5_PIN A3
#define D6_PIN A4
#define D7_PIN A5

//#define RS_PIN 7
//#define E_PIN 8
//#define D4_PIN 9
//#define D5_PIN 10
//#define D6_PIN 11
//#define D7_PIN 12

//Lista adresow czujnikow
const byte address [TEMPERATURE_SENSORS_NUM][8] PROGMEM = {
  0x28, 0xFF, 0x38, 0x8E, 0x70, 0x17, 0x3, 0xB1,
  0x28, 0xFF, 0x5C, 0xE5, 0x70, 0x17, 0x4, 0x38,
  0x28, 0xFF, 0xE2, 0x9,  0xA3, 0x16, 0x5, 0xE9
  };

OneWire onewire(ONEWIRE_PIN);
DS18B20 sensors(&onewire);
LiquidCrystal lcd(RS_PIN, E_PIN, D4_PIN, D5_PIN, D6_PIN, D7_PIN);
Countimer timer;

int buttonNextState = 0;
int buttonNextPreviousState = 0;
int buttonPlusState = 0;
int buttonPlusPreviousState = 0;
int buttonMinusState = 0;
int buttonMinusPreviousState = 0;
int buttonPreviousState = 0;
int buttonPreviousPreviousState = 0;

int isTimerSet = 0;

//Zmienna sterujaca - czy odczytywac juz temperature
int startReadingTemperature = 0;

int numberOfSteps = 1;
int meshingTime = 60;
int meshingTemp = 65;
int meshingTimeStepOne = 1;
int meshingTempStepOne = 50;
int meshingTimeStepTwo = 2;
int meshingTempStepTwo = 50;
int meshingTimeStepThree = 3;
int meshingTempStepThree = 50;
int meshingTimeStepFour = 4;
int meshingTempStepFour = 50;
int meshingTimeStepFive = 5;
int meshingTempStepFive = 50;

int phase = 1;
byte meshingSteps[5][2] = {
    {1,1},
    {2,2},
    {3,3},
    {4,4},
    {5,5}
    };
int nrOfMeshingStep = 1;
int nrOfMeshingStepTime = 1;
int min_meshing_temp = 40;
int max_meshing_temp = 110;
int min_meshing_time = 20;
int max_meshing_time = 90;
int temp_or_time = 1;
void setup() {
  while(!Serial);
  
  //Inicjalizacja wyswietlacza
  lcd.begin(16,2);
  sensors.begin();
  sensors.request();
 
//  pinMode(IN_ONE_PIN,OUTPUT);
  pinMode(BUTTON_NEXT, INPUT_PULLUP);
  pinMode(BUTTON_PLUS, INPUT_PULLUP);
  pinMode(BUTTON_MINUS, INPUT_PULLUP);
  pinMode(BUTTON_PREVIOUS, INPUT_PULLUP);

  Serial.begin(9600);
}


void loop() {

  //===============ODCZYT PRZYCISKOW====================
  handleButtons();

  //======================MENU===========================
  if(phase == 1) {
    printMessageString("Ilosc krokow    ", 0);
    printMessageNumber(numberOfSteps, 1);  
  } else if(phase == 2) {
    printMessageString("Czas zacierania ", 0);
    printMessageNumber(meshingTime, 1);  
  } else if(phase == 3 && nrOfMeshingStep<=numberOfSteps) {
    if(temp_or_time > 0){
      printMessageString("Temp. kroku nr.", 0);  
    } else {
      printMessageString("Czas. kroku nr.", 0);
    }
    
    printMessageOnSelectedPlace(nrOfMeshingStep, 0, 15);
    startReadingTemperature = 0;
    switch (nrOfMeshingStep) {
      case 1:
        if(temp_or_time > 0){
          printMessageNumber(meshingTempStepOne, 1);  
        } else {
          printMessageNumber(meshingTimeStepOne, 1);  
        }
        break;
      case 2:
        if(temp_or_time > 0){
          printMessageNumber(meshingTempStepTwo, 1);
        } else {
          printMessageNumber(meshingTimeStepTwo, 1);  
        }
        break;
      case 3:
        if(temp_or_time > 0){
          printMessageNumber(meshingTempStepThree, 1);
        } else {
          printMessageNumber(meshingTimeStepThree, 1);  
        }
        break;
      case 4:
        if(temp_or_time > 0){
            printMessageNumber(meshingTempStepFour, 1);
          } else {
            printMessageNumber(meshingTimeStepFour, 1);  
          }
        break;
      case 5:
        if(temp_or_time > 0){
            printMessageNumber(meshingTempStepFive, 1);
          } else {
            printMessageNumber(meshingTimeStepFive, 1);  
          }
        break;
    }
  } else if(phase == 5) {
      startReadingTemperature = 1;
      switch (nrOfMeshingStep - 1){
        case 1:
          setTimer(meshingTimeStepOne);
          break;
        case 2:
          setTimer(meshingTimeStepTwo);
          break;
        case 3:
          setTimer(meshingTimeStepThree);
          break;
        case 4:
          setTimer(meshingTimeStepFour);
          break;
        case 5:
          setTimer(meshingTimeStepFive);
          break;
      }
      
      timer.run();
      if(!timer.isCounterCompleted()){
        timer.start();
      }
  }
  
  //===============ODCZYT TEMPERATURY====================
  if(startReadingTemperature == 1) {
    float temperature = 0.0;
    if (sensors.available())
    {
      for(byte i=0; i<TEMPERATURE_SENSORS_NUM; i++){
        temperature += sensors.readTemperature(FA(address[i]));
      }
  
      sensors.request();
  
      float avgTemp = temperature/TEMPERATURE_SENSORS_NUM;
    
      printTemperatureOnSerialMonitor(avgTemp);
  
//      String message = "Temp: ";
//      printMessageString(message + avgTemp + " 'C  ", 0);
//      printMessageString("Zacieranie",1);
    }  
  }
  
  //===============ODCZYT TEMPERATURY====================

//    int pinState = digitalRead(IN_ONE_PIN);

//    if(temperature < 25.0) {
//    Serial.println(F("Temperature lower than 25 - Heating ON 'C"));
//    Serial.println(digitalRead(IN_ONE_PIN));
//    digitalWrite(IN_ONE_PIN, LOW);
//  } else {
//    Serial.println(F("Temperature lower than 25 - Heating OFF 'C"));
//    digitalWrite(IN_ONE_PIN, HIGH);
//  }
}
void onComplete(){
  Serial.println("Complete !!!!");
  nrOfMeshingStep++;
}
void setTimer(int numberOfMinutes) {
  if(isTimerSet == 0) {
    Serial.println("START TIMER");  
    timer.setCounter(0,numberOfMinutes,0, timer.COUNT_DOWN, onComplete);
    timer.setInterval(refreshClock, 1000);
    isTimerSet = 1;
    timer.run();
  }
}

void refreshClock() {
    Serial.print("Current count time is: ");
    Serial.println(timer.getCurrentTime());
}


void setStepTemp(int stepNr, byte value) {
  meshingSteps[stepNr-1][0] = value;
}

void setStepTime(int stepNr, byte value) {
  meshingSteps[stepNr-1][1] = value;
}

void moveToTheNextPhase() {
    buttonNextState = digitalRead(BUTTON_NEXT);
    buttonPreviousState = digitalRead(BUTTON_PREVIOUS);
    
    if((buttonNextState != buttonNextPreviousState) && buttonNextState == HIGH) {
      Serial.println(F("BUTTON NEXT"));
      if(phase < 3 && nrOfMeshingStep == 1)
        phase++;
      else if(phase == 3 && nrOfMeshingStep <= numberOfSteps) {
        if(temp_or_time < 0)
          nrOfMeshingStep++;
        
        temp_or_time = temp_or_time * (-1);
      } else /*if(phase == 3 && nrOfMeshingStep == numberOfSteps && temp_or_time < 0)*/ {
        phase++;
      }
      delay(50);
    }
  
    if((buttonPreviousState != buttonPreviousPreviousState) && buttonPreviousState == HIGH) {
      Serial.println(F("BUTTON PREVIOUS"));
      if(phase > 1 && nrOfMeshingStep == 1)
        phase--;
      else if(phase == 3 && nrOfMeshingStep > 1) {
        if(temp_or_time < 0) {
          nrOfMeshingStep--;
        }
        temp_or_time = temp_or_time * (-1);
      } else /*if(phase > 3 && nrOfMeshingStep == numberOfSteps && temp_or_time < 0)*/ {
        phase--;
      }
      delay(50);
    }
  
    buttonNextPreviousState = buttonNextState;
    buttonPreviousPreviousState = buttonPreviousState;  
}

void modifyValue(int value, int minValue, int maxValue) {
  buttonPlusState = digitalRead(BUTTON_PLUS);
  buttonMinusState = digitalRead(BUTTON_MINUS);
  
  if((buttonPlusState != buttonPlusPreviousState) && buttonPlusState == HIGH) {
    Serial.println(F("BUTTON PLUS"));
    if(value < maxValue)
      value++;

    delay(50);
  }

  if((buttonMinusState != buttonMinusPreviousState) && buttonMinusState == HIGH) {
    Serial.println(F("BUTTON MINUS"));
    if(value > minValue)
      value--;
    delay(50);
  }

  buttonPlusPreviousState = buttonPlusState;
  buttonMinusPreviousState = buttonMinusState;

  if(phase == 1) {
    numberOfSteps = value;  
  } else if(phase == 2) {
    meshingTime = value;
  } else if(phase == 3 && nrOfMeshingStep == 1) {
    if(temp_or_time > 0) {
      meshingTempStepOne = value;  
    } else {
      meshingTimeStepOne = value;  
    }
  } else if(phase == 3 && nrOfMeshingStep == 2) {
    if(temp_or_time > 0) {
      meshingTempStepTwo = value;
    } else {
      meshingTimeStepTwo = value;  
    }
  } else if(phase == 3 && nrOfMeshingStep == 3) {
    if(temp_or_time > 0) {
      meshingTempStepThree = value;
    } else {
      meshingTimeStepThree = value;  
    }
  } else if(phase == 3 && nrOfMeshingStep == 4) {
    if(temp_or_time > 0) {
      meshingTempStepFour = value;
    } else {
      meshingTimeStepFour = value;  
    }
  } else if(phase == 3 && nrOfMeshingStep == 5) {
    if(temp_or_time > 0) {
      meshingTempStepFive = value;
    } else {
      meshingTimeStepFive = value;  
    }
  }
}

void handleButtons() {

  if(phase == 1) {
    modifyValue(numberOfSteps, 1, 5);
  } else if(phase == 2) {
    modifyValue(meshingTime, 45, 90);
  } else if(phase == 3 && nrOfMeshingStep == 1) {
    if(temp_or_time > 0)
      modifyValue(meshingTempStepOne, min_meshing_temp, max_meshing_temp);
    else
      modifyValue(meshingTimeStepOne, min_meshing_time, max_meshing_time);
  } else if(phase == 3 && nrOfMeshingStep == 2) {
    if(temp_or_time > 0)
      modifyValue(meshingTempStepTwo, min_meshing_temp, max_meshing_temp);
    else
      modifyValue(meshingTimeStepTwo, min_meshing_time, max_meshing_time);
  } else if(phase == 3 && nrOfMeshingStep == 3) {
    if(temp_or_time > 0)
      modifyValue(meshingTempStepThree, min_meshing_temp, max_meshing_temp);
    else
      modifyValue(meshingTimeStepThree, min_meshing_time, max_meshing_time);
  } else if(phase == 3 && nrOfMeshingStep == 4) {
    if(temp_or_time > 0)
      modifyValue(meshingTempStepFour, min_meshing_temp, max_meshing_temp);
    else
      modifyValue(meshingTimeStepFour, min_meshing_time, max_meshing_time);
  } else if(phase == 3 && nrOfMeshingStep == 5) {
    if(temp_or_time > 0)
      modifyValue(meshingTempStepFive, min_meshing_temp, max_meshing_temp);
    else
      modifyValue(meshingTimeStepFive, min_meshing_time, max_meshing_time);
  } 
  moveToTheNextPhase();
}

void printMessageString(String message, int lineNumber) {
  Serial.print("====");
  Serial.print(message);
  Serial.println(F("===="));

  lcd.setCursor(0,lineNumber);
  lcd.print(message);
}

void printMessageOnSelectedPlace(int message, int lineNumber, int cursorNumber) {
  lcd.setCursor(cursorNumber,lineNumber);
  lcd.print(message);
  lcd.print("  ");
}

void printMessageNumber(int value, int lineNumber) {
  Serial.print("====");
  Serial.print(value);
  Serial.println(F("===="));

  if(value < 10) {
    lcd.setCursor(0,lineNumber);
    lcd.print(value, DEC);
    lcd.setCursor(1,lineNumber);
    lcd.print("                ");
  } else { 
    lcd.setCursor(0,lineNumber);
    lcd.print(value, DEC);
    lcd.setCursor(0,lineNumber);
  }
}

void printTemperatureOnSerialMonitor(float temp){
    Serial.print(F("Average Temperature: "));
    Serial.print(temp);
    Serial.println(F(" 'C "));
}
