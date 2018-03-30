#include "MountainApp.h"
#include <iostream>
#include <fstream>

MountainApp::MountainApp() {
	cout << "###### Application created: " << MountainApp::getName() << endl;

	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			maxHeightForShapeDisplay[i][j] = ofMap(ofDist(i,j,4.5,2), 0, 6, maxHeight, minHeight); //maxHeight
			//maxHeightForShapeDisplay[i][j] = maxHeight;
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

}

void MountainApp::update(float dt) {
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
						forceFromFinger[ii][jj] += scaleFactor * max(0.0f, pins[i][j]->getForce() - 60 * scaleFactor);
				}
			}
		}
	}

	// calculate pin movement
	float newHeights[INFORCE_COLS][INFORCE_ROWS];
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {

			float forceFromSpring = getTargetForce(heightsFromShapeDisplay[i][j]); // DEFINE force pushing upwards   
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
				newHeights[i][j] = 1.5*(heightsFromShapeDisplay[i][j] - heightsForShapeDisplay[i][j]) + heightsForShapeDisplay[i][j] + ofMap(forceFromSpring, 200, 1500, 1.0, 0.5) * dt*pinVelocity[i][j];
			else
				newHeights[i][j] = 1.5*(heightsFromShapeDisplay[i][j] - heightsForShapeDisplay[i][j]) + heightsForShapeDisplay[i][j] + dt*speedGoingUp*pinVelocity[i][j];

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
float MountainApp::getTargetForce(int height) {
	return ofMap(height, maxHeight, (maxHeight + minHeight) / 2, 130, 1500);
}

float MountainApp::getTargetForce2(int height) {
	return 200;
}

float MountainApp::getTargetForce3(int height) {
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

float MountainApp::getTargetForce4(int height) {
	if (height > 65000)
		return 200;
	else
		return 900;
}

float MountainApp::getTargetForce5(int height, int x, int y) {
	if ((x == 3 && y == 3) || (x == 7 && y == 2)) {
		return 900;
	}
	else {
		return 200;
	}
}



float MountainApp::getTargetFrequency(int height) {
	float fq = 5;
	if (height < 40000) {
		fq = 1;
	}
	else if (height < 78000) {
		fq = 20;
	}


	return fq;

}


string MountainApp::appInstructionsText() {
	string instructions = (string) "" +
		"This is for testing things."
		"";

	return instructions;
}

void MountainApp::keyPressed(int key) {
	cout << "keyPressed" << endl;

	if (key == 'w') {
		variance *= 1.3;
	}
	else if (key == 's') {
		variance /= 1.3;
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