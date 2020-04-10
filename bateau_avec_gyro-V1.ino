#include <nRF24L01.h>
#include <RF24.h>
#include<SPI.h>
#include<Servo.h>
#include "Wire.h"
#include <math.h>

RF24 radio(9,10);
const byte address[6] = "00001";
Servo ServoBarre;
Servo ServoEcoute;

//Variable pour Gyro
const int MPU=0x68;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
double pitch,roll;
int AcXoff,AcYoff,AcZoff,GyXoff,GyYoff,GyZoff;
int temp,toff;
double t,tx,tf;
      
//Acceleration data correction
AcXoff = -950;
AcYoff = -300;
AcZoff = 0;

//Temperature correction
toff = -1600;

//Gyro correction
GyXoff = 480;
GyYoff = 170;
GyZoff = 210;

//Variable pour servo
int angle1 = 0;
int angle2 = 0;
int angle2Max = 180; //Ecoute Max
int angle2Min = 0; // Ecoute Min

int modeVent,RollMax,RollMin;
RollMax = 30;
RollMin = 10;

char input[32] = "";
const char var1[32] = "ServoBarre";
const char var2[32] = "ServoEcoute";
const char var3[32] = "ModeVent";

void setup()
{
Serial.begin(9600);
ServoBarre.attach(2);
ServoEcoute.attach(3);
radio.begin();
radio.openReadingPipe(0, address);
radio.setChannel(100);
radio.setDataRate(RF24_250KBPS);
radio.setPALevel(RF24_PA_MAX);
radio.startListening();
Wire.begin();
Wire.beginTransmission(MPU);
Wire.write(0x6B);
Wire.write(0);
Wire.endTransmission(true);
}

void loop()
{
  delay(5);
  while(!radio.available());
  Serial.println("Radio ON");
  radio.read(&input, sizeof(input));

  if((strcmp(input,var1) == 0))
  {
    while(!radio.available());
    radio.read(&angle1, sizeof(angle1));
    ServoBarre.write(angle1);
    Serial.println(input);
    Serial.print("Angle:");
    Serial.println(angle1);
    Serial.println("--------------------------------");
  }
  else if ((strcmp(input,var3) == 0)){
    while(!radio.available());
    radio.read(&modeVent, sizeof(modeVent));
  
    // Mode Manuel
    if((modeVent == 0)){
      Serial.println("MODE MANUEL")
      while(!radio.available());
      radio.read(&angle2, sizeof(angle2));
      ServoEcoute.write(angle2);
      Serial.println(input);
      Serial.print("Angle:");
      Serial.println(angle2);
      Serial.println("--------------------------------");
    }

    // Mode Auto
    if((modeVent == 1){
      Serial.println("MODE AUTO")
      
      //programme pour gyro
      Wire.beginTransmission(MPU);
      Wire.write(0x3B);
      Wire.endTransmission(false);
      Wire.requestFrom(MPU,14,true);
      
      //read accel data
      AcX=(Wire.read()<<8|Wire.read()) + AcXoff;
      AcY=(Wire.read()<<8|Wire.read()) + AcYoff;
      AcZ=(Wire.read()<<8|Wire.read()) + AcYoff;

      //read temperature data
      temp=(Wire.read()<<8|Wire.read()) + toff;
      tx=temp;
      t = tx/340 + 36.53;
      tf = (t * 9/5) + 32;

      //read gyro data
      GyX=(Wire.read()<<8|Wire.read()) + GyXoff;
      GyY=(Wire.read()<<8|Wire.read()) + GyYoff;
      GyZ=(Wire.read()<<8|Wire.read()) + GyZoff;

      //get pitch/roll
      getAngle(AcX,AcY,AcZ);

      //send the data out the serial port
      Serial.print("Angle: ");
      Serial.print("Pitch = "); Serial.print(pitch);
      Serial.print(" | Roll = "); Serial.println(roll);

      Serial.print("Temp: ");
      Serial.print("Temp(F) = "); Serial.print(tf);
      Serial.print(" | Temp(C) = "); Serial.println(t);

      Serial.print("Accelerometer: ");
      Serial.print("X = "); Serial.print(AcX);
      Serial.print(" | Y = "); Serial.print(AcY);
      Serial.print(" | Z = "); Serial.println(AcZ);

      Serial.print("Gyroscope: ");
      Serial.print("X = "); Serial.print(GyX);
      Serial.print(" | Y = "); Serial.print(GyY);
      Serial.print(" | Z = "); Serial.println(GyZ);
      Serial.println(" ");
      delay(333);

      //Ajustement des voiles
      if((roll > RollMax && roll < 360 - RollMax && angle2 >= angle2Min + 10)){ // Gite trop
        angle2 = angle2 -10;
        ServoEcoute.write(angle2);
        Serial.println(input);
        Serial.print("Angle:");
        Serial.println(angle2);
        Serial.println("--------------------------------");
      }
      else if ((roll < RollMin || roll > 360 - RollMin && angle2 <= angle2Max - 10)){// Gite pas assez
        angle2 = angle2 +10;
        ServoEcoute.write(angle2);
        Serial.println(input);
        Serial.print("Angle:");
        Serial.println(angle2);
        Serial.println("--------------------------------");
      }
    } 
  }
}
 

//convert the accel data to pitch/roll
void getAngle(int Vx,int Vy,int Vz) {
  double x = Vx;
  double y = Vy;
  double z = Vz;

  pitch = atan(x/sqrt((y*y) + (z*z)));
  roll = atan(y/sqrt((x*x) + (z*z)));
  //convert radians into degrees
  pitch = pitch * (180.0/3.14);
  roll = roll * (180.0/3.14) ;
}
  
