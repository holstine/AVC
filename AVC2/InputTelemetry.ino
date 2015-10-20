


// ****************** Telemetry **********************

String inputString = "";  
boolean stringComplete = false;  // whether the string is complete
void readTelemetry(String s){
	int value =0;;
	if( s.indexOf(":")>0 ) value = parseInt(s);

	Serial<< "Telemetry *************************\n"<< s << endl;

	if( s.startsWith("modePlus")) changeMode(1);
	if( s.startsWith("modeMinus")) changeMode(-1);

	if( s.startsWith("buttonOn")) buttonOverride = true;
	if( s.startsWith("buttonOff")) buttonOverride = false;

	if( s.startsWith("valuePlus")) overrideEncoderValue = 1;
	if( s.startsWith("valueMinus")) overrideEncoderValue = -1;

	if( s.startsWith("telemetryOn")) transmitTelemetry = true;
	if( s.startsWith("telemetryOff")) transmitTelemetry = false;

	if( s.startsWith("lcdOn")) showLCD = true;
	if( s.startsWith("lcdOff")) showLCD = false;

	if( s.startsWith("serialOn")) transmitSerial = true;
	if( s.startsWith("serialOff")) transmitSerial = false;

	if( s.startsWith("fileOn")) transmitFile = true;
	if( s.startsWith("fileOff")) transmitFile = false;

	if( s.startsWith("resetOrigin")) resetOrigin();
	if( s.startsWith("addWaypoint")) addWaypoint();
	if( s.startsWith("clearWaypoints")) clearWaypoints();
	
	if( s.startsWith("save")) saveData();
	if( s.startsWith("load")) loadData();

	if( s.startsWith("steeringOn")) steeringEnable = true;
	if( s.startsWith("steeringOff")) steeringEnable = false;

	if( s.startsWith("powerOn")) autoPower = true;
	if( s.startsWith("powerOff")) autoPower = false;

	if( s.startsWith("sendWaypoints")) transmitWayPoints(&BLUETOOTH);
	if( s.startsWith("resetWaypoints")) restartWaypoints();
	 
	if( s.startsWith("speedyUpdateOn")) outputRefreshRate = 100;
	if( s.startsWith("speedyUpdateOff")) outputRefreshRate = 1000; 

	if( s.startsWith("integrateWaypointOn")) integrateWaypoint();
	if( s.startsWith("integrateWaypointOff")) integrateWaypoint(); 
	if( s.startsWith("setWaypoint")) setWaypoint(); 

	if( s.startsWith("backWaypoint")) {wayPointToRecord --; if (wayPointToRecord <0) wayPointToRecord = numWaypoints;}
	if( s.startsWith("forwardWaypoint")) {wayPointToRecord ++; if (wayPointToRecord>numWaypoints) wayPointToRecord = 0;}
	
	if( s.startsWith("calibrateCompassOn")){ resetCalibration(); calibrateMag = 1;}
	if( s.startsWith("calibrateCompassOff")) calibrateMag = 0; 

	if( s.startsWith("percentPower")) setPower(value); 
	if( s.startsWith("compMode")) competitionMode(); 
	if( s.startsWith("goMode")) autoPower = true; 
	if( s.startsWith("stop"))  autoPower = false;  


	if( s.startsWith("yOffsetCalibrationBar")) manual_magn_offset[1] = scaleValue(value, -100, 100); 
	if( s.startsWith("xOffsetCalibrationBar")) manual_magn_offset[0] = scaleValue(value, -100, 100); 
	if( s.startsWith("yScaleCalibrationBar")) manual_magn_scale[1] = scaleValue(value, .9, 1.1); 
	if( s.startsWith("xScaleCalibrationBar")) manual_magn_scale[0] = scaleValue(value, .9, 1.1); 
	
	if( s.startsWith("steeringBar")) CENTER = scaleValue(value, 80, 120); 
	if( s.startsWith("CENTER")) CENTER = scaleValue(value, 80, 120);
	if( s.startsWith("GPS_UNCERTAINTY_FACTOR")) GPS_UNCERTAINTY_FACTOR = scaleValue(value, .1, 20);
	if( s.startsWith("DEAD_RECONING_UNCERTAINTY_FACTOR")) DEAD_RECONING_UNCERTAINTY_FACTOR = scaleValue(value, .1,20);
	if( s.startsWith("COMPASS_UNCERTAINTY_FACTOR")) COMPASS_UNCERTAINTY_FACTOR = scaleValue(value,.1, 20);
	if( s.startsWith("GYRO_UNCERTAINTY_FACTOR")) GYRO_UNCERTAINTY_FACTOR = scaleValue(value, .1, 20);
	if( s.startsWith("COURSE_UNCERTAINTY_FACTOR")) COURSE_UNCERTAINTY_FACTOR = scaleValue(value, .1, 20);
	if( s.startsWith("WHEEL_FUDGE_FACTOR")) WHEEL_FUDGE_FACTOR = scaleValue(value, .5, 2); 
	if( s.startsWith("steeringSeverity")) steeringSeverity = scaleValue(value, .5, 2); 
	
	if( s.startsWith("CLOSEENOUGH")) CLOSEENOUGH = scaleValue(value, 0, 2); 
	if( s.startsWith("YAW_RATE_FUDGE_FACTOR")) YAW_RATE_FUDGE_FACTOR = scaleValue(value, .9, 1.1); 
	if( s.startsWith("repeatWaypoints")) repeatWaypoints = scaleValue(value, 0, 1.9); 
	if( s.startsWith("calibrateMag")) calibrateMag = scaleValue(value, 0, 1.9); 
	if( s.startsWith("ROLL_PITCH_FUDGE")) ROLL_PITCH_FUDGE = scaleValue(value, -1, 2); 
	

	if( s.startsWith("playbackPlay")) playback() ; 
	if( s.startsWith("playbackStop")) stopPlayback() ; 
	if( s.startsWith("playbackNumberMinus")) fileNumber-- ; 
	if( s.startsWith("playbackNumberPlus")) fileNumber++ ; 


	if( s.startsWith("playbackSpeedMinus")) slowDown() ; 
	if( s.startsWith("playbackSpeedPlus")) speedUp() ; 	//updateOutput(); dont do this,  it causes a loop that is bad

}




int  parseInt(String s){
	 
	int start=  s.indexOf(":")+1;
	
	String numString =s.substring(start,s.length());
	int i =numString.toInt();
	
	//Serial << "parsing"<< s << "is a "<< numString << "  "  << i<< endl;
	return i;
}


float  scaleValue (int value, float min, float max){ 
	float fraction = value/100.0; // value is assumed to be 0 to 100
	return fraction * max  + (1-fraction) * min ;

}

// On a Mega This gets kicked off by hardware
void serialEvent3() {
  while (BLUETOOTH.available()) {
	
    // get the new byte:
    char inChar = (char)BLUETOOTH.read();
    // add it to the inputString:
   
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
	  readTelemetry(inputString);
	  inputString = "";
    }else{
		inputString += inChar;
	}
  }
}

 
 
 