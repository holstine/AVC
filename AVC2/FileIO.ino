

File playbackFile ;
int pauseMills= 1000;
bool stopThePlayback = true;
void setupFileIO(){ 
	Serial.print("Initializing SD card...\n");
	// On the Ethernet Shield, CS is pin 4. It's set as an output by default.
	// Note that even if it's not used as the CS pin, the hardware SS pin
	// (10 on most Arduino boards, 53 on the Mega) must be left as an output
	// or the SD library functions will not work.

	if (!SD.begin(FILE_PIN)) {
		Serial.println("initialization failed!");
		transmitFile = false;
		return;
	}
	Serial.println("initialization done.");
	// open the file. note that only one file can be open at a time,
	// so you have to close this one before opening another.
	int i =0;

	char buffer1[16];
	PString fileName(buffer1, sizeof(buffer1));

	while(true){
		fileName.begin();
		i++;
		fileName <<"avc" << i<<  ".txt" <<'\0';

		if (! SD.exists(buffer1  )){
			myFile = SD.open(buffer1, FILE_WRITE);
			fileNumber = i;
			break;
		}
	} 
	// if the file opened okay, write to it:
	if (myFile) { 

		// close the file:
		//myFile.close();
		Serial<< "opening File: " << fileName<< endl;
	} else {
		// if the file didn't open, print an error:
		Serial.println("error opening test.txt");

	}
} 

void rewind(int counts){

}

void openFileNumber(int fileNum){ 
	char buffer1[16];
	PString fileName(buffer1, sizeof(buffer1));
	fileName.begin(); 
	fileName <<"avc" << fileNum<<  ".txt" <<'\0';

	if (  SD.exists(buffer1  )){
		Serial << "Playing back file" << fileName<< endl;
		playbackFile = SD.open(buffer1, FILE_READ); 
	} 
}

void slowDown(){
	outputRefreshRate++;
	outputRefreshRate *=2; 
}

void speedUp(){
	outputRefreshRate++;
	outputRefreshRate /=2;
	
}

void playback(){ 


	stopThePlayback = false;
	openFileNumber(fileNumber); 

	if (!playbackFile) return;
	

	//playbackFile.close();


}

void playBlock(){
	char prevChar = 'x'; 
	while (playbackFile.available()) {
		
		//	delay(pauseMills);
		//	
		char inChar = (char)playbackFile.read();
		
		

		BLUETOOTH.write(inChar);
		
		Serial.write(inChar); 
		if( inChar =='\n' && prevChar == '\n'){
			return;
		}

		prevChar = inChar;
	}

	stopThePlayback = true;
}



void stopPlayback(){
	stopThePlayback = true;
}