#include "MaterialAppPID.h"
#include <iostream>
#include <fstream>

MaterialAppPID::MaterialAppPID() {
	cout << "###### Application created: " << MaterialAppPID::getName() << endl;

	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			heightsForShapeDisplay[i][j] = maxHeight;
			pinVelocity[i][j] = 0;
			lastDeltaForce[i][j] = 0;
			stiffnessForShapeDisplay[i][j] = (9 - i) * 90 + 10; //up to 630?
			minHeightForShapeDisplay[i][j] = 70000;
			maxHeightForShapeDisplay[i][j] = 80000;

		}
	}
}

void MaterialAppPID::update(float dt) {
	time += dt;

	if (time < 2) {
		for (int i = 0; i < INFORCE_COLS; i++) {
			for (int j = 0; j < INFORCE_ROWS; j++) {
				pins[i][j]->calibrate(dt, forceFromShapeDisplay[i][j]);
				//heightsForShapeDisplay[i][j] = maxHeight;
			}
		}
		return;
	}
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {

			float forceFromSpring = stiffnessForShapeDisplay[i][j];
			float deltaForce = pins[i][j]->getForce() - forceFromSpring;
			//if (deltaForce < -50)
			//	deltaForce = -200;

			pinVelocity[i][j] = 0.5*pinVelocity[i][j] - 0.5*(kp*deltaForce + kd*(deltaForce - lastDeltaForce[i][j]));
			
			lastDeltaForce[i][j] = deltaForce;


			heightsForShapeDisplay[i][j] = 0.25*heightsFromShapeDisplay[i][j] + 0.75*heightsForShapeDisplay[i][j] + dt*pinVelocity[i][j];

			int32_t maxHeight_t = maxHeightForShapeDisplay[i][j];
			int32_t minHeight_t = minHeightForShapeDisplay[i][j];
			//int32_t minHeight_t = maxHeight_t - ofMap(i, 0, 9, 30000, 60000);
			if (heightsForShapeDisplay[i][j] < minHeight_t) {
				heightsForShapeDisplay[i][j] = minHeight_t; //change here for lower boundary
				pinVelocity[i][j] = 0;
			}

			if (heightsForShapeDisplay[i][j] > maxHeight_t) {
				heightsForShapeDisplay[i][j] = maxHeight_t; //change here for higher boundary (shape)
				pinVelocity[i][j] = 0;
			}
		}
	}
}
float MaterialAppPID::getForce(int height, int max) {
	return ofMap(height, minHeight, maxHeight, 600, 50);
}

float MaterialAppPID::getForce2(int height) {
	return minForce;
}

float MaterialAppPID::getForce3(int height) {
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


string MaterialAppPID::appInstructionsText() {
	string instructions = (string) "" +
		"This is for testing things."
		"";

	return instructions;
}

void MaterialAppPID::keyPressed(int key) {
	cout << "keyPressed" << endl;

	if (key == 'w') {
		
	}
}