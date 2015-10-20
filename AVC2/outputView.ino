

long outmill=0;
void displayScreen(){
	if(!showLCD ) return;
	screen.clear();


	screen.selectLine(1);

	screen<<lcd1;

	screen.selectLine(2);

	screen<<lcd2; 
}

void resetTo16CharactersLCD(){
	screen.write(0x7C);
	screen.write(LCD_SET16CHAR);
	delay(5);
	//screen.specialCommand(LCD_SET16CHAR);

}
 

// ***********************************
void sendTelemetry(Stream * send){
	getTime();
	*send<< "autoPower:\t\t\t" <<  autoPower   <<endl;  
	*send<< "mills:\t\t\t" << millis()  <<endl; 
	*send<< "time:\t\t\t" << month [0]<<"-"<< day[0]<<"  "<< hour[0]<<"."<< minute[0]<<"."<< second[0]<<"."<< hundreths[0]  <<endl; 
	*send<< "mode:\t\t\t" << mode <<endl; 
	*send<< "gpsCourse:\t\t\t" << gpsCourse <<endl; 
	*send<< "gpsSpeed:\t\t\t" << gpsSpeed <<endl; 
	//*send<< "loopMills:\t\t" << loopMills <<endl; 
	//*send<< "driveMills:\t\t" << drivemill <<endl; 
	*send<< "power:\t\t\t"<< percentPower <<endl; 

	*send<< "GPS_UNCERTAINTY_FACTOR:\t" <<  GPS_UNCERTAINTY_FACTOR <<endl; 
	*send<< "DEAD_RECONING_UNCERTAINTY_FACTOR:\t" <<  DEAD_RECONING_UNCERTAINTY_FACTOR <<endl;  
	*send<< "COMPASS_UNCERTAINTY_FACTOR:\t" <<  COMPASS_UNCERTAINTY_FACTOR  <<endl; 
	*send<< "GYRO_UNCERTAINTY_FACTOR:\t" <<  GYRO_UNCERTAINTY_FACTOR  <<endl; 
	*send<< "COURSE_UNCERTAINTY_FACTOR:\t" <<  COURSE_UNCERTAINTY_FACTOR  <<endl; 
	*send<< "WHEEL_FUDGE_FACTOR:\t\t" <<  WHEEL_FUDGE_FACTOR  <<endl; 

	*send<<  "gyro:\t\t\t" <<(int)gyro[0]<<","<<(int)gyro[1]<<","<<(int)gyro[2]<<endl; 
	*send<<  "accel:\t\t\t" <<(int)accel[0]<<","<<(int)accel[1]<<","<<(int)accel[2]<<endl; 
	*send<<  "magno:\t\t\t" <<-(int)magnetom[0]<<","<<(int)magnetom[1]<<","<<(int)magnetom[2]<<endl; 

	*send<<  "magnoOffset: \t" <<manual_magn_offset[0]<<","<<manual_magn_offset[1]<<","<<manual_magn_offset[2]<<endl; 
	*send<<  "magno Scale: \t" <<manual_magn_scale[0]<<","<<manual_magn_scale[1]<<","<<manual_magn_scale[2]<<endl; 

	*send<< "yaw: \t\t\t" << yaw << "\n";
	*send<< "pitch: \t\t\t" << pitch << "\n";
	*send<< "roll: \t\t\t" << roll << "\n";
	*send<< "CENTER: \t\t" << CENTER << "\n";
	*send<< "compassHeading:\t\t"<< getCompassHeading() <<endl; 
	*send<< "heading:\t\t"<< getHeading() <<endl; 
	*send<< "compassRate:\t\t"<< getCompassRate() <<endl;  

	*send<< "button:\t\t\t" <<  buttonPressed()   <<endl;  

	*send<< "timeSinceLastGPS:\t" << timeSinceLastGPS <<endl; 
	*send<< "prox:\t\t\t" <<  readProx()  <<endl; 
	*send<< "odo:\t\t\t"<< odoCount  <<endl; 

	*send<< "headingToWaypoint:\t" <<  headingToWaypoint <<endl; 
	*send<< "distanceToWaypoint:\t" <<  distanceToWaypoint() <<endl;  
	*send<< "wayPointToRecord:\t" <<  wayPointToRecord  <<endl; 
	*send<< "numWaypoints:\t\t" <<  numWaypoints  <<endl; 
	*send<< "waypointIterator:\t" <<  waypointIterator <<endl;  
	*send<< "CLOSEENOUGH:\t" <<  CLOSEENOUGH <<endl;  
	*send<< "YAW_RATE_FUDGE_FACTOR:\t" <<  YAW_RATE_FUDGE_FACTOR <<endl;  
	*send<< "ROLL_PITCH_FUDGE:\t" <<  ROLL_PITCH_FUDGE <<endl;  
	*send<< "repeatWaypoints:\t" <<  repeatWaypoints <<endl;  
	*send<< "steeringSeverity:\t" <<  steeringSeverity <<endl;  
	*send<< "calibrateMag:\t" <<  calibrateMag <<endl;  
	*send<< "fileNumber:\t" <<  fileNumber <<endl;  

	transmitPosition(send, "currentPosition",currentPosition);
	transmitPosition(send, "rawGPS\t\t",relativeCoords);
	transmitPosition(send, "currentWaypoint",waypoints[waypointIterator]);


	transmitLCD(send);
	*send <<"\n\n"; 
}

void transmitPosition(Stream* stream, char* tag, Coordinates coords){ 
	*stream<<tag <<":\t"<<coords.x<<","<<coords.y<<","<<coords.sigma   <<endl; 

}



void transmitWayPoints(Stream * stream){ 

	*stream <<"waypoints"<<":\t\t\t" ;

	for(int i = 0; i < numWaypoints; i++){
		*stream<<waypoints[i].x<<","<<waypoints[i].y<<","<<waypoints[i].sigma;
		*stream<<"!";
	}
	*stream <<endl;

}
void transmitLCD(Stream* stream){ 
	*stream <<"lcd1" <<":\t\t\t"<<lcd1 <<endl ; 
	*stream <<"lcd2" <<":\t\t\t"<<lcd2 <<endl ; 
}

void serialSentinal(int mills){
	static unsigned long lastRefresh =0;
	unsigned long millsSinceRefresh = mills - lastRefresh; 

	// dont refresh screen and serial very often
	if (millsSinceRefresh >outputRefreshRate ||	mode != prevMode  ){ 
			if(stopThePlayback){
				updateOutput();
			}else{
				playBlock();
			}
			lastRefresh = mills;
	} 
}
void updateOutput(){  
	if(showLCD){
		displayScreen();  //about 40 milliseconds for each display or device
		drive();
	}
	if(transmitSerial){
		sendTelemetry(&Serial);
		drive();
	}
	if(transmitTelemetry){
		sendTelemetry(&BLUETOOTH); 
		drive();
	}
	if(transmitFile){
		sendTelemetry(&myFile); 
		myFile.flush();
		drive(); 
	}
}


