#include "MirrorApp.h"
#include <iostream>
#include <fstream>

MirrorApp::MirrorApp() {
	cout << "###### Application created: " << MirrorApp::getName() << endl;

	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			//maxHeightForShapeDisplay[i][j] = ofMap(ofDist(i,j,4.5,2), 0, 6, maxHeight, minHeight); //maxHeight
			maxHeightForShapeDisplay[i][j] = maxHeight;
			minHeightForShapeDisplay[i][j] = minHeight;
		}
	}


	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			heightsForShapeDisplay[i][j] = midHeight;
			pinVelocity[i][j] = 0;
			lastNetForce[i][j] = 0;
			frequencyForShapeDisplay[i][j] = 0;
		}
	}

	for (int i = 4; i <= 5; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			//maxHeightForShapeDisplay[i][j] = ofMap(ofDist(i,j,4.5,2), 0, 6, maxHeight, minHeight); //maxHeight
			maxHeightForShapeDisplay[i][j] = minHeight;
			minHeightForShapeDisplay[i][j] = minHeight;
			heightsForShapeDisplay[i][j] = minHeight;
		}
	}

}

void MirrorApp::update(float dt) {
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
	const static int INFORCE_COLS_HALF = 4;


	// calculate pin movement
	float newHeights[INFORCE_COLS_HALF][INFORCE_ROWS];
	for (int i = 0; i < INFORCE_COLS_HALF; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {

			float forceFromFinger = leftStrength*max(0.0f, pins[i][j]->getForce() - 30.0f);
			float forceFromMirrorFinger = max(0.0f, pins[6+i][j]->getForce() - 30.0f);

			float netForce = forceFromFinger - forceFromMirrorFinger;

			// create deadband: if netForce is in (-30, 30), set equal to 0
			if (netForce < 0) 
				netForce = min(0.0f, netForce + 30.0f);
			else if (netForce > 0)
				netForce = max(0.0f, netForce - 30.0f);

			// PID loop for velocity with some memory
			float newVelocity = 0.05*pinVelocity[i][j] - 0.95*(kp*netForce + kd*(netForce - lastNetForce[i][j]));

			// getting faster requires acceleration; getting slower does not
			if (abs(newVelocity) < pinVelocity[i][j])
				pinVelocity[i][j] = newVelocity;
			else
				pinVelocity[i][j] = 0.94 * pinVelocity[i][j] + 0.03 * newVelocity;

			lastNetForce[i][j] = netForce;

			

			newHeights[i][j] = 1.5*(heightsFromShapeDisplay[i][j] - heightsForShapeDisplay[i][j]) + heightsForShapeDisplay[i][j] + dt*pinVelocity[i][j];

		}
	}

	// set new pin heights
	for (int i = 0; i < INFORCE_COLS_HALF; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			heightsForShapeDisplay[i][j] = newHeights[i][j]; //DEFINE NEW PIN HIGHT
		}
	}

	// boundaries
	for (int i = 0; i < INFORCE_COLS_HALF; i++) {
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

	// copy other half

	for (int i = 0; i < INFORCE_COLS_HALF; i++) { //0-3 --> 6-9
		for (int j = 0; j < INFORCE_ROWS; j++) {
			heightsForShapeDisplay[6+i][j] = 2*midHeight - heightsForShapeDisplay[i][j]; //DEFINE NEW PIN HIGHT
		}
	}

}

string MirrorApp::appInstructionsText() {
	string instructions = (string) "" +
		"This is for testing things."
		"";

	return instructions;
}

void MirrorApp::keyPressed(int key) {
	cout << "keyPressed" << endl;

	if (key == 'w') {
		leftStrength = 3.0;
	}
	else if (key == 's') {
		leftStrength = 1.0;
	}
	else if (key == 'r') {


	}
}