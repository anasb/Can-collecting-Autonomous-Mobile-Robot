//DEFINITIONS
#define sensorRXU 28
#define sensorRXD 29
#define sensorBW1 38
#define sensorBW2 39

#define sensorUp  9
#define sensorDown 8

#define trigLeft 45
#define echoLeft 46
#define trigRight 47
#define echoRight 48

#define MotorRF 30
#define MotorRB 31
 
#define MotorLF 32
#define MotorLB 33

#define MotorR 34
#define MotorC 35

#define MotorBL 36
#define MotorBR 37

#define HandUp 42
#define HandDown  43

//INCLUSIONS
#include <SimpleTimer.h>
#include <Wire.h>
#include <WiServer.h>
extern "C" {
  #include "uip.h"
}


//WIRELESS CONFIGURATION
// -------------------------Wireless configuration parameters ----------------------------------------
unsigned char local_ip[]    = {192,168,43,109};   // IP address of WiShield
unsigned char gateway_ip[]  = {192,168,43,1};   // router or gateway IP address
unsigned char subnet_mask[] = {255,255,255,0}; // subnet mask for the local network
char ssid[]                 = {"ALBAN"};   // max 32 bytes
unsigned char security_type = 0;               // 0 - open; 1 - WEP; 2 - WPA; 3 - WPA2

// WPA/WPA2 passphrase
const prog_char security_passphrase[] PROGMEM = {"12345678"};	// max 64 characters

// WEP 128-bit keys
prog_uchar wep_keys[] PROGMEM = { 
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,	// Key 0
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Key 1
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Key 2
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	// Key 3
};

// setup the wireless mode; infrastructure - connect to AP; adhoc - connect to another WiFi device
#define WIRELESS_MODE_INFRA	1
#define WIRELESS_MODE_ADHOC	2
unsigned char wireless_mode = WIRELESS_MODE_INFRA;
unsigned char ssid_len;
unsigned char security_passphrase_len;
//---------------------------------------WIFI OVER---------------------------------------------------


//---------------------------------------GET REQUEST------------------------------------------------
char answerServer[50];
uint8 ip[] = {192,168,43,108};
GETrequest getCommand(ip, 5000, "goni-pc", answerServer);


//-------------------------------------DEFINE TIMER
SimpleTimer timer;
int wd_timer_id;
boolean timerFlag = false;
long timerValue = 0;

//--------------------------------------COMPASS
int HMC6352SlaveAddress = 0x42;
int HMC6352ReadAddress = 0x41; //"A" in hex, A command is: 
int headingValue;

//-------------------------------------ALGORYTHM
String lastCommand;
String reading;
int can=0;
int value=0;
int distanceLeft;
int order;
int oldValue;
int retValue=1;
int prevAngle;
boolean angleFlag=true;
boolean collected=false;
boolean concat=true;
int state = 0;
int prevState = 0;



// Function that prints data from the server
void readCommand(char* data, int len) { 
  reading="";
  while (len-- > 0) {
      reading.concat(*(data++));
  }
  
  String temp;
  for(int a=0; a<reading.length(); a++){
    if(a%2==0)
      temp.concat(reading.charAt(a));
  }
  reading=temp;
  interpret();
  Serial.println(reading);  
}



void interpret(){
  String temp;
 if(reading.charAt(0)=='d'){
   lastCommand="drive";
   temp=subIt(reading, '&');
   value=satoi(temp);
   temp=subIt(reading, '!');
   distanceLeft=satoi(temp);
   temp=reading.substring(1,3);
   order=satoi(temp);
 }
 else if(reading.charAt(0)=='t'){
   lastCommand="driveD";
   temp=subIt(reading, '&');
   value=satoi(temp);
   temp=reading.substring(1,3);
   order=satoi(temp);
 }
 else if(reading.charAt(0)=='r'){\
   lastCommand="rotate";
   temp=subIt(reading, '&');
   value=satoi(temp);
   temp=reading.substring(1,3);
   order=satoi(temp);
 }
 else if(reading.charAt(0)=='g'){
   lastCommand="sensorRight";
 }
 else if(reading.charAt(0)=='l'){
   lastCommand="sensorLeft";
 }
 else if(reading.charAt(0)=='f'){
   lastCommand="RELEASE";
 }

}


