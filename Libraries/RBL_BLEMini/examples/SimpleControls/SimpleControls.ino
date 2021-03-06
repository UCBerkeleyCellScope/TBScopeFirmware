/*

Copyright (c) 2012, 2013 RedBearLab

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include <ble_mini.h>
#include <Servo.h> 
 
#define DIGITAL_OUT_PIN    13
#define DIGITAL_IN_PIN     5
#define PWM_PIN            6
#define SERVO_PIN          7
#define ANALOG_IN_PIN      A5

#define FAR_RED_LED 10 //CHOSEN FOR PWM CAPABILITY

#include <SoftwareSerial.h>

//SoftwareSerial mySerial(10, 11); // RX, TX

Servo myservo;

unsigned long currentMillis;        // store the current value from millis()
unsigned long previousMillis;       // for comparison with currentMillis
int samplingInterval = 250;          // how often to run the main loop (in ms)

void setup()
{
  //Serial.begin(57600);

  
  BLEMini_begin(57600);
  
  //Serial.println("Hello world");
  
  pinMode(DIGITAL_OUT_PIN, OUTPUT);
  pinMode(DIGITAL_IN_PIN, INPUT);
  pinMode(FAR_RED_LED, OUTPUT);
  
  // Default to internally pull high, change it if you need
  digitalWrite(DIGITAL_IN_PIN, HIGH);
  //digitalWrite(DIGITAL_IN_PIN, LOW);
  
  myservo.attach(SERVO_PIN);
}

void loop()
{
  static boolean analog_enabled = false;
  static byte old_state = LOW;
   
  
  // If data is ready
  while ( BLEMini_available() == 3 )
  //while ( BLEMini_available() == 3 )
  {

  /*    
      digitalWrite(13,HIGH);
      delay(100);
      digitalWrite(13,LOW);
 */   
    // read out command and data
    byte data0 = BLEMini_read();
    byte data1 = BLEMini_read();
    byte data2 = BLEMini_read();    
    
//    BLEMini_write(0x0A);
//    BLEMini_write(0x00);
//    BLEMini_write(0x00);
    
    if (data0 == 0x01)  // Command is to control digital out pin
    {
//      BLEMini_write(0x0A);
//      BLEMini_write(0x00);
//      BLEMini_write(0x00);
           
      //Serial.println("Controlling D_OUT pin!");
      if (data1 == 0x01)
        digitalWrite(DIGITAL_OUT_PIN, HIGH);
      else
        digitalWrite(DIGITAL_OUT_PIN, LOW);
    }
    else if (data0 == 0xA0) // Command is to enable analog in reading
    {
      //Serial.println("Controlling A_IN pin!");
      if (data1 == 0x01)
        analog_enabled = true;
      else
        analog_enabled = false;
    }
    else if (data0 == 0x02) // Command is to control PWM pin
    {
      analogWrite(PWM_PIN, data1);
    }
    else if (data0 == 0x03)  // Command is to control Servo pin
    {
      myservo.write(data1);
    }
    
    
    else if (data0 == 0x04) // Command is to reset
    {
      analog_enabled = false;
      myservo.write(0);
      analogWrite(PWM_PIN, 0);
      digitalWrite(DIGITAL_OUT_PIN, LOW);
    }
    
    /*
    else if (data0 == 0x0A) 
    {
      if (data1 == 0x01)
        digitalWrite(FAR_RED_LED, HIGH);
      else
        digitalWrite(FAR_RED_LED, LOW);
      //analogWrite(FAR_RED_LED, data1);
    }
    */
  }
  
  if (analog_enabled)  // if analog reading enabled
  {
    currentMillis = millis();
    if (currentMillis - previousMillis > samplingInterval)
    {
      previousMillis += millis();
      
      // Read and send out
      uint16_t value = analogRead(ANALOG_IN_PIN); 
      BLEMini_write(0x0B);
      BLEMini_write(value >> 8);
      BLEMini_write(value);
    }
  }
  
  // If digital in changes, report the state
  if (digitalRead(DIGITAL_IN_PIN) != old_state)
  {
    old_state = digitalRead(DIGITAL_IN_PIN);
    
    if (digitalRead(DIGITAL_IN_PIN) == HIGH)
    {
      BLEMini_write(0x0A);
      BLEMini_write(0x01);
      BLEMini_write(0x00);    
    }
    else
    {
      BLEMini_write(0x0A);
      BLEMini_write(0x00);
      BLEMini_write(0x00);
    }
  }
  
  delay(100);
}

