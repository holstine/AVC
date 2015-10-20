#include <Adafruit_L3GD20.h>
#include <L3G4200D.h>
#include <SD.h>
#include <Servo.h>
#include <Adafruit_GPS.h>
#include <PWMServo.h>
#include <PString.h>
#include <Streaming.h>
#include <serLCD.h>
#include "testdefs.h"
#include <EEPROMEx.h>
#include <Wire.h>
#include <math.h>
#include <TinyGPS.h>
#include <SoftwareSerial.h>    
#include <L3G4200D.h>

L3G4200D gyroscope;
#define TO_RAD(x) (x * 0.01745329252)  // *pi/180
#define TO_DEG(x) (x * 57.2957795131)  // *180/pi

#define PROXIMITY_PIN		A0
#define BACKWARD_PIN 		A2
// Wire is on A4 And A5 fo UNO
// wire on mega is 20 (yellow) and 21 (green)
#define KNOBBUTTONPIN 				22
#define GOPIN				25
#define COMTX 				2
#define ODOMETER_PIN		3   // interupt 1
#define SPEED_PIN			7
#define MODEREAD1 			8
#define MODEREAD2 			9
#define STEERING_PIN 		12  // pwm servo works on 11,12,13 for mega
#define FORWARD_PIN			11
#define ledPin				13
#define FILE_PIN			53
/*
extra pin notes:
Serial3 is bluetooth pin 14 15
GPS uses Serial2 pin 16 17  (yellow gps tx to 16 orang gps rx to 17)
*/

File myFile; 
int fileNumber;

int  year[1];
byte month[1];
byte  day[1];
byte  hour[1];
byte  minute[1];
byte  second[1];
byte  hundreths[1];

float gpsCourse;
float gpsSpeed;

#define	BLUETOOTH Serial3
 
Stream * cx;

int waypointCountAddress =0;
int waypointsAddress = 0;
int originAddress = 0;
int calibrationAddress = 0;
#define calibrationsMax  10

#define waypointsMax  35  // thats all that fits in UNO epromm

int mode = 8;
int maxModes = 11;
int prevMode = 0;

#define GOMODE		100

char buffer1[16];
PString lcd1(buffer1, sizeof(buffer1));
char buffer2[16];
PString lcd2(buffer2, sizeof(buffer2)); 




// Start heading to waypoint 0.
// when done return to waypoint zero.
int numWaypoints = 0;
int waypointIterator=0;
int wayPointToRecord = 0;
int repeatWaypoints = 1;

Coordinates waypoints [waypointsMax] = { // feet
	{0, 5,10000000}, // return to origin unless otherwise specified
	{9, 5,10000000},
	{9, -5,10000000},
	{0, 5,10000000}
};
 
unsigned long buttonStartTime;
unsigned long buttonDownTime;

GPSCoordinates origin;
float deciMicroDegreesToMeters = .0111180;
float sineOfLat =.7;

float HDOP_WEIGHT = .1;  // given hdop find sigma

int prox;

Coordinates currentPosition = {0, 0,5000000};
float CLOSEENOUGH = .5;// meters
float DISTANCEPERROTATION = 0.3506; //meters
float ROTATIONPERCLICK = 26.0/143.0;  // ODO Wheel turn per click
float UNCERTAINTY_PER_STEP = .01;// meters

float GPS_UNCERTAINTY_FACTOR = 1;
float DEAD_RECONING_UNCERTAINTY_FACTOR =1;
float COMPASS_UNCERTAINTY_FACTOR =1;
float GYRO_UNCERTAINTY_FACTOR =1;
float COURSE_UNCERTAINTY_FACTOR =7;
float WHEEL_FUDGE_FACTOR =1;
float YAW_RATE_FUDGE_FACTOR = 1;
float  ROLL_PITCH_FUDGE = 1.0f;
int calibrateMag = 0;

int DIRECTION = 1;

PWMServo myservo;  
PWMServo myEscServo;  

int CENTER = 100;
int MAX_TURN = 180;
int MIN_TURN = 0;
//int HMC6352Address = 0x42;
#define Addr 0x1E  
// This is calculated in the setup() function
//int slaveAddress;


// ****** PIN DEFINITIONS **********

#define ENC_PORT PINB
 
bool autoPower = 1;
int percentPower =50;
float steeringSeverity =1;
int buttonOverride =0;
bool steeringEnable =true; 

bool transmitTelemetry = true;
bool transmitSerial = true;
bool showLCD = true;
bool transmitFile = true;

float  manual_magn_offset [3] = {0.0f, 0.0f, 0.0f}; 

float  manual_magn_scale  [3]  =  {1.0f, 1.0f, 1.0f}; 


boolean obstacle = false;
int turning =0;
boolean ledState = false;

long odoCount=0;
boolean odoState= true;
volatile long tempOdoCount =0;
int overrideEncoderValue =0;
int outputRefreshRate = 1000;

 
double heading =0 ;
double headingSigma=10000000;

double headingToWaypoint = 0;
double steeringDifference =0;

//SoftwareSerial gpsDevice(GPSRX, GPSTX);
serLCD screen(COMTX);
TinyGPS gps;
Adafruit_GPS GPS(&Serial2); 