String subIt(String str, char a){
  String temp;
  int first, second;
  first=str.indexOf(a);
  second=str.indexOf(a, first+1);
  for(int i=first+1; i<second; i++){
    temp.concat(str.charAt(i));
  }
  return temp;
}

int satoi(String str ){  
  int r = 0;  
  int len = str.length();  
  for(int i=0; i<len; i++){  
     r = r * 10;  
     r += (str.charAt(i)-0x30);  
   }   
  return r; 
}  




void setup() {
  
  //MOTORS
  pinMode(MotorRF, OUTPUT);
  pinMode(MotorRB, OUTPUT);
  pinMode(MotorLF, OUTPUT);
  pinMode(MotorLB, OUTPUT);
  pinMode(MotorR, OUTPUT);
  pinMode(MotorC, OUTPUT);
  pinMode(MotorBL, OUTPUT);
  pinMode(MotorBR, OUTPUT);  
  pinMode(sensorRXU, OUTPUT);
  pinMode(sensorRXD, OUTPUT);
  pinMode(sensorBW1, OUTPUT);
  pinMode(sensorBW2, OUTPUT);
  
  digitalWrite(MotorRF, HIGH);
  digitalWrite(MotorRB, HIGH);
  digitalWrite(MotorLF, HIGH);
  digitalWrite(MotorLB, HIGH);
  digitalWrite(MotorR, HIGH);
  digitalWrite(MotorC, HIGH);
  digitalWrite(MotorBL, HIGH);
  digitalWrite(MotorBR, HIGH);  
  digitalWrite(sensorRXU, LOW);
  digitalWrite(sensorRXD, LOW);
  digitalWrite(sensorBW1, HIGH);
  digitalWrite(sensorBW2, HIGH);
  
  //Manual SENSORS
  pinMode(HandUp, INPUT);
  pinMode(HandDown, INPUT);

  pinMode(trigLeft, OUTPUT);
  digitalWrite(trigLeft, LOW);
  pinMode(echoLeft, INPUT);
  
  pinMode(trigRight, OUTPUT);
  digitalWrite(trigRight, LOW);
  pinMode(echoRight, INPUT);
  
  //COMPASS
  HMC6352SlaveAddress = HMC6352SlaveAddress >> 1; // I know 0x42 is less than 127, but this is still required
 Serial.begin(9600);
  Wire.begin();
  
  WiServer.init(NULL);
  getCommand.setReturnFunc(readCommand);
  lastCommand="ready";
  delay(300);
  Serial.println("done setup");
}


void loop()
{
  timer.run();    
  
  WiServer.server_task();

  if(lastCommand=="GET") openHand();
  else if(lastCommand=="RELEASE") rotateB();
  else if(lastCommand=="turn") turn();
  else if(lastCommand=="openHand") openHand();
  else if(lastCommand=="collect") collect();
  else if(lastCommand=="turnBack") turnBack();
  else if(lastCommand=="drop") drop();
  else if(lastCommand=="closeHand") closeHand();
  else if(lastCommand=="distanceLow") getDistanceLow();
  else if(lastCommand=="distanceUp") getDistanceUpR();
  else if(lastCommand=="getAngle")getAngle();
  else if(lastCommand=="drive")driveToDistance();
  else if(lastCommand=="driveC")driveAndCollect();
  else if(lastCommand=="rotate")rotate90();
  else if(lastCommand=="ready")answer();
  else if(lastCommand=="driveD")driveD();
  else if(lastCommand=="sensorRight")getDistanceRight();
  else if(lastCommand=="sensorLeft")getDistanceLeft();
  
  delay(100);
}

