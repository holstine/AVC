




#define FULL_SPEED_AHEAD 180
#define FULL_SPEED_REVERSE 0
#define FULL_STOP 90

#define ACCEL_ADDRESS ((int) 0x53) // 0x53 = 0xA6 / 2
#define MAGN_ADDRESS ((int) 0x1E) // 0x1E = 0x3C / 2
#define GYRO_ADDRESS ((int) 0x68) // 0x68 = 0xD0 / 2
#define WIRE_SEND(b) Wire.write((byte) b) 
#define WIRE_RECEIVE() Wire.read() 
 


void setPins(){

	
	pinMode(ledPin, OUTPUT);      // Set the LED pin as output
	 pinMode(COMTX, OUTPUT);
	
	//     Timer1.initialize(5000);         // initialize timer1, and set a 1/2 second period 
	//    Timer1.attachInterrupt(updatePosition);  // attaches callback() as a timer overflow interrupt
	pinMode(FORWARD_PIN, OUTPUT); 

	pinMode(BACKWARD_PIN, OUTPUT);
	pinMode(STEERING_PIN, OUTPUT);

	pinMode(ODOMETER_PIN, INPUT);
	digitalWrite(ODOMETER_PIN, HIGH);

	pinMode(PROXIMITY_PIN, INPUT);

	pinMode(MODEREAD1, INPUT);
	pinMode(MODEREAD2, INPUT);
	digitalWrite(MODEREAD1, HIGH);
	digitalWrite(MODEREAD2, HIGH);
	

	pinMode(KNOBBUTTONPIN, INPUT);
	digitalWrite(KNOBBUTTONPIN, HIGH);

	pinMode(GOPIN, INPUT);
	digitalWrite(GOPIN, HIGH);

	pinMode(FILE_PIN, OUTPUT);
}
void armCompass(){
	Wire.begin();

	// Set operating mode to continuous
	Wire.beginTransmission(Addr); 
	Wire.write(0x02);
	Wire.write(0x00);
	Wire.endTransmission();


}
void armMagnetometer(){
	Wire.begin();

	// Set operating mode to continuous
	Wire.beginTransmission(Addr); 
	Wire.write(0x02);
	Wire.write(0x00);
	Wire.endTransmission();
	 delay(5);

   
}

 

void armSteering(){
	myservo.attach(STEERING_PIN);  
	turn(0);

}

