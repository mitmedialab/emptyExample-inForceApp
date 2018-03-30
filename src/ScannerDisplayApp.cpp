#include "ScannerDisplayApp.h"
#include <iostream>
#include <fstream>

ScannerDisplayApp::ScannerDisplayApp() {
	cout << "###### Application created: " << ScannerDisplayApp::getName() << endl;

	// Create pins
	for (int i = 0; i < INFORCE_COLS; i++)
		for (int j = 0; j < INFORCE_ROWS; j++)
			scannerPins[i][j] = new PinScanner(i, j, minHeight, maxHeight);

	// Initialize pin parameters
	string line;
	string filename = "foamScan.txt";
	ifstream myfile(filename);
	cout << "Displaying " << filename << endl;
	if (myfile.is_open())
	{
		for (int i = 0; i < INFORCE_COLS; i++) {
			for (int j = 0; j < INFORCE_ROWS; j++) {
				// <index = i*ROWS + j>
				getline(myfile, line);
				int index = std::stoi(line);
				int pinI = index / INFORCE_ROWS;
				int pinJ = index % INFORCE_ROWS;
				assert(pinI == i && pinJ == j);

				// <noObject>
				getline(myfile, line);
				scannerPins[pinI][pinJ]->noObject = std::stoi(line);

				// <lowerBoundary>
				getline(myfile, line);
				scannerPins[pinI][pinJ]->lowerBoundary = std::stoi(line);

				// <upperBoundary>
				getline(myfile, line);
				scannerPins[pinI][pinJ]->upperBoundary = std::stoi(line);

				// <pointer>
				getline(myfile, line);
				scannerPins[pinI][pinJ]->pointer = std::stoi(line);

				// <scannedHeights[p]>
				// <scannedForces[p]>
				for (int p = 0; p < scannerPins[pinI][pinJ]->pointer; p++) {
					getline(myfile, line);
					scannerPins[pinI][pinJ]->scannedHeights[p] = std::stoi(line);

					getline(myfile, line);
					scannerPins[pinI][pinJ]->scannedForces[p] = std::stoi(line);
				}
			}
		}
		myfile.close();
	}


	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			//maxHeightForShapeDisplay[i][j] = ofMap(ofDist(i,j,4.5,2), 0, 6, maxHeight, minHeight); //maxHeight
			maxHeightForShapeDisplay[i][j] = scannerPins[i][j]->invertHeight(scannerPins[i][j]->lowerBoundary);
			minHeightForShapeDisplay[i][j] = scannerPins[i][j]->invertHeight(scannerPins[i][j]->upperBoundary);

			//frequencyForShapeDisplay[i][j] = ofMap(i, 0, INFORCE_COLS - 1, 1, 30);
		}
	}

	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			heightsForShapeDisplay[i][j] = maxHeightForShapeDisplay[i][j];
			pinVelocity[i][j] = 0;
			lastNetForce[i][j] = 0;

			frequencyForShapeDisplay[i][j] = 0;
		}
	}

}