void answer(){
 String temp = "";
 temp.concat("ready");
 temp.concat(order);
 temp.concat("&");
 temp.concat(retValue);
 temp.concat("&");
 temp.concat("!");
 temp.concat(can);
 temp.concat("!");
 temp.toCharArray(answerServer, 50);
 Serial.println(answerServer);
 getCommand.submit();
 //can=0;
}

void turn(){
  if (!timerFlag) {
    timerValue=millis();
    timerFlag=true;
  }
  if(millis()<timerValue+11000){
    digitalWrite(MotorBL, LOW);
  }
  else{
    digitalWrite(MotorBL, HIGH);
    if(lastCommand=="RELEASE") lastCommand="drop";
    else lastCommand="collect";
    timerFlag=false;
  }
}

void turnBack(){
  if (!timerFlag) {
    timerValue=millis();
    timerFlag=true;
  }
  if(millis()<timerValue+11300){
    digitalWrite(MotorBR, LOW);
  }
  else{
    digitalWrite(MotorBR, HIGH);
    can=1; 
    lastCommand = "drive";
    timerFlag=false;
  }
}

int getDistanceLow(){
  digitalWrite(sensorRXD, HIGH);
  delay(30);
  digitalWrite(sensorRXD, LOW);  
  int distance;
  distance = analogRead(sensorDown)/2;
  distance=distance*2.54;
  Serial.print("Distance LOW: ");
  Serial.println(distance);
  return (distance);
}

int getDistanceUp(){
  digitalWrite(sensorRXU, HIGH);
  delay(30);
  digitalWrite(sensorRXU, LOW);  
  int distance;
  distance = analogRead(sensorUp)/2;
  distance=distance*2.54;
  Serial.print("Distance UP: ");
  Serial.println(distance);
  return (distance); 
}

void getDistanceUpR(){
  digitalWrite(sensorRXU, HIGH);
  delay(30);
  digitalWrite(sensorRXU, LOW);  
  int distance;
  distance = analogRead(sensorUp)/2;
  distance=distance*2.54;
  Serial.print("Distance UP: ");
  Serial.println(distance);
  retValue=distance; 
  lastCommand="ready";
}

void getDistanceLeft(){
  int duration;                      // Define the variable we will use to store the pulse duration
  digitalWrite(trigLeft, HIGH);        // Set the trigPin high, start of pulse to get module working
  delayMicroseconds(10);              // Wait 10 mS
  digitalWrite(trigLeft, LOW);         // Set the trigPin low, module will now send a pulse
  duration = pulseIn(echoLeft, HIGH);  // Read the pulse length from the module
  duration = duration/53;             // Convert number to cm
  retValue=duration;
}

void getDistanceRight(){
  int duration2;                      // Define the variable we will use to store the pulse duration
  digitalWrite(trigRight, HIGH);        // Set the trigPin high, start of pulse to get module working
  delayMicroseconds(10);              // Wait 10 mS
  digitalWrite(trigRight, LOW);         // Set the trigPin low, module will now send a pulse
  duration2 = pulseIn(echoRight, HIGH);  // Read the pulse length from the module
  duration2 = duration2/53;             // Convert number to cm
  retValue=duration2;
}

void getAngle(){
    //"Get Data. Compensate and Calculate New Heading"
  Wire.beginTransmission(HMC6352SlaveAddress);
  Wire.write(HMC6352ReadAddress);              // The "Get Data" command
  Wire.endTransmission();
  delay(6);
  Wire.requestFrom(HMC6352SlaveAddress, 2); //get the two data bytes, MSB and LSB
  byte MSB = Wire.read();
  byte LSB = Wire.read();
  float headingSum = (MSB << 8) + LSB; //(MSB / LSB sum)
  int headingInt = headingSum / 10; 
  Serial.print("Angle: ");
  Serial.println(headingInt);
  retValue=headingInt;
}

void rotate90(){
   if (!timerFlag) {
    timerValue=millis();
    timerFlag=true;
  }
  if(millis()<timerValue+3300)rotate(value);
  else{
    rotate(0);
    lastCommand="ready";
    timerFlag=false;
  }  
}

