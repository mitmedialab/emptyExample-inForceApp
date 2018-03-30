#include "MaterialApp2.h"
#include <iostream>
#include <fstream>

MaterialApp2::MaterialApp2() {
	cout << "###### Application created: " << MaterialApp2::getName() << endl;

	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			maxHeightForShapeDisplay[i][j] = maxHeight;
			minHeightForShapeDisplay[i][j] = minHeight;
		}
	}

	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			heightsForShapeDisplay[i][j] = maxHeight;
			pinVelocity[i][j] = 0;
			lastNetForce[i][j] = 0;

			frequencyForShapeDisplay[i][j] = 0;
		}
	}
	for (int i = -1; i < INFORCE_COLS + 1; i++) {
		for (int j = -1; j < INFORCE_ROWS + 1; j++) {
			heightsForShapeDisplayBuffered[i + 1][j + 1] = maxHeight;
			heightsFromShapeDisplayBuffered[i + 1][j + 1] = maxHeight;
		}
	}
}

void MaterialApp2::update(float dt) {
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
					if (heightsForShapeDisplay[i][j] > 40000) variance = 8.0;
					else if (heightsForShapeDisplay[i][j] > 30000) variance = 5.0;
					else if (heightsForShapeDisplay[i][j] > 20000) variance = 4.0;
					else variance = 3.0;

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

			float forceFromSpring = getTargetForce3(heightsFromShapeDisplay[i][j]); // force pushing upwards
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

			if (pinVelocity[i][j] > 0)
				pinVelocity[i][j] = 0;


			if ((pins[i][j]->heightForShapeDisplayHistory[0] - 35000)*(pins[i][j]->heightForShapeDisplayHistory[5] - 35000) < 0) {
				pinVelocity[i][j] = 0;
			}

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
					if (newHeights[i][j] > 40000) variance = 16.0;
					else if (newHeights[i][j] > 30000) variance = 5.0;
					else if (newHeights[i][j] > 20000) variance = 4.0;
					else variance = 3.0;

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
			heightsForShapeDisplay[i][j] = indentations[i][j];
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
float MaterialApp2::getTargetForce(int height) {
	return ofMap(height, maxHeight, (maxHeight + minHeight) / 2, 300, 1500);
}

float MaterialApp2::getTargetForce2(int height) {
	return 300;
}

float MaterialApp2::getTargetForce3(int height) {
	if (height > 35000)
		return 200;
	else
		return 800;
}

float MaterialApp2::getTargetForce4(int height) {
	if (height > 65000)
		return 200;
	else
		return 900;
}

float MaterialApp2::getTargetFrequency(int height) {
	float fq = 5;
	if (height < 40000) {
		fq = 1;
	}
	else if (height < 78000) {
		fq = 20;
	}


	return fq;

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
		for (int i = -1; i < INFORCE_COLS + 1; i++) {
			for (int j = -1; j < INFORCE_ROWS + 1; j++) {
				heightsForShapeDisplayBuffered[i + 1][j + 1] = maxHeight;
				heightsFromShapeDisplayBuffered[i + 1][j + 1] = maxHeight;
			}
		}
	}
}