int readEncoder()
{
  //noInterrupts();  // don't want our interrupt to be interrupted
  // First, we'll do some software debouncing. Optimally there'd
  // be some form of hardware debounce (RC filter). If there is
  // feel free to get rid of the delay. If your encoder is acting
  // 'wacky' try increasing or decreasing the value of this delay.
//  delayMicroseconds(5000);  // 'debounce'
 
  // enc_states[] is a fancy way to keep track of which direction
  // the encoder is turning. 2-bits of oldEncoderState are paired
  // with 2-bits of newEncoderState to create 16 possible values.
  // Each of the 16 values will produce either a CW turn (1),
  // CCW turn (-1) or no movement (0).
  int8_t enc_states[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
  static uint8_t oldEncoderState = 0;
  static uint8_t newEncoderState = 0;

  // First, find the newEncoderState. This'll be a 2-bit value
  // the msb is the state of the B pin. The lsb is the state
  // of the A pin on the encoder.
  newEncoderState = (digitalRead(MODEREAD1)<<1) | (digitalRead(MODEREAD2));
 
  // Now we pair oldEncoderState with new encoder state
  // First we need to shift oldEncoder state left two bits.
  // This'll put the last state in bits 2 and 3.
  oldEncoderState <<= 2;
  // Mask out everything in oldEncoderState except for the previous state
  oldEncoderState &= 0xC0;
  // Now add the newEncoderState. oldEncoderState will now be of
  // the form: 0b0000(old B)(old A)(new B)(new A)
  oldEncoderState |= newEncoderState; // add filteredport value

  // Now we can update encoderPosition with the updated position
  // movement. We'll either add 1, -1 or 0 here.
  int encoderPosition = enc_states[oldEncoderState];
 
   
 
 // interrupts();  // re-enable interrupts before we leave
  encoderPosition += overrideEncoderValue;
	overrideEncoderValue =0;
  return encoderPosition;
}
int readEncoder2(){  
	//int encode = 0;
	//static int lastEncodedA =0;
	//static int lastEncodedB =0;
	//static int encodedA = LOW;

	//encodedA = digitalRead(MODEREAD1);
 //  if ((lastEncodedA == LOW) && (encodedA == HIGH)) {
 //    if (digitalRead(MODEREAD2) == LOW) {
 //      encode--;
 //    } else {
 //      encode++;
 //    } 
 //  } 
 //  lastEncodedA = encodedA;
//		return encode;
   
   //**************************************
	int encoderValue = 0;
	static int lastEncoded =0;
	int MSB = digitalRead(MODEREAD1); //MSB = most significant bit
  int LSB = digitalRead(MODEREAD2); //LSB = least significant bit

  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue =1;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue =-1;

//if (encoderValue !=0) Serial << encoderValue;

	//overrideEncoder allows input via telemetry
	
     lastEncoded = encoded; //store this value for next time
  return encoderValue;

   //**************************************

	//static int8_t enc_states[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
	//static uint8_t old_AB = 0;
	// 
	//old_AB <<= 2;                   //remember previous state
	//old_AB |= ( ENC_PORT & 0x03 );  //add current state
	//int encode =  ( enc_states[( old_AB & 0x0f )]);
	// if(encode != 0) Serial << encode;
	//return encode;
}
// ***********************************


bool buttonPressed(){
	if(buttonOverride == true) return true;
	return !digitalRead( KNOBBUTTONPIN);
}

// ***********************************


bool goButtonPressed(){ 
	return !digitalRead( GOPIN);
}

// ***********************************
void flash(){
	// Flash the LED on pin 13 just to show that something is happening
	// Also serves as an indication that we're not "stuck" waiting for TWI data
	ledState = !ledState;
	if (ledState) {
		digitalWrite(ledPin,HIGH);
	}  else  {
		digitalWrite(ledPin,LOW);
	}
}

// ***********************************
void turn(int deg){
	int turnTo = deg + CENTER;
	if (turnTo > MAX_TURN) turnTo = MAX_TURN;
	if (turnTo < MIN_TURN) turnTo = MIN_TURN;
	if(steeringEnable) myservo.write(turnTo);
}


void odoInterupt(){
	tempOdoCount++;
}

// ***********************************
int readOdo(){

	int val=  digitalRead(ODOMETER_PIN);
	return val;
}


// ***********************************
int readProx(){
	int val=  analogRead(PROXIMITY_PIN);
	//prox = (6.0*prox/10.0) + (4.0*val/10.0);
	return val;
}

 
 
 
// ***********************************
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
		float val = 180.0 / 3.1415926 *atan2(-y,x);
		return val;
	}
	return 0.0;
}
// ***********************************
void checkOdometer(){
	static	long start = millis();

	long oc = tempOdoCount; 
	tempOdoCount =0;

	odoCount += oc; 
	updateCurrentPositionFromOdo(oc);
	long temp = millis()-start;
	if(drivemill < temp) drivemill = temp;
	drivemill = drivemill-1;
	start = millis(); 
}


// ***********************************
void go(){
	//digitalWrite(FORWARD_PIN, HIGH); 
	//	myEscServo.writeMicroseconds(1900);
	if( autoPower){
		myEscServo.write(power( FULL_SPEED_AHEAD, percentPower));
	}else{
		myEscServo.write(power( FULL_SPEED_AHEAD, 0));
	}
	
	DIRECTION = 1;
}

// ***********************************
void backup (){
	//myEscServo.writeMicroseconds(900);
	myEscServo.write(power( FULL_SPEED_REVERSE, percentPower));
	if(DIRECTION ==1){
		// this model has a braking mode.
		// full reverse brakes the car. 
		// go to neutral then go to reverse
		delay(1000);
		stops();
		delay(1000);
		myEscServo.write(power( FULL_SPEED_REVERSE, percentPower));

	} 
	DIRECTION = -1;
}

// ***********************************
void stops(){ 
	myEscServo.write(power( FULL_SPEED_AHEAD, 0));
	
}

// ***********************************
void armMotorControler(){ 
	myEscServo.attach(FORWARD_PIN,900,1900);
	// set the motor controler with initialization 
	// full forward then full back.
//	myEscServo.write(FULL_SPEED_AHEAD);
	//delay(4000);
//	myEscServo.write(FULL_SPEED_REVERSE);
	//delay(4000);
//	myEscServo.write(FULL_STOP);
	myEscServo.write(power( FULL_SPEED_AHEAD, 0));
	setPower(0);
	
	/*myEscServo.writeMicroseconds(900);
	delay(4000);
	myEscServo.writeMicroseconds(1900);
	delay(4000);
	myEscServo.writeMicroseconds(1500);*/

}

int power( int original, int percent){
	int finalPower = percent * original + (100-percent)*FULL_STOP;
	finalPower /=100;
	return finalPower;
}

int setPower(int percent){
	percentPower = percent;
	if(percentPower < 0) percentPower = 0;
	if(percentPower >100) percentPower = 100;
}
