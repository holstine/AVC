


// basic averaging
void integrateCompass(double reading, double readingSigma){

	// if reading is 359 and heading is 001 we don't want to pass through 180
	if(reading < 0) reading += 360; 
	double delta = heading -reading;
	if(delta < -180 ) reading -= 360;
	if(delta >  180 ) reading += 360;

	double i1 = 1.0/(headingSigma * headingSigma);
	double i2 = 1.0/(readingSigma * readingSigma);
	double norm = i1+i2;
	heading = (i1*heading + i2*reading)/norm;
	if (heading > 360) heading -= 360;
	if (heading < 0) heading += 360;
	headingSigma = 1.0/sqrt(norm);
	headingSigma +=.005;  // this makes a running average
}

void integrateGyro(double yawRate, double sigmaRate){
	static long millsSinceLastCheck;
	long delta = millis() - millsSinceLastCheck;
	headingSigma += .005* delta * abs(yawRate)* sigmaRate/1000.0;
	heading += (delta*yawRate*YAW_RATE_FUDGE_FACTOR)/1000.0;
//	if(heading > 360) heading -= 360;
//	if(heading < 0) heading += 360;
	millsSinceLastCheck = millis();
}

void integrateSteering(double steering, double sigma){


}

void integrateGPSCourse(double heading, double speed){
	static double prevHeading =0;
	double sigma = COURSE_UNCERTAINTY_FACTOR/abs( speed +.01 );
	double variance = (heading -prevHeading)*(heading -prevHeading);
	sigma += variance;
	prevHeading = heading;
	integrateCompass( heading, sigma );
}

float getHeading(){ 
	return heading;
}

void findHeading(){
	integrateCompass(getCompassHeading(), 5*COMPASS_UNCERTAINTY_FACTOR);
	integrateGyro(getCompassRate(), .5*GYRO_UNCERTAINTY_FACTOR);
}
