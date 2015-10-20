#include <Wire.h>
#include <Servo.h> 
#include <math.h>
//#include "TimerOne.h"


double waypoints [][2] = { 
  {5, 50}, 
  {90, 50},
  {90, -50},
   {0, 50}
};

int numWaypoints = 4;
int waypointIterator=0;
int prox;

int N = 0; // indicies for waypoints
int E = 1; 
double currentWaypoint[2] ;
double currentPosition[2] = {0, 0};
double CLOSEENOUGH = 10;
double DISTANCEPERROTATION = 9.5;
int DIRECTION = 1;

Servo myservo;  // create servo object to control a servo 
                // a maximum of eight servo objects can be created 
 
int steeringPos = 0;    // variable to store the servo position 
int CENTER = 35;
int MAX_TURN = 65;
int MIN_TURN = 5;
//int HMC6352Address = 0x42;
#define Addr 0x1E  
// This is calculated in the setup() function
//int slaveAddress;

int ledPin = 13;
int STEERING_PIN = 6;
int ODOMETER_PIN = A3;
int PROXIMITY_PIN = A0;
int FORWARD_PIN =A1;
int BACKWARD_PIN =A2;
int IOIO_PIN = 11;

boolean obstacle = false;

boolean ledState = false;
byte headingData[2];
int i, headingValue;
float heading ;
long odoCount=0;
boolean odoState= true;

double headingToWaypoint = 0;
  double steeringDifference =0;
int count =0;


// ***********************************  SETUP ***************************************
void setup(){
  
  delay(1000);     
  
    Serial.begin(9600);
  // Shift the device's documented slave address (0x42) 1 bit right
  // This compensates for how the TWI library only wants the
  // 7 most significant bits (with the high bit padded with 0)
//  slaveAddress = HMC6352Address >> 1;   // This results in 0x21 as the address to pass to TWI

  pinMode(ledPin, OUTPUT);      // Set the LED pin as output
  
   turn(30);
//     Timer1.initialize(5000);         // initialize timer1, and set a 1/2 second period 
//    Timer1.attachInterrupt(updatePosition);  // attaches callback() as a timer overflow interrupt
  pinMode(FORWARD_PIN, OUTPUT);
  pinMode(BACKWARD_PIN, OUTPUT);
  pinMode(STEERING_PIN, OUTPUT);

  pinMode(ODOMETER_PIN, INPUT);
  pinMode(PROXIMITY_PIN, INPUT);
  
 delay(1000);     
  turn(0);// Power up delay
  Wire.begin();
  
  // Set operating mode to continuous
  Wire.beginTransmission(Addr); 
  Wire.write(0x02);
  Wire.write(0x00);
  Wire.endTransmission();
  
  
  myservo.attach(STEERING_PIN);  
   currentWaypoint[N] = waypoints[waypointIterator][N];
   currentWaypoint[E] = waypoints[waypointIterator][E];
 
}
// **************************************** LOOP *****************************
void loop()
{
 flash();
// go();
  heading = readCompass();
  updatePosition();
 if(count++ %500 ==0) printState();
  steer();
  updateWaypoint();
  
//  breaks();
if(digitalRead(IOIO_PIN)){
 stops(); 
}else{
  go();
}
// backup(); 
}

// ***********************************
void breaks(){
   float prox = readProx();
   //  Serial.println(prox);
   if(prox < 20){
      stops();
      delay(1000);
      backup();
      turn(-steeringDifference);
      delay(200);
      stops();
      delay(200);
      go();
   }
}
// ***********************************
void updateWaypoint(){
  double deltaN = currentWaypoint[N] - currentPosition[N];
  double deltaE = currentWaypoint[E] - currentPosition[E];   
  
  double distance2 = deltaN*deltaN + deltaE*deltaE;
  
  if(distance2 < CLOSEENOUGH) {
    
    waypointIterator = ++waypointIterator % numWaypoints;
    
    currentWaypoint[N] = waypoints[waypointIterator][N];
    currentWaypoint[E] = waypoints[waypointIterator][E];
  }
  
  
}
// ***********************************
void steer(){
  
  double deltaN = currentWaypoint[N] - currentPosition[N];
  double deltaE = currentWaypoint[E] - currentPosition[E];    
    
   headingToWaypoint = -180/3.1415926 * atan2(-deltaE,deltaN);
   steeringDifference = headingToWaypoint - heading;
  if(steeringDifference > 180) steeringDifference -=360;
  if(steeringDifference < -180) steeringDifference +=360;
  
   if(prox < 50) steeringDifference -= (50-prox)/2.0;  // go left more and more urgently as obstacle approaches
  
  turn(steeringDifference/2.0);
  
}