// ***********************************  SETUP ***************************************
void setup(){


	waypointCountAddress = EEPROM.getAddress(sizeof(int));
	originAddress = EEPROM.getAddress(sizeof(GPSCoordinates));
	waypointsAddress = EEPROM.getAddress(sizeof(Coordinates)*waypointsMax);
	calibrationAddress  = EEPROM.getAddress(sizeof(float)*calibrationsMax);

	Serial.begin(115200);
	
	BLUETOOTH.begin(115200); // bluetooth 

	screen.begin(9600);

	resetTo16CharactersLCD();
	//screen.autoscroll();
	lcd1.begin();
	lcd2.begin();
	lcd1  <<"Startup";
	lcd2  <<"Please Wait"; 
	displayScreen();
	buttonStartTime = millis();

	setPins();

	attachInterrupt(1, odoInterupt, CHANGE);

	setupFileIO();// Power up delay

	initGPS();
	armCompass();
	//armMagnetometer();
	//armGyro();
	armSteering();
	resetOrigin();
	armMotorControler();
	
	//BLUETOOTH.write("$$$");
	//delay(100);
//	BLUETOOTH.write( "SN,AVC2");
//	BLUETOOTH.write("$$$");

	rawTenDofSetup();
	//stops();
	//loadData();
	// let things settle in before send

}
long loopMills;
// **************************************** LOOP *****************************
// **************************************** LOOP *****************************
void loop(){ 
	
	unsigned long mills = millis();
	 
	if(!buttonPressed()){ 
		buttonStartTime = mills;
	}	
	buttonDownTime = mills - buttonStartTime;


	if(goButtonPressed()) autoPower = true;


	serialSentinal(mills); 

	readMode();  
	
	if(mode ==0) pauseMode();	
	else if(mode ==1) recordOriginMode();
	else if(mode ==2) recordWaypointMode();
	else if(mode ==3) addWaypointMode();
	else if(mode ==4) changeWaypointMode();
	else if(mode ==5) saveDataMode();
	else if(mode ==6) loadDataMode();
	else if(mode ==7) clearPointMode();
	else if(mode ==8) navigate();
	else if(mode ==9) setPower();
	else if(mode ==10) debug();

	else if(mode ==GOMODE) goMode(); 

	loopMills =millis()-mills;
}

long drivemill=0;
void drive(){
	
	long start = millis();
	go(); 
	flash();
	//stops();
	steer();
	rawTenDofLoop();
	
	
	findHeading();
	checkOdometer();
	updateWaypoint();
	integrateGPS(currentPosition);

	//drivemill = millis()-start;
}


// ***********************************
void updateWaypoint(){ 
	if(distanceToWaypoint() < CLOSEENOUGH || isUnreachable() ) {
		waypointIterator++;
		
		if(waypointIterator == numWaypoints){
			waypointIterator =0; 
			if(repeatWaypoints ==0){
				autoPower =false;  // turn off power to motor
			}
		}
		wayPointToRecord = waypointIterator-1; 
	} 
}



// *********************************
bool brakes(){
	float prox = readProx();
	if(prox < 20){
		backup();
		turn(-90); 
		return true;
	}
	return false;
}

// *********************************** [                                                                                                                                                                                                                                           
void steer(){

	double deltaN = waypoints[waypointIterator].y - currentPosition.y;
	double deltaE = waypoints[waypointIterator].x - currentPosition.x;    

	headingToWaypoint = -180/3.1415926 * atan2(-deltaE,deltaN);
	steeringDifference = headingToWaypoint - heading;
	if(steeringDifference > 180) steeringDifference -=360;
	if(steeringDifference < -180) steeringDifference +=360;
//	Serial <<"steeringDifference: "<<steeringDifference << "\n";
//	if(prox < 50) steeringDifference -= (50-prox)/2.0;  // go left more and more urgently as obstacle approaches
	turning = steeringDifference*steeringSeverity; 
	turn(turning);
}

// ***********************************
void updateCurrentPositionFromOdo(int count){
	if (count ==0 ) return ;
	float x = sin(heading/180.0*3.1415926);
	float y = cos(heading/180.0*3.1415926);
	x = x*DISTANCEPERROTATION * WHEEL_FUDGE_FACTOR* ROTATIONPERCLICK * DIRECTION * count;
	y = y*DISTANCEPERROTATION * WHEEL_FUDGE_FACTOR* ROTATIONPERCLICK * DIRECTION * count;
	currentPosition.x = currentPosition.x + x;
	currentPosition.y = currentPosition.y + y;
	// the positional certainty degrades as we step forward;
	currentPosition.sigma = currentPosition.sigma + UNCERTAINTY_PER_STEP* DEAD_RECONING_UNCERTAINTY_FACTOR;
}


boolean isUnreachable(){
	static float turningRadius = 1; // 1 meter turning radius
	 
	double minDist = 2* turningRadius* fabs(sin(TO_RAD( steeringDifference) ));
//	Serial<< steeringDifference << " " <<distanceToWaypoint() << " " <<minDist << " " << endl;
	if(distanceToWaypoint() > minDist){
		return false;
	}else{
		return true;
	}


}
// **************** @TODONE ***********************
// linear least squares kahlman filter 
// get all compnents working
// add arming switch to transmitter

// re add odometry
// ADD Bluetooth telemetry
// android app to understand telemetry
// Assemble components
// input output made nice

// ***************** @TODO **********************

// TEST THE DAMN THING OUT!!!!!!  (repeat alot!) 

// calibrate components 

// Reverse mode
// proximity mode
 

// CUT POWER FROM ESC TO ARDUINO(WITH SWITCH)





