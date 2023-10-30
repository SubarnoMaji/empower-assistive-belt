//DYNAMIC PARAMETERS
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 nrf(9, 8); 
const byte linkAddress[6] = "link1";

// HYPERPARAMETERS
float adjust_detection = 110.0;
float sensor_offset[3] = {560,550,470};
float beta = 0.82;     // expvg of sound sample
float beta2 = 0.94;    // exp avg of theshold change


//INPUTS
float val[3] = {0,0,0};        //sount input
float EWAval[3] = {0,0,0};     //average sound input
int sensor_detection[3] ={0,0,0};
int sensor_detection_count[3]={0,0,0};
float references[3]={0,0,0};
float thresholds[3]={100,100,100};
int vibrate = -1;
//OUTPUT
int signal[2] = {0,0};

int indexOfMax(int arr[3]) {
    int maxIndex = 0;

    for (int i = 1; i < 3; ++i) {
        if (arr[i] > arr[maxIndex]) {
            maxIndex = i;
        }
    }

    return maxIndex;
}



int maxnn(int n){
  if(n<255) return n;
  else return 255;

}

float absVal(int n) {
    if (n < 0) {
        return -n;
    } else {
        return n;
    }
}

void setup() {
  Serial.begin(9600);
  nrf.begin();
  nrf.openWritingPipe(linkAddress);
  nrf.setPALevel(RF24_PA_HIGH); 
  nrf.stopListening(); 
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  pinMode(10 ,OUTPUT);  
  pinMode(6,INPUT);  
}

void update_thresholds(float n, int i){

  references[i]= beta2*references[i] + (1-beta2)*n;
  thresholds[i]= (references[i] + adjust_detection);

}


float detect_sound(float n,int i){
  
  if(n > thresholds[i]){
    return 1;
  }

  else{ 
    update_thresholds(n,i); 
    return 0 ; 
  }
}


void loop() {
  int a = digitalRead(6);
  Serial.println(a);
  if(digitalRead(6)==HIGH) delay(500);

  val[0] = absVal(analogRead(A0)-sensor_offset[0]);
  val[1] = absVal(analogRead(A1)-sensor_offset[1]);
  val[2] = absVal(analogRead(A2)-sensor_offset[2]);

  for(int i=0 ;i<3;i++){

    EWAval[i] = beta*EWAval[i] + (1-beta)*val[i] ;
    
  }

  if( EWAval[0] >= EWAval[1] && EWAval[0] >= EWAval[2]) {
      vibrate = 0;
    } 
  else if (EWAval[1] >= EWAval[0] && EWAval[1] >= EWAval[2]) {
      vibrate = 1;
    } 
  else {
      vibrate = 2;
    }



  for(int i =0 ;i<3;i++){
    sensor_detection[i] = detect_sound(EWAval[i],i);  
  }



  int flag = 0;

  for(int i =0 ;i<3;i++){
    if(sensor_detection[i]) flag = 1 ;  
  }

  if( flag > 0){
    
    sensor_detection_count[vibrate]++;

    signal[0] = maxnn(EWAval[vibrate] - references[vibrate]);
    signal[1] =indexOfMax(sensor_detection_count);

  }
  else {
    sensor_detection_count[0] =0;
    sensor_detection_count[1] =0;
    sensor_detection_count[2] =0;
    signal[0] =0;
    signal[1] =-1;

  }
 
 
// belt code
    nrf.write(signal, sizeof(signal));

 
}