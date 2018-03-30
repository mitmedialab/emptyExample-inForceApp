#include "GradientApp.h"
#include <iostream>
#include <fstream>

GradientApp::GradientApp() {
	cout << "###### Application created: " << GradientApp::getName() << endl;

	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			//maxHeightForShapeDisplay[i][j] = ofMap(ofDist(i,j,4.5,2), 0, 6, maxHeight, minHeight); //maxHeight
			maxHeightForShapeDisplay[i][j] = maxHeight;
			minHeightForShapeDisplay[i][j] = minHeight;

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

	dataSender.switchOperationMode(2); // gradient Operation
	updateOscSend();

}

void GradientApp::update(float dt) {
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

	// calculate pin movement
	float newHeights[INFORCE_COLS][INFORCE_ROWS];
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {

			float forceFromSpring = getTargetForce(heightsFromShapeDisplay[i][j]); // DEFINE force pushing upwards   
			forceN[i][j] = forceFromSpring;
			float netForce = pins[i][j]->getForce() - forceFromSpring;

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

			newHeights[i][j] = heightsForShapeDisplay[i][j];
			if (pinVelocity[i][j] < 0) // going down
				newHeights[i][j] += kd_height*(heightsFromShapeDisplay[i][j] - heightsForShapeDisplay[i][j]) + ofMap(forceFromSpring, 200, 1500, 1.0, 0.5) * dt*pinVelocity[i][j];
			else // going up is faster
				newHeights[i][j] += kd_height*(heightsFromShapeDisplay[i][j] - heightsForShapeDisplay[i][j]) + dt*speedGoingUp*pinVelocity[i][j];
			//1.5*
		}
	}

	// set new pin heights
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			heightsForShapeDisplay[i][j] = newHeights[i][j]; //DEFINE NEW PIN HIGHT
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

void GradientApp::updateOscSend() {

	sendOscParameter();

}

void GradientApp::sendOscParameter() {
	const int parameterNumberPerPin = 1; // change here depending on the number of parameter per pin
	int dataSize = 1 + (INFORCE_COLS * INFORCE_ROWS) * parameterNumberPerPin;
	int32_t data[1 + (INFORCE_COLS * INFORCE_ROWS) * parameterNumberPerPin];
	data[0] = (INFORCE_COLS * INFORCE_ROWS) * parameterNumberPerPin;

	for (int x = 0; x < INFORCE_COLS; x++) {
		for (int y = 0; y < INFORCE_ROWS; y++) {
			data[1 + (y + x * INFORCE_ROWS) * parameterNumberPerPin] = 30000 + 5000 * x; // max
			// if you have other parameter write like this
			// data[1 + (y + x * INFORCE_ROWS) * parameterNumberPerPin + 1] = 30000 + 5000 * x;
			// data[1 + (y + x * INFORCE_ROWS) * parameterNumberPerPin + 2] = 30000 + 5000 * x;
		}
	}

	dataSender.sendOperationParameter(data, dataSize);
}
float GradientApp::getTargetForce(int height) {
	return ofMap(height, maxHeight, minHeight, 65, 1500);
}

float GradientApp::getTargetForce2(int height) {
	return 200;
}

float GradientApp::getTargetForce3(int height) {
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

float GradientApp::getTargetForce4(int height) {
	if (height > 65000)
		return 200;
	else
		return 900;
}

float GradientApp::getTargetForce5(int height, int x, int y) {
	if ((x == 3 && y == 3) || (x == 7 && y == 2)) {
		return 900;
	}
	else {
		return 200;
	}
}



float GradientApp::getTargetFrequency(int height) {
	float fq = 5;
	if (height < 40000) {
		fq = 1;
	}
	else if (height < 78000) {
		fq = 20;
	}


	return fq;

}


string GradientApp::appInstructionsText() {
	string instructions = (string) "" +
		"This is for testing things."
		"";

	return instructions;
}

void GradientApp::keyPressed(int key) {
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