/// read a gps event
GPSCoordinates rawGPS;
Coordinates relativeCoords;
bool freshGPS = false;
long timeOfLastGPS=0;
long timeSinceLastGPS;
#define PMTK_SET_BAUD_57600r "$PMTK251,57600*2C\r\n"

void initGPS(){

	Serial2.begin(9600); // gps seems to be either 9600 or 57600
	GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
	GPS.sendCommand(PMTK_SET_NMEA_UPDATE_10HZ);
	
	for(int i = 0 ; i < 10; i++) {
			GPS.sendCommand(PMTK_SET_NMEA_UPDATE_10HZ);
		GPS.sendCommand(PMTK_SET_BAUD_57600r);
		delay(1000);
	}
  	GPS.begin(57600); 
	Serial2.begin(57600);
	for(int i = 0 ; i < 10; i++) {
			GPS.sendCommand(PMTK_SET_NMEA_UPDATE_10HZ); 
		delay(100);
	}

	
//	Serial2.begin(57600);
//	delay(1000);
	//GPS.sendCommand(PMTK_SET_BAUD_57600r);
	//Serial2 << PMTK_SET_NMEA_OUTPUT_RMCGGA;
	//Serial2 << PMTK_SET_NMEA_UPDATE_10HZ;
//	Serial2.begin(9600);
	
}
//long drivemill=0;
void serialEvent2() {

	drive();
	
	while (Serial2.available()) {
		
		char c = Serial2.read();
		// if(transmitSerial) Serial.write(c); // uncomment this line if you want to see the GPS data flowing
		if (gps.encode(c)){ // Did a new valid sentence come in?
			 rawGPS =	getAbsolutePositionFromGPS();
			 relativeCoords = absoluteToRelativeCoords(rawGPS);
			 timeSinceLastGPS = millis() -timeOfLastGPS;
		//	 if(transmitSerial) Serial <<"\n **fresh GPS "<< timeSinceLastGPS;
			 timeOfLastGPS = millis();
		
			// sendTelemetry();
			 freshGPS = true;
		}else{
			
		}
	}
	
//	Serial.write("|");
}
void integrateGPS(GPSCoordinates & coords){ 
	if(freshGPS == false) return;
	 coords = combineCoords(coords, rawGPS);
	 freshGPS = false;
}

// WARNING using combine coords with absolute coords reduces accuraccy
GPSCoordinates combineCoords(GPSCoordinates c1, GPSCoordinates c2){
//	 Serial << "combine gps c1: " << c1.lat << " " << c1.lon <<" " << c1.sigma << "\n";
//	 Serial << "combine gps c2: " << c2.lat << " " << c2.lon <<" " << c2.sigma << "\n";
	 double i1 = 1.0/(c1.sigma * c1.sigma);
	double i2 = 1.0/(c2.sigma * c2.sigma);
	double norm = i1+i2;
	
	GPSCoordinates c3;
	c3.lat = (i1/norm*c1.lat + i2/norm*c2.lat);
	c3.lon = (i1/norm*c1.lon + i2/norm*c2.lon);
	c3.sigma = 1.0/sqrt(norm);
//	Serial << "combine gps c3: " << c3.lat << " " << c3.lon <<" " << c3.sigma << "\n\n";
	return c3;
}


// ************* Relative coords ********


void integrateGPS(Coordinates & current ){
	if(freshGPS == false) return;
	// only use what is in a 1 km area around  origin (throw out really bad points)
	if(relativeCoords.x > 1000 || relativeCoords.y>1000) return;
	gpsCourse = getCourseFromGPS();
	gpsSpeed = getSpeedFromGPS();

	integrateGPSCourse(gpsCourse, gpsSpeed);
	 current = combineCoords(relativeCoords, current );
	// Serial.write("GPS integrating  ");
	// updateCoordinatesFromGPS(current); 
	 freshGPS = false; 
} 

GPSCoordinates  getAbsolutePositionFromGPS(){ 
	 GPSCoordinates gc;
	bool error = false; 
	unsigned long age;
	// Serial << "-";
	static long llat =0;
	static long llon = 0; 

	gps.get_position(&llat, &llon, &age); 
	
//	 Serial << "absCoords: " << llat << " " << llon <<" " <<age << "\n";
	

	if (llat == TinyGPS::GPS_INVALID_ANGLE ||
		llon == TinyGPS::GPS_INVALID_ANGLE) error = true;

	long lat =(llat == TinyGPS::GPS_INVALID_ANGLE ? 0.0 : llat);
    long lon =(llon == TinyGPS::GPS_INVALID_ANGLE ? 0.0 : llon );
    float hdop  = (gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
	hdop = 10 *GPS_UNCERTAINTY_FACTOR ;
	gc.lat = lat;
	gc.lon = lon;
	gc.sigma = hdop * HDOP_WEIGHT; 
	if (error == true) gc.sigma = 1000000;
	return gc;

}

float calcSineOfLat (long deciMicroDegrees){
			 sineOfLat = sin(origin.lat *3.1415926
				 / (1000000.0* 180.0)) ;
}

float getCourseFromGPS(){
	return gps.f_course();
}


float getSpeedFromGPS(){
	return gps.f_speed_mps();
}

Coordinates absoluteToRelativeCoords(GPSCoordinates c){
	// Serial << "a2r: " << c.lat << " " << c.lon <<" " << c.sigma << "\n";
	// Serial << "origin: " << origin.lat << " " << origin.lon <<" " << origin.sigma << "\n";
		 
	static float microDegreesToMeters = .111180;

	long deltaLat = c.lat - origin.lat;
	long deltaLon = c.lon - origin.lon;
//	  Serial << "delta: " <<deltaLat << " " << deltaLon <<" " <<  "\n";

	double f_deltaLon = deltaLon * microDegreesToMeters * sineOfLat;
	double f_deltaLat = deltaLat * microDegreesToMeters ;
 
	double hdop = gps.hdop();

	Coordinates coord ;
	coord.x = f_deltaLon;
	coord.y = f_deltaLat;
	coord.sigma = c.sigma;

	return coord;


}
 
Coordinates combineCoords(Coordinates c1, Coordinates c2){
	double i1 = 1.0/(c1.sigma * c1.sigma);
	double i2 = 1.0/(c2.sigma * c2.sigma);
	double norm = i1+i2;

	Coordinates c3;
	c3.x = (i1*c1.x + i2*c2.x)/norm;
	c3.y = (i1*c1.y + i2*c2.y)/norm;
	c3.sigma = 1.0/sqrt(norm);
	return c3;

}

void washOrigin(){
	odoCount =0;
	origin.sigma =10000000; 
}
void recordOrigin(){ 
	//integrateGPS( origin);
	calcSineOfLat(origin.lat);
	//transmitAbsoluteGPS("AbsoluteGPS", origin);
	currentPosition.sigma =100000000;
	currentPosition.x =0;
	currentPosition.y =0;
	// updateCoordinatesFromGPS(  currentPosition );
}

void resetOrigin(){ 
	washOrigin();

	Serial<< "reseting origin\n";
	unsigned long start = millis();
	while (millis() -start  <1000){
		serialEvent2();
		recordOrigin();
		origin = rawGPS;
	} 
}

long getTime(){
	unsigned long foo[1];
	unsigned long date[1];
	unsigned long age[1];  
	gps.get_datetime(date,foo,age);
	gps.crack_datetime(year,month,day,hour,minute,second,hundreths,age);
	return foo[0];
}