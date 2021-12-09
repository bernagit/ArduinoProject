#include <RTClib.h>
#include <SevSeg.h>
#include <IRremote.h>

int RECV_PIN = A1;
byte numDigits = 4;   
byte digitPins[] = {10, 11, 12, 13}; //posizione nel display: 1,2,3,4
byte segmentPins[] = {2, 3, 4, 5, 6, 7, 8, 9}; //Segmenti: A,B,C,D,E,F,G, PuntoBasso


IRrecv irrecv(RECV_PIN);     
decode_results results;
RTC_DS1307 rtc; //Instantiate a rtc element
SevSeg sevseg; //Instantiate a seven segment controller object

int updateTime = 2000;
unsigned long timenow = 0;
int updateTimeIr = 100;
unsigned long timenowIr = 0;
bool state = false;

void setup(){
  
  Serial.begin(9600);
  sevseg.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins);
  sevseg.setBrightness(10);


  if(!rtc.begin()){
    Serial.print("Errore nell'inizializzazione dell'orologio");
    return;
  }
  if(!rtc.isrunning()){
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  irrecv.enableIRIn();
  
  delay(2500);
}

void loop(){
  int value;
  if( millis() > timenowIr + updateTimeIr ){
    timenowIr = millis();
    if (irrecv.decode(&results)){
      value=results.value;
      if(value == 0x182D || value == 0x102D){
        state = !state;
      }
      irrecv.resume(); 
    }
  }
  int orario;
  if( millis() > timenow + updateTime){
    timenow = millis();
    DateTime now = rtc.now();
    int o= now.hour();
    int ore = o*100;
    int minuti = now.minute();
    orario = ore+minuti;
    Serial.println(" - Ora Aggiornata");
    Serial.print(state);
  }
  if(state){
    sevseg.setNumber(orario,2);
    sevseg.refreshDisplay();
  }
  else{
    sevseg.blank();
  }
}
