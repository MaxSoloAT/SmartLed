#define I2CADDRESS 0x15
#define PWMPIN 9

#include <Wire.h>
#include <GyverPWM.h>

bool new_command = false;
String rawData = ""; //raw data from i2c
String Data = "", PreData;
int tPWM = 0 , lPWM = 0;
int del = 5;

void setup() {
  Serial.begin(9600);
  Wire.begin(I2CADDRESS);
  Wire.onReceive(receiveEvent);
  pinMode(PWMPIN,OUTPUT);
  PWM_prescaler(PWMPIN, 3);
  PWM_mode(PWMPIN, 3);
  digitalWrite(PWMPIN,LOW);
}

void loop() {
  if(new_command){ 
    ParseRawData(rawData);
    new_command = false;
  }
  
  if(Data =="PWM"){
    for(;;){
      if(new_command){ break;}
     // if(lPWM != tPWM){
      for(int i = 0; i<= abs(tPWM-lPWM) ;++i){
        if(new_command){ 
          if(tPWM > lPWM){
            tPWM = lPWM+i;
            lPWM = tPWM;
          }
          else{
            tPWM = lPWM-i;
            lPWM = tPWM;
          }
          break;
        }
        
        if(tPWM > lPWM){
          Serial.println(lPWM+i);
          PWM_16KHZ_D9(lPWM+i);
        }
        else{
          Serial.println(lPWM-i);
          PWM_16KHZ_D9(lPWM-i);
        }
        delay(del); //add value for set time
      }
      lPWM = tPWM;
   // }
    }
  }
  
}


void receiveEvent(int bytes)
{
  rawData = "";
  while( Wire.available()){
    rawData += (char)Wire.read();
  }
  Serial.print("RAW: ");
  Serial.println(rawData);
  new_command = true;
}

void ParseRawData(String rawData){
  PreData = Data;
  int sep = rawData.indexOf(" ");
  Data = rawData.substring(0,sep);
  
  if(Data == "PWM"){
    String pwm = rawData.substring(sep+1,rawData[rawData.length()-1]);
    tPWM = pwm.toInt();
    Serial.println(tPWM);
  }
  if(Data == "SETT"){
    String dels = rawData.substring(sep+1,rawData[rawData.length()-1]);
    del = dels.toInt();
    Data = PreData;
  }
  Serial.println(Data);
}
