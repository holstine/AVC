// testdefs.h

#ifndef _TESTDEFS_h
#define _TESTDEFS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif



typedef struct GPSCoords {
	long  lat;
	long  lon;
	float sigma;
} GPSCoordinates; 

typedef struct  Coordinates {
	double  x;
	double  y;
	float sigma;
} Coordinates;

#endif

