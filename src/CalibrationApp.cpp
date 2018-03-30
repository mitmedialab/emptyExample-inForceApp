#include "CalibrationApp.h"
#include <iostream>
#include <fstream>

CalibrationApp::CalibrationApp() {
	cout << "###### Application created: " << CalibrationApp::getName() << endl;
	outputFile.open("calibrationData 3-19-2018.txt");
	updateHeights(heightsOfPins);

	dataSender.switchOperationMode(0);
}


void CalibrationApp::update(float dt) {
	
	// Move pins
	pause -= dt;
	if (pause < 0) {
		if (phase == 0) {
			heightsOfPins += speed;
			updateHeights(heightsOfPins);
			pause = 0.7;
			if (heightsOfPins > maxHeight) {
				phase += 1;
				pause = 4.0;
				speed = 100;
				heightsOfPins = minHeight;
				updateHeights(minHeight);
			}
		}
		else if (phase < 10) {
			if (goingUp) {
				heightsOfPins += speed;
				updateHeights(heightsOfPins);
				if (heightsOfPins > maxHeight) {
					goingUp = false;
					pause = 1.0;
				}
			}
			else {
				heightsOfPins -= speed;
				updateHeights(heightsOfPins);
				if (heightsOfPins < minHeight) {
					phase += 1;
					goingUp = true;
					pause = 1.0;
					speed += 50;
				}
			}
		}
	}

	// Collect data
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			
			int pinHeight = (heightsFromShapeDisplay[i][j] + 500) - (heightsFromShapeDisplay[i][j] + 500) % 1000; // round to nearest 1000
			int pinForce = forceFromShapeDisplay[i][j];

			if (phase == 0) {
				if (pause > 0 && pause < 0.3) {
					heightToForceRest[i][j][pinHeight] += pinForce;
					heightToForceRestNum[i][j][pinHeight] += 1;
				}
			}
			else if (phase < 10) {
				if (pause < 0) {
					if (goingUp) {
						heightToForceGoingUp[i][j][pinHeight] += pinForce;
						heightToForceGoingUpNum[i][j][pinHeight] += 1;
					}
					else {
						heightToForceGoingDown[i][j][pinHeight] += pinForce;
						heightToForceGoingDownNum[i][j][pinHeight] += 1;;

					}
				}
			}
		}
	}
	
	if (phase == 10) {
		// Process data

		for (int i = 0; i < INFORCE_COLS; i++) {
			for (int j = 0; j < INFORCE_ROWS; j++) {
				for (int height = minHeight; height <= maxHeight; height += 1000) {
					if (heightToForceRestNum[i][j][height] > 0) heightToForceRest[i][j][height] = heightToForceRest[i][j][height] / heightToForceRestNum[i][j][height];
					if (heightToForceGoingUpNum[i][j][height] > 0) heightToForceGoingUp[i][j][height] = heightToForceGoingUp[i][j][height] / heightToForceGoingUpNum[i][j][height];
					if (heightToForceGoingDownNum[i][j][height] > 0) heightToForceGoingDown[i][j][height] = heightToForceGoingDown[i][j][height] / heightToForceGoingDownNum[i][j][height];
				}
			}
		}

		// Write calibration data to file

		// Count elements
		int numLines = 0;
		for (int i = 0; i < INFORCE_COLS; i++) {
			for (int j = 0; j < INFORCE_ROWS; j++) {
				for (int height = minHeight; height <= maxHeight; height += 1000) {
					if (heightToForceRestNum[i][j][height] > 0) numLines += 4;
					if (heightToForceGoingUpNum[i][j][height] > 0) numLines += 4;
					if (heightToForceGoingDownNum[i][j][height] > 0) numLines += 4;
				}
			}
		}

		// Write to file
		outputFile << numLines << "\n";
		for (int i = 0; i < INFORCE_COLS; i++) {
			for (int j = 0; j < INFORCE_ROWS; j++) {
				int index = i*INFORCE_ROWS + j;
				for (int height = minHeight; height <= maxHeight; height += 1000) {
					if (heightToForceRestNum[i][j][height] > 0) {
						outputFile << index << "\n";
						outputFile << "0" << "\n";
						outputFile << height << "\n";
						outputFile << heightToForceRest[i][j][height] << "\n";
					}
				}
				for (int height = minHeight; height <= maxHeight; height += 1000) {
					if (heightToForceGoingUpNum[i][j][height] > 0) {
						outputFile << index << "\n";
						outputFile << "1" << "\n";
						outputFile << height << "\n";
						outputFile << heightToForceGoingUp[i][j][height] << "\n";
					}
				}
				for (int height = minHeight; height <= maxHeight; height += 1000) {
					if (heightToForceGoingDownNum[i][j][height] > 0) {
						outputFile << index << "\n";
						outputFile << "-1" << "\n";
						outputFile << height << "\n";
						outputFile << heightToForceGoingDown[i][j][height] << "\n";
					}
				}
			}
		}
		outputFile.close();
		phase += 1;
	}
	dataSender.sendHeightData(heightsForShapeDisplay);
}

void CalibrationApp::updateOscSend() {

}

void CalibrationApp::updateHeights(int height) {
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			heightsForShapeDisplay[i][j] = height;
		}
	}
}

string CalibrationApp::appInstructionsText() {
	string instructions = (string) "" +
		"This is for testing things."
		"";

	return instructions;
}

void CalibrationApp::keyPressed(int key) {
	cout << "keyPressed" << endl;

	if (key == 'd') {
		speed *= 1.1;
	}
	else if (key == 'a') {
		speed /= 1.1;
	}



}