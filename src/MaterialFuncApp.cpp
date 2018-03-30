#include "MaterialFuncApp.h"
#include <iostream>
#include <fstream>

MaterialFuncApp::MaterialFuncApp() {
	cout << "###### Application created: " << MaterialFuncApp::getName() << endl;

	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {

			// static haptic parameter is defined here
			/*
			if (i == 0) {

				setPinParameter(i, j, maxHeight, maxHeight, maxForce, 0, false);
			}
			else {
				int gradationForce = ofMap(i, 1, INFORCE_COLS-1,  maxForce, minForce);
				setPinParameter(i, j, maxHeight, minHeight, gradationForce, 0, false);
			}
			*/

			int gradationFrequency = ofMap(i, INFORCE_COLS - 1, 0, 1, maxFrequency);
			setPinParameter(i, j, minHeight, minHeight, 900, gradationFrequency, false);


			heightsForShapeDisplay[i][j] = maxHeightForShapeDisplay[i][j];
			pinVelocity[i][j] = 0;
			lastNetForce[i][j] = 0;
		}
	}


}

void MaterialFuncApp::update(float dt) {



	time += dt;
	if (time > 2 && time < 5) {
		for (int i = 0; i < INFORCE_COLS; i++) {
			for (int j = 0; j < INFORCE_ROWS; j++) {
				pins[i][j]->calibrate(dt, forceFromShapeDisplay[i][j]);
			}
		}
		return;
	}

	// setPinParameter here for dynamic control
	// setPinParameter(int x, int y, int maxH, int minH, int force, float frequency)


	processPinPositionUsingParameter(dt);

}

void MaterialFuncApp::drawApp() {

}

float MaterialFuncApp::getTargetForce(int height) {
	return ofMap(height, 20000, maxHeight, 1000, 500);
}

float MaterialFuncApp::getTargetForce2(int height) {
	return minForce;
}

float MaterialFuncApp::getTargetForce3(int height) {
	if (height > 65000)
		return minForce;
	else if (height > 50000)
		return midForce;
	else if (height > 35000)
		return minForce;
	else if (height > 20000)
		return midForce;
	else
		return minForce;
}

float MaterialFuncApp::getTargetForce4(int height) {
	if (height > 65000)
		return 200;
	else
		return 900;
}

float MaterialFuncApp::getTargetFrequency(int height) {
	float fq = 5;
	if (height < 40000) {
		fq = 1;
	}
	else if (height < 78000) {
		fq = 20;
	}


	return fq;

}


void MaterialFuncApp::setPinParameter(int x, int y, int maxH, int minH, int force, float frequency, bool vibrationWithPushing) {
	maxHeightForShapeDisplay[x][y] = maxH;
	minHeightForShapeDisplay[x][y] = minH;

	if (pinVelocity[x][y] < 100 || vibrationWithPushing) {
		frequencyForShapeDisplay[x][y] = frequency;
	}
	else {
		frequencyForShapeDisplay[x][y] = 0;
	}

	forceN[x][y] = force;

}

void MaterialFuncApp::resetPinCalculationParameter(int x, int y) {
	pinVelocity[x][y] = 0;
	lastNetForce[x][y] = 0;
	pinVelocity[x][y] = 0;
}

void MaterialFuncApp::processPinPositionUsingParameter(float dt) {


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
					float height = exp(-distanceSquared / variance) * pins[i][j]->getForce();
					forceFromFinger[ii][jj] += height;
				}
			}
		}
	}

	// calculate pin movement
	float newHeights[INFORCE_COLS][INFORCE_ROWS];
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {

			float forceFromSpring = forceN[i][j]; // DEFINE force pushing upwards   
			float forceFromFinger = pins[i][j]->getForce();
			float netForce = forceFromFinger - forceFromSpring;

			if (netForce < -100)
				netForce += 100;
			else if (netForce < 0)
				netForce = 0;


			float newVelocity = 0.05*pinVelocity[i][j] - 0.95*(kp*netForce + kd*(netForce - lastNetForce[i][j]));
			if (abs(newVelocity) < pinVelocity[i][j])
				pinVelocity[i][j] = newVelocity;
			else
				pinVelocity[i][j] = 0.97 * pinVelocity[i][j] + 0.03 * newVelocity;

			lastNetForce[i][j] = netForce;

			newHeights[i][j] = 0.95*heightsFromShapeDisplay[i][j] + 0.05*heightsForShapeDisplay[i][j] + dt*pinVelocity[i][j];
		
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

	// set new pin heights
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			if (forceN[i][j] <= maxForce) {
				heightsForShapeDisplay[i][j] = indentations[i][j]; //DEFINE NEW PIN HIGHT
			}
			else {
				heightsForShapeDisplay[i][j] = forceN[i][j] - maxForce;
				newHeights[i][j] = 0.95*heightsFromShapeDisplay[i][j] + 0.05*heightsForShapeDisplay[i][j] + dt*pinVelocity[i][j];
			}

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


string MaterialFuncApp::appInstructionsText() {
	string instructions = (string) "" +
		"This is for testing things."
		"";

	return instructions;
}

void MaterialFuncApp::keyPressed(int key) {
	switch (key)
	{
	case OF_KEY_UP:

		break;

	}
}