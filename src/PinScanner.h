#pragma once
#include "Config.h"
#include <map>


class PinScanner {
public:
	PinScanner(int ii, int jj, int _minHeight, int _maxHeight);


	void update(float dt, float heightFromShapeDisplay, float force);
	float getHeightForShapeDisplay();
	

	// Pin[i][j]
	int pinI, pinJ;


	float time = 0.0;
	float pauseTime = 0.0;
	bool verbose = false;

	int height;

	void scanUpperBoundary();
	void scanLowerBoundary();
	void scanStiffness(int lowerBound);


	bool scanningUpperBoundary = false;
	bool scanningLowerBoundary = false;
	float upperBoundary = -1; // ####
	float lowerBoundary = -1; // ####
	bool noObject = false; // ####

	bool scanningStiffness = false;

	int increment = 250;

	int samplingNumAroundAnIncrement = 10;
	bool sampling = false;
	int samplingValForces[10];
	int samplingValHeights[10];
	int currentSamplingNum = 0;
	int currentIncrementHeight = maxHeight;

	int pointer = 0; // #### stores number of data points in scannedHeights and scannedForces
	int scannedHeights[1000]; // ####  at scannedHeights[i] it felt scannedForces[i]
	int scannedForces[1000]; // ####

	bool doneScanning = false;

	float invertHeight(int height);
	float getTargetForce(int height);
	void reset();



private:

	int upOrDownThreshold = 15;

	int minHeight;
	int maxHeight;

};