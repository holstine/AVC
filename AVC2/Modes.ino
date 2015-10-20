
char* modeNames [11]= {
	"pause", 
	"Record Origin",
	"recordWaypointMode",	
	"addWaypointMode",
	"changeWaypointMode",
	"saveDataMode",
	"loadDataMode",
	"clearPointMode",
	"navigate",
	"setPower",
	"debug" 
};

// *************** Pause ************************
void pauseMode(){
	 
	lcd1.begin();
	lcd1<< mode <<"pause  " << _FLOAT(currentPosition.sigma,4); 
	lcd2.begin();
	 brakes(); 
	 autoPower =0;
	if(buttonPressed()){ 
		lcd2<<"Set";
	}else{
		lcd2<<"ready ";
	}
	if(buttonDownTime > 100){
		lcd2<<"GO";
		mode = GOMODE;
		autoPower =1;
		buttonStartTime=millis();
		buttonDownTime=0;
		transmitSerial =false;
		goMode();
	}
	turn(0);
	
}

// *************** GO ************************
void goMode(){ 
	
	flash();
	lcd1.begin();
	lcd2.begin();
	lcd1<< mode <<"Go to " <<waypointIterator ;
	

	drive();
	 
	if(buttonDownTime > 2000){
		stops(); 
		mode = 0;
		buttonStartTime=millis();
		autoPower =0;
	}
	 
}
// ************* Record Origin**************************
void recordOriginMode(){
	static bool wash  = true;
	lcd1.begin();
	lcd1 << mode <<"Record Origin"; 
	lcd2.begin();
	lcd2<<origin.sigma; 
	if(buttonDownTime > 100) {
		lcd1.begin();
		lcd1 << mode <<"Recording Origin"; 
		if ( wash )washOrigin();  
		recordOrigin(); 
		wash= false;
	}else{

		wash = true;
	}
}
// ************* Record Waypoint**************************
void recordWaypointMode(){
	static bool wash = true;
	float x =waypoints[wayPointToRecord].x;
	float y =waypoints[wayPointToRecord].y;
	float sigma = waypoints[wayPointToRecord].sigma; 
	
	lcd1.begin();
	
	lcd2.begin();
	if(buttonPressed()){
		
	}else{
		lcd1 <<mode <<" Record  "<<wayPointToRecord; 
	} 
	lcd2<<x<<" "<<y<<" "<<sigma;
	if(buttonDownTime > 100) {
		lcd1.begin();
		lcd1 <<mode <<" Recording  "<<wayPointToRecord; 
		if (wash)waypoints[wayPointToRecord].sigma =10000; 
		 integrateGPS( waypoints[wayPointToRecord]);
		wash= false;
	}else{
		wash = true;
	}
}



// ************* change Waypoint Number**************************
void changeWaypointMode(){
	lcd1.begin();
	lcd2.begin();

	if(buttonDownTime > 100) {
		lcd1 << mode <<" Changeing WP " ;
		// Only change the waypoint using the knob if the button is down
		readWaypointNumber(); 
		lcd2 << "WP " << wayPointToRecord <<"/" <<numWaypoints;
	}else{
		lcd1 << mode <<" Change WP " ;
		lcd2 << "WP " << wayPointToRecord <<"/" <<numWaypoints;
	}
}

// ************* add Waypoint**************************
void addWaypointMode(){ 
	lcd1.begin();
	lcd2.begin();
	lcd1 << mode <<"Add  " << numWaypoints; 
	if(buttonPressed())lcd2 << "adding"; else lcd2 <<"waiting";
	if(buttonDownTime > 1000) {
		numWaypoints =(numWaypoints+1)%waypointsMax;
		wayPointToRecord =numWaypoints;
		lcd2.begin();	
		lcd2 << "added";
		buttonStartTime=millis();
		buttonDownTime =0;
		mode =2; // go to record mode
	}
}

// ************* Save Data**************************
void saveDataMode(){


	lcd1.begin();
	lcd1<< mode <<"Save Data";
	lcd2.begin();
	lcd2 <<"waiting"; 
	if(buttonDownTime > 1000) {
		saveData();
		lcd2.begin();
		lcd2 <<"saved";
		buttonStartTime=millis();
	}
}

// ************* Load Data**************************
void loadDataMode(){
	static int numTimesLoaded = 0;


	lcd1.begin();
	lcd1<< mode <<"Load Data";
	lcd2.begin();
	lcd2 <<"waiting"; 
	
	if(buttonDownTime > 1000) {
		loadData();
		lcd2.begin();
		lcd2 << "loaded";
		buttonStartTime=millis();
	}
}

void clearPointMode(){
	lcd1.begin();
	lcd2.begin();
	lcd1 <<  mode <<"clear Waypoint" << wayPointToRecord;
	lcd2 <<  mode <<"clear Waypoint" << wayPointToRecord;
	if(buttonDownTime > 1000) {
		washPoint(wayPointToRecord);
		lcd2.begin();
		lcd2 << "washed";
	}
	if(buttonDownTime > 5000) {
		lcd2.begin();
		lcd2 << "reset";
		 numWaypoints = 1;
		waypointIterator=1;
		wayPointToRecord = 1;
	}

}


// ************* Navigation Only**************************

void navigate(){
	lcd1.begin();
	lcd2.begin();
	 
	float x = currentPosition.x;
	float y = currentPosition.y;
	float sigma =currentPosition.sigma;
	
	lcd1 << mode << "Nav " << waypointIterator;
	lcd2<<x<<" "<<y<<" "<<sigma;

	if(buttonDownTime > 1000) {
		restartWaypoints();
		buttonStartTime=millis();
	}
	
	drive();
}
// ************* Estimate Biases**************************
// ************* Get GPS Variance**************************

void setPower(){ 
	lcd1.begin();
	lcd2.begin(); 
	if(buttonPressed()){ 
		go();
		percentPower += 5*readEncoder();
		
	}else {
		stops();
	}
	lcd1 <<  mode <<"demo"  << percentPower;
}
// ************* Debug **************************
void debug(){
	lcd1.begin();
	static int angle = 90;
//	integrateGPS(currentPosition);
//	GPSCoordinates gpscoords= getAbsolutePositionFromGPS();
	//Coordinates coords = absoluteToRelativeCoords(gpscoords);


	lcd2.begin();
	if(buttonPressed()){
		lcd2<<"angle "<<angle;
		myservo.write(angle);
		angle += readEncoder();
		
	}else{ 
		lcd2<<"no button";
	}

	if(buttonDownTime > 1000) {
		Serial << "debug" << "\n";
	}
	lcd1 <<  mode <<"power" << wayPointToRecord;
		lcd1 <<  mode <<"   " << wayPointToRecord;
//	Serial <<"odo: "<<odoCount<<"\t";   
//	Serial <<"readProx: "<<readProx()<<"\t";
//	Serial<<"  heading: " <<  readCompass()<<"\t"; 
//	Serial<<"  x: " <<  coords.x <<"\t";
//	Serial<<"  y: " <<  coords.y <<"\n";
	checkOdometer();
}

// ************* Change Power *************

void competitionMode(){
	  
	autoPower =false;
	// blink something to show ready
	showLCD =false;
	transmitSerial = false;
	transmitTelemetry = false;
	 repeatWaypoints = 0;
	 restartWaypoints(); 
	 transmitFile = false;
	  
}

