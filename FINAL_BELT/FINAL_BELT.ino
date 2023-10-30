#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 nrf(9, 8);  // CE, CSN
const byte linkAddress[6] = "link1"; 
int data[2];
int output;
void setup()
{ 
  Serial.begin(9600); 
  nrf.begin();
  pinMode(3,OUTPUT);  
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  nrf.openReadingPipe(0, linkAddress);  //set the address 
  nrf.startListening();   //Set nrf as receiver
}
void loop()
{
//   Serial.println("A");
//   Serial.println("B")  ;
   if (nrf.available())  //Read the data if available in buffer
     {
      nrf.read(&data, sizeof(data));
      Serial.println(data[1] );
    
      if(data[1]==0) 
      analogWrite(3,data[0]);
      if(data[1]==1)
      analogWrite(5,data[0]);
      if(data[1]==2)
      analogWrite(6,data[0]);
      delay(50);
      digitalWrite(3,LOW);
      digitalWrite(5,LOW);
      digitalWrite(6,LOW);
     }  
}
