#include "GyverEncoder.h"

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <DS3231.h>
//#include <EEPROM.h>

LiquidCrystal_I2C lcd(0x27,16,2);

RTClib myRTC;
DS3231 rtc;
Encoder enc(2,3,4);

void initLed(){
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
}

int t1sleep = 1;
int t1active = 1;
int t1counter = 0;
bool t1flag = false;

int t2sleep = 1;
int t2active = 1;
int t2counter = 0;
bool t2flag = false;

// 0 - работает таймер
// 10 - изменить таймер 1
// 20 - изменить таймер 2
int status = 0; 

void printTimer(int t){
  lcd.setCursor(0,0);
  if(t == 1){
    lcd.print("T1 sleep = ");
    lcd.print(t1sleep);
  }
  if(t == 2){
    lcd.print("T1 active = ");
    lcd.print(t1active);
  }
  if(t == 3){
    lcd.print("T2 sleep = ");
    lcd.print(t2sleep);
  }
  if(t == 4){
    lcd.print("T2 active = ");
    lcd.print(t2active);
  }
}

void setup() {
  Serial.begin(9600);
  initLed();
  lcd.init();
  lcd.backlight();
}

void t1tick(){
  t1counter +=1;

  if(t1flag & t1counter >= t1sleep){
    t1counter = 0;
    t1flag = !t1flag;
    digitalWrite(5, 1);
    return;
  }
  if(!t1flag & t1counter >= t1active){
    t1counter = 0;
    t1flag = !t1flag;
    digitalWrite(5, 0);
    return;
  }
  
}

void t2tick(){
    t2counter +=1;

  if(t2flag & t2counter >= t2sleep){
    t2counter = 0;
    t2flag = !t2flag;
    digitalWrite(6, 1);
    return;
  }
  if(!t2flag & t2counter >= t2active){
    t2counter = 0;
    t2flag = !t2flag;
    digitalWrite(6, 0);
    return;
  }
}

uint32_t old_time = 0;
bool loader = true;
void loop() {
  DateTime now = myRTC.now();
  uint32_t current_time = now.unixtime();
  
  enc.tick();
  
  if(status == 0){
    if(old_time < current_time){
      old_time = current_time;
      
      t1tick();
      t2tick();
        
      lcd.setCursor(0,0);
      loader = !loader;
      lcd.print(loader?"Work |":"Work - ");
    }
    
  }
  if(status == 0 && enc.isPress()){
    lcd.clear();
    status = 10;
  }
  if(status == 10){
    printTimer(1);
    if(enc.isRight()){
      t1sleep += 1;
      return;
    }
    if(enc.isLeft()){
      t1sleep -= 1;
      return;
    }
    if(enc.isPress()){
      lcd.clear();
      status = 11;
      delay(300);
      return;
    }
  }
  
  if(status == 11){
    printTimer(2);
    if(enc.isRight()){
      t1active += 1;
      return;
    }
    if(enc.isLeft()){
      t1active-= 1;
      return;
    }
    if(enc.isPress()){
      lcd.clear();
      status = 20;
      delay(300);
      return;
    }
  }

  if(status == 20){
    printTimer(3);
    if(enc.isRight()){
      t2sleep += 1;
      return;
    }

    if(enc.isLeft()){
      t2sleep -= 1;
      return;
    }
    if(enc.isPress()){
      lcd.clear();
      status = 21;
      delay(300);
      return;
    }
  }
  if(status == 21){
    printTimer(4);
    if(enc.isRight()){
      t2active += 1;
      return;
    }

    if(enc.isLeft()){
      t2active-= 1;
      return;
    }
    if(enc.isPress()){
      lcd.clear();
      status = 0;
      delay(300);
      return;
    }
  }
}