void rotateB(){
    can=0;
    if (!timerFlag) {
    timerValue=millis();
    timerFlag=true;
  }
  if(millis()<timerValue+3300) rotate(1);
  else{
    rotate(0);
    lastCommand="openHand";
    timerFlag=false;
  }    
}

//Drive to a Ceratin Distance
void driveToDistance(){
  int a=getDistanceLow();
  int b=getDistanceUp();
  if(a>value){
    drive(1);
    lastCommand="drive";
  }
  else {
    drive(0);
    if(a+3<b){
      can=1;
      lastCommand="driveC";
    }
    else lastCommand="ready";
  }
}

void driveD(){
  if (!timerFlag) {
    timerValue=millis();
    timerFlag=true;
  }
  if(millis()<timerValue+value*1000) drive(1);
  else{
    drive(0);
    lastCommand="ready";
    timerFlag=false;
  } 
}

void driveAndCollect(){
      Serial.println("DRIVE C");
  int a = getDistanceLow();
  can=1;
  if(a>15){
    drive(1);
    lastCommand="driveC";
  }
  else {
    drive(0);
    lastCommand="GET";
    }
}

void objectFound(){
 int distance;
 distance = min(2,3);
}

void collect(){
  state = digitalRead(HandDown);
  if( state == LOW) 
    digitalWrite(MotorC, LOW);
  else {
    digitalWrite(MotorC, HIGH);
    lastCommand= "turnBack";
    }
  }
  
void openHand(){
  state = digitalRead(HandUp);
  if( state == LOW) 
    digitalWrite(MotorC, LOW);
  else {
    digitalWrite(MotorC, HIGH);
    if (lastCommand=="GET") lastCommand="turn";
    else lastCommand="closeHand";
    }
  }
  
void drop(){
    state = digitalRead(HandUp);
  if( state == LOW) 
    digitalWrite(MotorR, LOW);
  else {
    digitalWrite(MotorR, HIGH);
    lastCommand= "closeHand";
     }
  }
  
void closeHand(){
    state = digitalRead(HandDown);
  if( state == LOW) 
    digitalWrite(MotorR, LOW);
  else {
    digitalWrite(MotorR, HIGH);
    lastCommand= "turnBack";
     }
  }

void drive(int flag){
 if (flag==1){
   digitalWrite(MotorRB, HIGH);
   digitalWrite(MotorLB, HIGH);
   digitalWrite(MotorRF, LOW);
   digitalWrite(MotorLF, LOW);
 }
 else if (flag==-1){
   digitalWrite(MotorRF, LOW);
   digitalWrite(MotorLF, LOW);
   digitalWrite(MotorRB, HIGH);
   digitalWrite(MotorLB, HIGH);
 }
 else if (flag==0){
   digitalWrite(MotorRF, HIGH);
   digitalWrite(MotorLF, HIGH);
   digitalWrite(MotorRB, HIGH);
   digitalWrite(MotorLB, HIGH);
 }
}

void rotate(int flag){
 if (flag==1){
   digitalWrite(MotorRF, HIGH);
   digitalWrite(MotorRB, LOW);
   digitalWrite(MotorLF, LOW);
   digitalWrite(MotorLB, HIGH);
 }
 else if (flag==-1){
   digitalWrite(MotorRF, LOW);
   digitalWrite(MotorRB, HIGH);
   digitalWrite(MotorLF, HIGH);
   digitalWrite(MotorLB, LOW);
 }
 else if (flag==0){
   digitalWrite(MotorRF, HIGH);
   digitalWrite(MotorRB, HIGH);
   digitalWrite(MotorLF, HIGH);
   digitalWrite(MotorLB, HIGH);
 }
}


extern "C" {

   // Not needed for this example but must exist
   void udpapp_init(void)
   {
   }

   // Not needed for this example but must exist
   void dummy_app_appcall(void)
   {
   }
}

