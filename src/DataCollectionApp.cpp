#include "DataCollectionApp.h"
#include <iostream>
#include <fstream>

DataCollectionApp::DataCollectionApp() {
	cout << "###### Application created: " << DataCollectionApp::getName() << endl;
	outputFile.open("calibrationData2.txt");
	outputFile << "time i j heightsForShapeDisplay heightsFromShapeDisplay forceFromShapeDisplay\n";
}

void DataCollectionApp::update(float dt) {
	// Data collection start
	time += dt;
	if (phase < 30) {
		for (int i = 0; i < INFORCE_COLS; i++) {
			for (int j = 0; j < INFORCE_ROWS; j++) { 
				outputFile << time << " " << i << " " << j << " ";
				outputFile << heightsForShapeDisplay[i][j] << " ";
				outputFile << heightsFromShapeDisplay[i][j] << " ";
				outputFile << forceFromShapeDisplay[i][j] << "\n";
			}
		}
	}
	else {
		outputFile.close();
	}

	// Data collection end

	if (time > 5) {
		pause -= dt;
		if (pause <= 0) {
			if (up == 1)
			{
				velocity = 100 * phase;
				if (height > maxHeight) {
					up = -1;
					velocity = 0;
					pause = 2;
				}
			}
			else {
				velocity = -100 * phase;
				if (height < minHeight) {
					up = 1;
					velocity = 0;
					pause = 2;
					phase += 1;
				}
			}
		}
	}
	if (phase < 30) 
		height += velocity;

	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			heightsForShapeDisplay[i][j] = height;
		}
	}
}


string DataCollectionApp::appInstructionsText() {
	string instructions = (string) "" +
		"This is for testing things."
		"";

	return instructions;
}

void DataCollectionApp::keyPressed(int key) {
	cout << "keyPressed" << endl;

	if (key == 'w') {
		velocity = setVelocity;
	}
	else if (key == 's') {
		velocity = -setVelocity;
	}
	else if (key == 'a') {
		velocity = -2000;
	}
	else if (key == 'd') {
		velocity = 2000;
	}
	else if (key == 'f') {
		velocity = 0;
	}
	else if (key == 'z') {
		velocity -= 50;
	}
	else if (key == 'x') {
		velocity += 50;
	}
	else if (key == '1') {
		setVelocity = 200;
	}
	else if (key == '2') {
		setVelocity = 400;
	}
	else if (key == '3') {
		setVelocity = 600;
	}
	else if (key == '4') {
		setVelocity = 800;
	}
	else if (key == '5') {
		setVelocity = 1000;
	}
	else if (key == '6') {
		setVelocity = 1200;
	}
	else if (key == '7') {
		setVelocity = 1400;
	}
	else if (key == '8') {
		setVelocity = 1600;
	}


}