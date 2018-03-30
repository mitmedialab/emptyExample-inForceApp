#include "TouchTestApp.h"

TouchTestApp::TouchTestApp() {
	cout << "###### Application created: " << TouchTestApp::getName() << endl;


	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			pins[i][j] = new PinModel(i, j);
			forceFromShapeDisplayEMA[i][j] = 0;
		}
	}
}

void TouchTestApp::update(float dt) {
	
	if (calibrationTime > 0) {
		for (int i = 0; i < INFORCE_COLS; i++) {
			for (int j = 0; j < INFORCE_ROWS; j++) {
				pins[i][j]->calibrate(dt, forceFromShapeDisplay[i][j]);	
				heightsForShapeDisplay[i][j] = maxHeight;
				if (i == 0 && j == 0)
					cout << forceFromShapeDisplay[0][0] << ", " << pins[0][0]->restMean << ", " << pins[0][0]->restVar << endl;	
			}
		}
		calibrationTime -= dt;
	}
	else {
		for (int i = 0; i < INFORCE_COLS; i++) {
			for (int j = 0; j < INFORCE_ROWS; j++) {
				pins[i][j]->update(dt, 0, 0, forceFromShapeDisplay[i][j]);

				float force = pins[i][j]->getForce();

				if (i == 0 && j == 0) {
					PinModel* pin = pins[i][j];
					float zScore = abs(pin->currentRawForce - pin->restMean) / pow(pin->restVar, 0.5);

					if (force != 0) {
						cout << "Pin touched " << i << " " << j << endl;
						cout << zScore << endl;
					}
				}
				
				//if (force != 0)
				//	cout << "Pin touched " << i << " " << j << endl;

				float alpha = pow(0.5, dt / halfLife);
				forceFromShapeDisplayEMA[i][j] = alpha*forceFromShapeDisplayEMA[i][j] + (1 - alpha)*pins[i][j]->getForce();
			}
		}

		for (int i = 0; i < INFORCE_COLS; i++) {
			for (int j = 0; j < INFORCE_ROWS; j++) {
				float forceFromFinger = forceFromShapeDisplayEMA[i][j];

				//heightsForShapeDisplay[i][j] = maxHeight - 30 * forceFromFinger;

				
				float forceFromSpring = getForce2(heightsFromShapeDisplay[i][j]);   

				float deltaForce = forceFromFinger - forceFromSpring;

				//if (deltaForce > 0)
					heightsForShapeDisplay[i][j] -= dt * speedConstant * (deltaForce)*0.8;

				if (heightsForShapeDisplay[i][j] < minHeight)
					heightsForShapeDisplay[i][j] = minHeight;
				if (heightsForShapeDisplay[i][j] > maxHeight)
					heightsForShapeDisplay[i][j] = maxHeight;
			}
		}
	}
}


float TouchTestApp::getForce(int height) {
	float k = float(maxHeight - height) / (maxHeight - minHeight);
	return k*k*maxForce + (1 - k*k)*minForce;
}

float TouchTestApp::getForce2(int height) {
	return minForce;
}

float TouchTestApp::getForce3(int height) {
	// minHeight:1000, maxHeight:80,000
	float midForce = 0.2 * maxForce + 0.8 * minForce;
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




string TouchTestApp::appInstructionsText() {
	string instructions = (string) "" +
		"This is for testing things."
		"";

	return instructions;
}

void TouchTestApp::keyPressed(int key) {
	cout << "keyPressed" << endl;

	if (key == 'w') {
		
	}



}