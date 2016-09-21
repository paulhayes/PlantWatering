
#include "LowPower.h"
#include <EEPROM.h>

#define RXLED 17
#define PUMP_DURATION 5000
bool serialMode = false;
bool printSensor = false;
int threshold;
int sensorWarmup;
int sensor = 0;
long sampleInterval;

void setup() {
  // put your setup code here, to run once:
  pinMode(9,OUTPUT);
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
  pinMode(RXLED, OUTPUT); 
  pinMode(10,OUTPUT);
  digitalWrite(9,LOW);
  digitalWrite(10,HIGH);
  delay(1000);
  GreenLED(false);
  OrangeLED(false);
  Serial.begin(9600);
  
  delay(3000);
  EEPROM.get(0,threshold);
  if( threshold <= 0 ){
    threshold = 200;
  }
  EEPROM.get(2,sensorWarmup);
  if( sensorWarmup <= 0 ){
    sensorWarmup = 800;
  }
  EEPROM.get(4,sampleInterval);
  if( sampleInterval <= 0 ){
    sampleInterval = 8000;
  }

  if(Serial){
      serialMode = true;
      
      Serial.println("PLANT WATERING SYSTEM");
      Serial.println("commands:");
      Serial.print("pump - Trigger the pump for ");
      Serial.print(PUMP_DURATION);
      Serial.println("ms ");
      Serial.println("sensor - Print sensor value once");
      Serial.println("print sensor - Toggle continuous sensor print");
      Serial.println("threshold [int]- gets or sets the threshold for watering");
      Serial.println("sensor warmup [int] - warmup time in ms");
      Serial.println("sample interval [int] - sample interval in ms");
      Serial.println("======================================================");
      Serial.print("threshold=");
      Serial.print(threshold);
      Serial.print(", sensor warmup=");
      Serial.print(sensorWarmup);
      Serial.print(", sample interval=");
      Serial.println(sampleInterval);
      GreenLED(true);  

  }
  
}

void loop() {
  
  
  if( serialMode ){
    if( Serial.available() ){
      String a = Serial.readString();
      a.trim();
      //Serial.println(a);
      if( a == "pump" ){
        Pump();
      }else if(a == "sensor" ){
        Serial.print("sensor: ");
        Serial.println(sensor);
             
      }
      else if(a == "print sensor" ){
        printSensor = !printSensor;
      }
      else if(a.substring(0,9)=="threshold"){
        if(a.length() > 9 ){
          threshold = a.substring(10).toInt();
          EEPROM.put(0,threshold);
          Serial.print("Setting threshold to ");
          Serial.println(threshold);
        } else {
          Serial.print( "Threshold set to " );
          Serial.println( threshold );
        }
       
      } else if(a.substring(0,13)=="sensor warmup"){
        if(a.length() == 13 ){
          Serial.print("Sensor Warmup time ");
          Serial.print( sensorWarmup );
          Serial.println("ms ");
        }
        else {
          sensorWarmup = a.substring(13).toInt();
          EEPROM.put(2,sensorWarmup);

          Serial.print("Sensor warmup set to ");
          Serial.print(sensorWarmup);
          Serial.println("ms");
        }
      } else if(a.substring(0,15)=="sample interval"){
        if( eepromInt(sampleInterval, 4, "sample interval", a) ){
          Serial.print("Sample interval ");
          Serial.print( sampleInterval );
          Serial.println("ms");
        }
        else {
          Serial.print("Sample interval set to ");
          Serial.print(sampleInterval);
          Serial.println("ms");
        }
      
      }
      else {
        Serial.print("Command \"");  
        Serial.print(a);
        Serial.println("\" not found");
      }
      
    }
    if( printSensor ){
        Serial.print("sensor: ");
        Serial.println(sensor);
        delay(500);        
    }
  }

    digitalWrite(10,LOW);
    if( serialMode ){
      delay(500);
    }
    else {
      int iter = sampleInterval / 8000;
      for(int i=0;i<iter;i++){
        LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);  
      }
    }
    digitalWrite(10,HIGH);
    delay(sensorWarmup);
    sensor = analogRead(A0);
    if( sensor > threshold ){
      OrangeLED(true);
      Pump();
      OrangeLED(false);
    }
  
  
}

bool eepromInt(long &value, int offset, String commandName, String command){
  int len = commandName.length();
  if( len == command.length() ){
    return true;
  }
  else {
    value = command.substring(len).toInt();
    EEPROM.put(offset,value);
    return false;
  }
}

void Pump(){
  PumpOn(true);
  delay(5000);
  PumpOn(false);
}

void OrangeLED(bool on){
  digitalWrite(RXLED,on ? LOW : HIGH);
}

void GreenLED(bool on){
  if( on ) TXLED0;
  else TXLED1;
}

void PumpOn(bool on){
  digitalWrite(9,on ? HIGH : LOW);  
}


