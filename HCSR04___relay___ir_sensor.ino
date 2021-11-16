#include <HCSR04.h>
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <Filters.h> 

#define ACS_Pin A0  
#define ACS_Pin1 A1
 

#include "EmonLib.h"             
EnergyMonitor emon1; 

float ACS_Value;
float ACS_Value1;

                              
float testFrequency = 50;                    
float windowLength = 40.0/testFrequency;     


float intercept = 0; 
float slope = 0.0752; 

float Amps_TRMS;
float Amps_TRMS1;


unsigned long printPeriod = 1000; 
 
unsigned long previousMillis = 0;

HCSR04 hc(6,new int[2]{5,7},2);//initialisation class HCSR04 (trig pin , echo pin)

/////////////////////RELAY//////////////////
int relay = 2; 
int relay1 = 3;


////////////////////SENSOR//////////////////
int sensor = 8;
int valuesensor = 0;

int delayy = 1;
int value = 0;

int state2 = 0;
int state3 = 0;
int state4 = 0;


int stateultrasonic1 = 0;
int stateultrasonic2 = 0;

int valueultrasonic1 = 0;
int valueultrasonic2 = 0;


int signalrosak = 10;
int signalrosak1 = 11;


void setup()
{ Serial.begin(9600); 
  pinMode(signalrosak,OUTPUT);
  pinMode(signalrosak1,OUTPUT);
 
  pinMode(relay,OUTPUT);
  pinMode(relay1, OUTPUT);
 
  pinMode(sensor,INPUT);
  digitalWrite (relay, HIGH);
  digitalWrite (relay1, HIGH);

  pinMode(ACS_Pin,INPUT);  
  pinMode(ACS_Pin1,INPUT); 
   
  emon1.voltage(3, 234.26, 1.7);

  Serial.println("COMBINE ULTRSONIC + CURRENT S +VOLTAGE S"); // print some text in Serial Monitor
  Serial.println("FYP 2021");
}

void loop(){ 

  RunningStatistics inputStats;  
  RunningStatistics inputStats1; 
            
  inputStats.setWindowSecs( windowLength );  
  inputStats1.setWindowSecs( windowLength );    
   
  while( true ) {   
    ACS_Value = analogRead(ACS_Pin);  
    ACS_Value1 = analogRead(ACS_Pin1);
    
    inputStats.input(ACS_Value);  
    inputStats1.input(ACS_Value1);
        
    if((unsigned long)(millis() - previousMillis) >= printPeriod) { 
      previousMillis = millis();  
      
      Amps_TRMS = intercept + slope * inputStats.sigma();
      Amps_TRMS1 = intercept + slope * inputStats1.sigma();

       emon1.calcVI(20,2000);                          
        float supplyVoltage   = emon1.Vrms;     

      Serial.print ("VOLTAGE =");
      Serial.println (supplyVoltage);
      
      Serial.print ("CURRENTS = ");
      Serial.print( "Amps1: " ); 
      Serial.print( Amps_TRMS );
      Serial.print( " Amps2: " ); 
      Serial.println( Amps_TRMS1 );
      
      Serial.print("ULTRASONIC = ")
      Serial.print("ULTRASONIC 1: ")
      Serial.print( hc.dist(0) );
      delay (5);
      Serial.print(" ULTRASONIC 2: ")
      Serial.println( hc.dist(1) );
      delay (5);

  //Serial.println( hc.dist()); 
  tmElements_t tm;
  RTC.read(tm);
  Serial.print("time : ");
  Serial.println (tm.Minute);

  if (tm.Hour == 8 || tm.Hour == 9 || tm.Hour == 10 || tm.Hour == 11 || tm.Hour == 12 || tm.Hour == 13 || tm.Hour == 14 || tm.Hour == 15 || tm.Hour == 16 || tm.Hour == 17){

    if (supplyVoltage >= 200 && Amps_TRMS <= 1){
      digitalWrite (signalrosak, HIGH);
      delay (5000);
      digitalWrite (signalrosak, LOW);
      }

    if (supplyVoltage >= 200 && Amps_TRMS1 <= 1){
      digitalWrite (signalrosak1, HIGH);
      delay (5000);
      digitalWrite (signalrosak1, LOW);
      }
    }
  
  valuesensor = digitalRead (sensor);
  
  if (valuesensor == 0 && state3 == 0){
    state3 = 1;
    state2 = 1;
    state4 = 1;
  
  }
  else if (valuesensor == 0 && state3 == 1){
    state3 = 0;
    state2 = 0;
    state4 = 0;
    
    }
    
  
if (state2 == 0){  /////utk tukar posisi suis dri 0 ke 1
if (hc.dist(0) >= 9){

  if (stateultrasonic1 == 0){/////sini di fix masa x berubah sbb nak set masa utk lampu mati plus delay utk 2 minit
  valueultrasonic1 = tm.Minute + delayy;
  Serial.print("value : ");
  Serial.println(valueultrasonic1);
  stateultrasonic1 = 1;
  }
  value = tm.Minute; //////sini dye berulang plus ngan mse y bjln
  if (valueultrasonic1 == value ){
    digitalWrite (relay, LOW);
    stateultrasonic1 = 0;
    }
  }

}
delay(100);

if (state4 == 0){
 if (hc.dist(1) >= 9){
  
  if (stateultrasonic2 == 0){/////sini di fix masa x berubah sbb nak set masa utk lampu mati plus delay utk 2 minit
  valueultrasonic2 = tm.Minute + delayy;
  Serial.print("value1 : ");
  Serial.println(valueultrasonic2);
  stateultrasonic2 = 1;
  }
  value = tm.Minute; //////sini dye berulang plus ngan mse y bjln
  if (valueultrasonic2 == value ){
    digitalWrite (relay1, LOW);
    stateultrasonic2 = 0;
    }
}
}

//------------------------------------change position-----------------------------------------------------
if (state2 == 1){  
  
if (hc.dist(0) >= 9){

  if (stateultrasonic1 == 0){/////sini di fix masa x berubah sbb nak set masa utk lampu mati plus delay utk 2 minit
  valueultrasonic1 = tm.Minute + delayy;
  Serial.println(valueultrasonic1);
  stateultrasonic1 = 1;
  }
  value = tm.Minute; //////sini dye berulang plus ngan mse y bjln
  if (valueultrasonic1 == value ){
    digitalWrite (relay, HIGH);
    stateultrasonic1 = 0;
    }
   }
 }
delay(100);
if (state4 == 1){
 if (hc.dist(1) >= 9){
  if (stateultrasonic2 == 0){/////sini di fix masa x berubah sbb nak set masa utk lampu mati plus delay utk 2 minit
  valueultrasonic2 = tm.Minute + delayy;
  Serial.println(valueultrasonic2);
  stateultrasonic2 = 1;
  }
  value = tm.Minute; //////sini dye berulang plus ngan mse y bjln
  if (valueultrasonic2 == value ){
    digitalWrite (relay1, HIGH);
    stateultrasonic2 = 0;
    }
  }
}
delay (200);

 }
}
}//return curent distance in serial