// ***********************************
void flash(){
 // Flash the LED on pin 13 just to show that something is happening
  // Also serves as an indication that we're not "stuck" waiting for TWI data
  ledState = !ledState;
  if (ledState) {
    digitalWrite(ledPin,HIGH);
  }
  else
  {
    digitalWrite(ledPin,LOW);
  }
}

// ***********************************
void turn(int deg){
  int turnTo = deg + CENTER;
  if (turnTo > MAX_TURN) turnTo = MAX_TURN;
   if (turnTo < MIN_TURN) turnTo = MIN_TURN;
  myservo.write(turnTo);
}

// ***********************************
void go (){
 digitalWrite(FORWARD_PIN, HIGH); 
 DIRECTION = 1;
}

// ***********************************
void backup (){
 digitalWrite(BACKWARD_PIN, HIGH); 
 DIRECTION = -1;
}

// ***********************************
void stops(){ 
 digitalWrite(FORWARD_PIN, LOW); 
 digitalWrite(BACKWARD_PIN, LOW); 
}

// ***********************************
int readOdo(){
  
    int val=  analogRead(ODOMETER_PIN);
    return val;
}


// ***********************************
int readProx(){
    int val=  analogRead(PROXIMITY_PIN);
    prox = (6.0*prox/10.0) + (4.0*val/10.0);
      return prox;
}

// ***********************************
//float readCompassOld(){
//  
//   // Send a "A" command to the HMC6352
//  // This requests the current heading data
//  Wire.beginTransmission(slaveAddress);
//  Wire.send("A");              // The "Get Data" command
//  Wire.endTransmission();
//  delay(1);                   // The HMC6352 needs at least a 70us (microsecond) delay
//  // after this command.  Using 10ms just makes it safe
//  // Read the 2 heading bytes, MSB first
//  // The resulting 16bit word is the compass heading in 10th's of a degree
//  // For example: a heading of 1345 would be 134.5 degrees
//  Wire.requestFrom(slaveAddress, 2);        // Request the 2 byte heading (MSB comes first)
//  i = 0;
//  while(Wire.available() && i < 2)
//  { 
//    headingData[i] = Wire.receive();
//    i++;
//  }
//  headingValue = headingData[0]*256 + headingData[1];  // Put the MSB and LSB together
//  
//  return headingValue/10.0;
//}
float readCompass(){
 int x, y, z;

  // Initiate communications with compass
  Wire.beginTransmission(Addr);
  Wire.write(0x03);                         // Send request to X MSB register
  Wire.endTransmission();

  Wire.requestFrom(Addr, 6);               // Request 6 bytes; 2 bytes per axis
  if(Wire.available() <=6) {               // If 6 bytes available
    x = Wire.read() << 8 | Wire.read();
    z = Wire.read() << 8 | Wire.read();
    y = Wire.read() << 8 | Wire.read();
    float val = 180.0 / 3.1415926 *atan2(-x,y);
    return val;
  }
    return 0.0;
  }
// ***********************************
void updatePosition(){
   int val=  analogRead(ODOMETER_PIN);
   if(odoState && (val > 110)){
     odoCount ++;
     odoState = !odoState;
      updateCurrentPosition();
   }
    if(!odoState && (val < 85)){
      odoCount ++;
      odoState = !odoState;
       updateCurrentPosition();
    }
}

// ***********************************
void updateCurrentPosition(){
  float x = sin(heading/180.0*3.1415926);
  float y = cos(heading/180.0*3.1415926);
  x = x*DISTANCEPERROTATION * DIRECTION;
  y = y*DISTANCEPERROTATION * DIRECTION;
  currentPosition[E] = currentPosition[E] + x;
  currentPosition[N] = currentPosition[N] + y;
}

// ***********************************
void printState(){
      
       Serial.print("prox: ");
     Serial.println(readProx());
//     Serial.print("  "); 
     Serial.print("Current heading: ");
     Serial.print(heading);     
        Serial.print("  headingToWaypoint: ");
     Serial.print(headingToWaypoint);   
     Serial.println();
      Serial.println();
      Serial.print("currentPosition: ");
     Serial.print(currentPosition[N]);     
      Serial.print("  ");     
       Serial.print(currentPosition[E]);     
     Serial.println();
     
       Serial.print("currentWaypoint: ");
     Serial.print(currentWaypoint[N]);     
      Serial.print("  ");     
       Serial.print(currentWaypoint[E]);     
     Serial.println();
     
     
      Serial.println();
}
