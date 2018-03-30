#include "MaterialApp2.h"
#include <iostream>
#include <fstream>

MaterialApp2::MaterialApp2() {
	cout << "###### Application created: " << MaterialApp2::getName() << endl;

	variance = 16;

	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			heightsForShapeDisplay[i][j] = maxHeight;
			pinVelocity[i][j] = 0;
			lastDeltaForce[i][j] = 0;
			//stiffnessForShapeDisplay[i][j] = (9 - i) * 90 + 10; //up to 630?
			minHeightForShapeDisplay[i][j] = 10000;
			maxHeightForShapeDisplay[i][j] = 80000;
			isTouched[i][j] = FALSE;
		}
	}
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			if (j == 0 || j == 4)
				disablePin[i][j] = TRUE;
			else
				disablePin[i][j] = FALSE;
		}
	}
}

void MaterialApp2::update(float dt) {
	time += dt;
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			forceFromTouchedPins[i][j] = 0;
			heightsFromTouchedPins[i][j] = 0;
		}
	}

	if (time < 2) {
		for (int i = 0; i < INFORCE_COLS; i++) {
			for (int j = 0; j < INFORCE_ROWS; j++) {
				pins[i][j]->calibrate(dt, forceFromShapeDisplay[i][j]);
				if (disablePin[i][j]) {
					heightsForShapeDisplay[i][j] = 0;
				}
			}
		}
		return;
	}
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			float forceFromSpring = ofMap(heightsFromShapeDisplay[i][j], 80000, 50000, 250, 200);
			stiffnessForShapeDisplay[i][j] = 250;
			float deltaForce = pins[i][j]->getForce() - forceFromSpring;

			pinVelocity[i][j] = 0.1*pinVelocity[i][j] - 0.9*(kp*deltaForce + kd*(deltaForce - lastDeltaForce[i][j]));
			
			lastDeltaForce[i][j] = deltaForce;


			heightsForShapeDisplay[i][j] = 0.95*heightsFromShapeDisplay[i][j] + 0.05*heightsForShapeDisplay[i][j] + dt*pinVelocity[i][j];

			int32_t maxHeight_t = maxHeightForShapeDisplay[i][j];
			int32_t minHeight_t = minHeightForShapeDisplay[i][j];
			//int32_t minHeight_t = maxHeight_t - ofMap(i, 0, 9, 30000, 60000);
			if (heightsForShapeDisplay[i][j] < minHeight_t) {
				heightsForShapeDisplay[i][j] = minHeight_t+1000; //change here for lower boundary
				pinVelocity[i][j] = 0;
			}

			if (heightsForShapeDisplay[i][j] > maxHeight_t) {
				heightsForShapeDisplay[i][j] = maxHeight_t; //change here for higher boundary (shape)
				pinVelocity[i][j] = 0;
			}

			if ((i == 0 && j == 0) || (i == 3 && j == 0) || (i == 6 && j == 0))
				heightsForShapeDisplay[i][j] = maxHeightForShapeDisplay[i][j];
			if (heightsForShapeDisplay[i][j] > maxHeightForShapeDisplay[i][j] - 5000) {
				isTouched[i][j] = FALSE;
				if (pins[i][j]->getForce() > 400)
					isTouched[i][j] = TRUE;
			}

			if (isTouched[i][j]) {
				for (int ii = 0; ii < INFORCE_COLS; ii++) {
					for (int jj = 0; jj < INFORCE_ROWS; jj++) {
						if (!((i == 0 && j == 0) || (i == 3 && j == 0) || (i == 6 && j == 0))) {
							float distanceSquared = (i - ii)*(i - ii) + (j - jj)*(j - jj);
							float height = exp(-distanceSquared / variance) * (maxHeightForShapeDisplay[i][j] - heightsForShapeDisplay[i][j]);
							heightsFromTouchedPins[ii][jj] += height;
						}
					}
				}
			}
		}
	}
	isTouched[0][0] = FALSE;
	isTouched[3][0] = FALSE;
	isTouched[6][0] = FALSE;
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			if (!isTouched[i][j]) {
				if (i == 9 && j == 1)
					cout << "huh " << heightsFromTouchedPins[i][j] << endl;

				heightsForShapeDisplay[i][j] = maxHeightForShapeDisplay[i][j] - 1.0*heightsFromTouchedPins[i][j];
			}
			else {
				cout << "TOUCHED " << i << " " << j << endl;

			}
		}
	}
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			if (disablePin[i][j])
				heightsForShapeDisplay[i][j] = 0;
		}
	}
}
float MaterialApp2::getForce(int height, int max) {
	return ofMap(height, minHeight, maxHeight, 600, 50);
}

float MaterialApp2::getForce2(int height) {
	return minForce;
}

float MaterialApp2::getForce3(int height) {
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


string MaterialApp2::appInstructionsText() {
	string instructions = (string) "" +
		"This is for testing things."
		"";

	return instructions;
}

void MaterialApp2::keyPressed(int key) {
	cout << "keyPressed" << endl;

	if (key == 'w') {
		variance *= 1.1;
	}
	else if (key == 's') {
		variance /= 1.1;
	}
	
}