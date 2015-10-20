
// *************************************
int readMode(){ 
	static int encoder =0;
	static int maxEncoder =0;
	if(!buttonPressed()){  // only read mode if button not pressed
		prevMode = mode;
		encoder += readEncoder() ;
		if(encoder >maxEncoder){
			encoder =0;
			changeMode(1);
		}
		if(encoder <0){
			encoder =maxEncoder;
			changeMode(-1);
		}
		if(mode == GOMODE) return GOMODE;
		 
	}
}

void changeMode(int delta){ 
	mode += delta;
	if(mode < 0) mode += maxModes ;
		if(mode >maxModes) mode -= maxModes;
}

// *************************************
int readWaypointNumber(){ 
	static int encoder =0;
	static int maxEncoder =3;
	if(buttonPressed()){  // only read mode if button not pressed
		prevMode = mode;
		encoder += readEncoder() ;
		if(encoder >maxEncoder){
			encoder =0;
			wayPointToRecord ++;
		}
		if(encoder <0){
			encoder =maxEncoder;
			wayPointToRecord --;
		} 
	} 
	if(wayPointToRecord < 0) wayPointToRecord = numWaypoints;
	if(wayPointToRecord >numWaypoints) wayPointToRecord = 0;
	return(wayPointToRecord);
}
void saveData(){
	EEPROM.writeInt(waypointCountAddress, numWaypoints);
	EEPROM.writeBlock(originAddress, origin);
	EEPROM.writeBlock(waypointsAddress, waypoints,waypointsMax); 
	float calibrations[] = {MAGN_X_MIN, MAGN_X_MAX, MAGN_Y_MIN, MAGN_Y_MAX,WHEEL_FUDGE_FACTOR, percentPower};
	EEPROM.writeBlock(calibrationAddress, calibrations); 
 
}

void loadData(){ 
	numWaypoints = EEPROM.readInt(waypointCountAddress);
	EEPROM.readBlock(originAddress, origin);
	EEPROM.readBlock(waypointsAddress, waypoints,waypointsMax); 
	float calibrations[calibrationsMax ]    ;
	EEPROM.readBlock(calibrationAddress, calibrations,calibrationsMax); 
	MAGN_X_MIN = calibrations[0];
	MAGN_X_MAX = calibrations[1];
	MAGN_Y_MIN = calibrations[2];
	MAGN_Y_MAX = calibrations[3];
	WHEEL_FUDGE_FACTOR = calibrations[4]; 
	percentPower = calibrations[5];
	currentPosition.sigma = 10000000;  
	calibrate_mag();
	transmitWayPoints(&BLUETOOTH);
}

void washPoint(int waypointNumber){
	waypoints[waypointNumber].sigma =100000;
	// anything added to this point will 
	// remember very little of the previous readings
}

void restartWaypoints(){
	odoCount =0;
	for(int i = 0; i < numWaypoints; i++){
		waypoints[i].sigma = CLOSEENOUGH; 
	}
	waypointIterator =0;

		lcd2.begin();
		lcd2 << "restart";
}

// **********************************
float distanceToWaypoint(){
	double deltaN = waypoints[waypointIterator].y - currentPosition.y;
	double deltaE = waypoints[waypointIterator].x - currentPosition.x;   

	double distance2 = deltaN*deltaN + deltaE*deltaE;
	distance2 = sqrt(distance2);
	return distance2;
}


void addWaypoint(){
	if(numWaypoints == waypointsMax) return;

		wayPointToRecord =numWaypoints;
		setWaypoint();
		
		numWaypoints =(numWaypoints+1)  ;
		
		transmitWayPoints(&BLUETOOTH);
}

void clearWaypoints(){
	numWaypoints =0;
	wayPointToRecord=0;
	transmitWayPoints(&BLUETOOTH);
}

void integrateWaypoint(){ 
	waypoints[wayPointToRecord].sigma =10000; 

	Serial<< "recording waypoint\n";
	unsigned long start = millis();
	while (millis() -start  <5000){
		serialEvent2();
		 integrateGPS( waypoints[wayPointToRecord]); 
	} 
	transmitWayPoints(&BLUETOOTH);
}

void setWaypoint(){ 
	waypoints[wayPointToRecord].x = currentPosition.x;
	waypoints[wayPointToRecord].y = currentPosition.y;
	waypoints[wayPointToRecord].sigma = CLOSEENOUGH;
	transmitWayPoints(&BLUETOOTH);
}