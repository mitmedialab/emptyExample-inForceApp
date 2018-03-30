#include "PinScanner.h"

PinScanner::PinScanner( int ii, int jj, int _minHeight, int _maxHeight) {
	pinI = ii;
	pinJ = jj;
	minHeight = _minHeight;
	maxHeight = _maxHeight;
	height = _minHeight;
}



void PinScanner::update(float dt, float heightFromShapeDisplay, float force) {
	if (pauseTime > 0.0) {
		pauseTime -= dt;
		return;
	}

	time += dt;

	if (scanningUpperBoundary) {
		if (verbose) cout << "Pin[" << pinI << "][" << pinJ << "] force, height, heightFromShapeDisplay: " << force << ", " << height << ", " << heightFromShapeDisplay << endl;
		if (force > 600 || abs(height - heightFromShapeDisplay) > 10000 || height > maxHeight) {
			if (height > maxHeight) {
				noObject = true;
				lowerBoundary = maxHeight;
				upperBoundary = maxHeight;
				height = maxHeight;
				doneScanning = true;
				if (verbose) cout << "Pin[" << pinI << "][" << pinJ << "] No object. Done scanning." << endl;
			}
			else {
				scanningLowerBoundary = true;
				pauseTime = 0.2f;
				if (verbose) cout << "Pin[" << pinI << "][" << pinJ << "] upperBound: " << heightFromShapeDisplay << ". Begin scanning lower boundary..." << endl;
			}
			scanningUpperBoundary = false;
		}
		else {
			height += 150.0f;
		}
	}
	else if (scanningLowerBoundary && !noObject) {
		if (verbose) cout << "Pin[" << pinI << "][" << pinJ << "] force, height, heightFromShapeDisplay: " << force << ", " << height << ", " << heightFromShapeDisplay << endl;
		if (force < 20) {
			scanningLowerBoundary = false;
			lowerBoundary = heightFromShapeDisplay;
			if (verbose) cout << "Pin[" << pinI << "][" << pinJ << "] lowerBound: " << lowerBoundary << endl;
			
			//doneScanning = true;
			scanStiffness(lowerBoundary);
		}
		else {
			height -= 400.0f;
			pauseTime = 0.5f;
		}
	}

	else if (scanningStiffness && !noObject) {

		if (force > 1200 || height - heightFromShapeDisplay > 0.9*increment || height > maxHeight) {
			upperBoundary = heightFromShapeDisplay;
			scanningStiffness = false;
			doneScanning = true; 
			height = lowerBoundary;
			if (verbose) cout << "Pin[" << pinI << "][" << pinJ << "] upperBound: " << upperBoundary << endl;
		}
		else {
			if (verbose) cout << "Pin[" << pinI << "][" << pinJ << "] height, force: " << heightFromShapeDisplay << " " << force << endl;
			scannedHeights[pointer] = heightFromShapeDisplay;
			scannedForces[pointer] = force;
			pointer++;
			height += increment;
			pauseTime = 1.0f;
			/*
			if(currentSamplingNum == samplingNumAroundAnIncrement){ //samplingNumAroundAnIncrement = 10, so this means if 10 sampling data is collected
				float sumH = 0, sumF = 0;
				for (int i = 0; i < samplingNumAroundAnIncrement; i++) {
					sumH += samplingValHeights[i];
					sumF += samplingValForces[i];

				}

				scannedHeights[pointer] = sumH/samplingNumAroundAnIncrement;
				scannedForces[pointer] = sumF/samplingNumAroundAnIncrement;

				if (verbose) cout << "AVERAGE VAL IS GENERATED FROM SAMPLING : Pin[" << pinI << "][" << pinJ << "] height, force: " << scannedHeights[pointer] << " " << scannedForces[pointer] << endl;


				currentSamplingNum = 0;
				pointer++;
				sampling = false;
				currentIncrementHeight += increment;
				height = currentIncrementHeight;
			}
			else {
				if (sampling) {
					height = currentIncrementHeight;
					sampling = false;
				}
				else { 
					samplingValHeights[currentSamplingNum] = heightFromShapeDisplay;
					samplingValForces[currentSamplingNum] = force;
					currentSamplingNum++;

					//this line is to move away from the currentIncrementHeight in each sampling
					height = currentIncrementHeight + currentSamplingNum * (1200/samplingNumAroundAnIncrement) - 600;
					sampling = true;
				}

			}
			pauseTime = 0.3f;

			*/
		}
	}
}


void PinScanner::scanUpperBoundary() {
	doneScanning = false;
	scanningUpperBoundary = true;
	height = minHeight;
	pauseTime += 1.0f;
}

void PinScanner::scanLowerBoundary() {
	doneScanning = false;
	scanningLowerBoundary = true;
	pauseTime = 0.2f;
	if (verbose) cout << "Pin[" << pinI << "][" << pinJ << "] Begin scanning lower boundary..." << endl;
}

// Has to be done after scanLowerBoundary() has completed
void PinScanner::scanStiffness(int _lowerBound) {
	if (!noObject) {
		doneScanning = false;
		scanningStiffness = true;
		height = _lowerBound;
		currentIncrementHeight = _lowerBound;
		pointer = 0;
		pauseTime += 1.0f;
	}
}


float PinScanner::invertHeight(int height) {
	return minHeight + maxHeight - height;
}

float PinScanner::getTargetForce(int height) {
	if (noObject)
		return 0.0f;
	if (height <= lowerBoundary)
		return 0.0f;
	if (height >= upperBoundary)
		return 1200;


	for (int i = 0; i < pointer; i++) {
		if (scannedHeights[i] >= height)
			return scannedForces[i];
	}
	cout << "Something went wrong" << endl;
	return 0.0f;;
}


float PinScanner::getHeightForShapeDisplay() {
	return height;
}

void PinScanner::reset() {
	time = 0.0;
	pauseTime = 0.0;

	scanningUpperBoundary = false;
	scanningLowerBoundary = false;
	upperBoundary = -1; // ####
	lowerBoundary = -1; // ####
	noObject = false; // ####
	
	scanningStiffness = false;
	pointer = 0; // #### stores number of data points in scannedHeights and scannedForces
	
	doneScanning = false;
}