void ScannerDisplayApp::update(float dt) {
	time += dt;
	// give some time to calibrate
	if (time < 5) {
		if (time > 2) {
			for (int i = 0; i < INFORCE_COLS; i++) {
				for (int j = 0; j < INFORCE_ROWS; j++) {
					pins[i][j]->calibrate(dt, forceFromShapeDisplay[i][j]);
				}
			}
		}
		return;
	}


	// calculate force pushing down
	float forceFromFinger[INFORCE_COLS][INFORCE_ROWS];
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			forceFromFinger[i][j] = 0;
		}
	}
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			for (int ii = 0; ii < INFORCE_COLS; ii++) {
				for (int jj = 0; jj < INFORCE_ROWS; jj++) {
					float distanceSquared = (i - ii)*(i - ii) + (j - jj)*(j - jj);
					float scaleFactor = exp(-distanceSquared / variance); // sf = 0: 0, sf = 1: 100
					if (i == ii && j == jj)
						forceFromFinger[ii][jj] += max(0.0f, pins[i][j]->getForce() - 60);
					if (i == ii && j == jj)
						forceFromFinger[ii][jj] += scaleFactor * max(0.0f, pins[i][j]->getForce() - 60*scaleFactor);
				}
			}
		}
	}

	// calculate pin movement
	float newHeights[INFORCE_COLS][INFORCE_ROWS];
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {

			float forceFromSpring = scannerPins[i][j]->getTargetForce(scannerPins[i][j]->invertHeight(heightsFromShapeDisplay[i][j])) + 100; // DEFINE force pushing upwards   
			forceN[i][j] = forceFromSpring;
			float netForce = forceFromFinger[i][j] - forceFromSpring;

			// create deadband: if netForce is in (-30, 0), set equal to 0
			if (netForce < 0) 
				netForce = min(0.0f, netForce + 30.0f);

			// PID loop for velocity with some memory
			float newVelocity = 0.05*pinVelocity[i][j] - 0.95*(kp*netForce + kd*(netForce - lastNetForce[i][j]));

			// getting faster requires acceleration; getting slower does not
			if (abs(newVelocity) < pinVelocity[i][j])
				pinVelocity[i][j] = newVelocity;
			else
				pinVelocity[i][j] = 0.94 * pinVelocity[i][j] + 0.03 * newVelocity;

			lastNetForce[i][j] = netForce;

			
			if (pinVelocity[i][j] < 0)
				newHeights[i][j] = 0.5*(heightsFromShapeDisplay[i][j] - heightsForShapeDisplay[i][j]) + heightsForShapeDisplay[i][j] + ofMap(forceFromSpring, 200, 1500, 1.0, 0.5) * dt*pinVelocity[i][j];
			else 
				newHeights[i][j] = 0.5*(heightsFromShapeDisplay[i][j] - heightsForShapeDisplay[i][j]) + heightsForShapeDisplay[i][j] + dt*speedGoingUp*pinVelocity[i][j];
			//1.5*
		}
	}

	// calculate indentations
	float indentations[INFORCE_COLS][INFORCE_ROWS];
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			indentations[i][j] = maxHeightForShapeDisplay[i][j];
		}
	}
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			for (int ii = 0; ii < INFORCE_COLS; ii++) {
				for (int jj = 0; jj < INFORCE_ROWS; jj++) {
					float distanceSquared = (i - ii)*(i - ii) + (j - jj)*(j - jj);
					float height = exp(-distanceSquared / variance) * (maxHeightForShapeDisplay[i][j] - newHeights[i][j]);
					if (indentations[ii][jj] > maxHeightForShapeDisplay[ii][jj] - height)
						indentations[ii][jj] = maxHeightForShapeDisplay[ii][jj] - height;
				}
			}
		}
	}
	for (int iter = 0; iter < 3; iter++) {
		for (int i = 0; i < INFORCE_COLS; i++) {
			for (int j = 0; j < INFORCE_ROWS; j++) {
				for (int ii = 0; ii < INFORCE_COLS; ii++) {
					for (int jj = 0; jj < INFORCE_ROWS; jj++) {
						float distanceSquared = (i - ii)*(i - ii) + (j - jj)*(j - jj);
						float height = exp(-distanceSquared / variance) * (maxHeightForShapeDisplay[i][j] - indentations[i][j]);
						if (indentations[ii][jj] > maxHeightForShapeDisplay[ii][jj] - height)
							indentations[ii][jj] = maxHeightForShapeDisplay[ii][jj] - height;
					}
				}
			}
		}
	}
	// set new pin heights
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			heightsForShapeDisplay[i][j] = indentations[i][j]; //DEFINE NEW PIN HIGHT
		}
	}

	// boundaries
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			if (heightsForShapeDisplay[i][j] < minHeightForShapeDisplay[i][j]) {
				heightsForShapeDisplay[i][j] = minHeightForShapeDisplay[i][j]; //lower boundary
				pinVelocity[i][j] = 0;
			}
			if (heightsForShapeDisplay[i][j] > maxHeightForShapeDisplay[i][j]) {
				heightsForShapeDisplay[i][j] = maxHeightForShapeDisplay[i][j]; // higher boundary (shape)
				pinVelocity[i][j] = 0;
			}
		}
	}

}
float ScannerDisplayApp::getTargetForce(int height) {
	return ofMap(height, maxHeight, (maxHeight+minHeight)/2, 130, 1500);
}

float ScannerDisplayApp::getTargetForce2(int height) {
	return 200;
}

float ScannerDisplayApp::getTargetForce3(int height) {
	if (height > 65000)
		return 200;
	else if (height > 50000)
		return 700;
	else if (height > 35000)
		return 200;
	else if (height > 20000)
		return 700;
	else
		return 200;
}

float ScannerDisplayApp::getTargetForce4(int height) {
	if (height > 65000)
		return 200;
	else
		return 900;
}

float ScannerDisplayApp::getTargetForce5(int height, int x, int y) {
	if ((x == 3 && y == 3) || (x == 7 && y == 2)) {
		return 900;
	} 
	else {
		return 200;
	}
}



float ScannerDisplayApp::getTargetFrequency(int height) {
	float fq = 5;
	if (height < 40000) {
		fq = 1;
	} else if (height < 78000) {
		fq = 20;
	}


	return fq;

}


string ScannerDisplayApp::appInstructionsText() {
	string instructions = (string) "" +
		"This is for testing things."
		"";

	return instructions;
}

void ScannerDisplayApp::keyPressed(int key) {
	cout << "keyPressed" << endl;

	if (key == 'w') {
		variance *= 1.3;
		cout << "Variance: " << variance << endl;
	}
	else if (key == 's') {
		variance /= 1.3;
		cout << "Variance: " << variance << endl;
	}
	else if (key == 'r') {
		variance = 4;
		for (int i = 0; i < INFORCE_COLS; i++) {
			for (int j = 0; j < INFORCE_ROWS; j++) {
				heightsForShapeDisplay[i][j] = maxHeight;
				pinVelocity[i][j] = 0;
				lastNetForce[i][j] = 0;
				frequencyForShapeDisplay[i][j] = 0;
				time = 0;
			}
		}
//		for (int i = -1; i < INFORCE_COLS + 1; i++) {
//			for (int j = -1; j < INFORCE_ROWS + 1; j++) {
//				heightsForShapeDisplayBuffered[i + 1][j + 1] = maxHeight;
//				heightsFromShapeDisplayBuffered[i + 1][j + 1] = maxHeight;
//			}
//		}
	}
}

