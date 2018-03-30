#include <iostream>
#include <fstream>
#include <string>

#include "Application.h"


Application::Application() {
	//cout << "###### Application created: " << Application::getName() << endl;



	// Create pins
	for (int i = 0; i < INFORCE_COLS; i++)
		for (int j = 0; j < INFORCE_ROWS; j++)
			pins[i][j] = new PinModel(i, j);

	// Initialize pin parameters
	string line;
	ifstream myfile("calibrationData 3-19-2018.txt");

	if (myfile.is_open())
	{
		getline(myfile, line);
		int numLines = std::stoi(line);
		for (int n = 0; n < numLines; n += 4) {
			getline(myfile, line);
			int index = std::stoi(line);

			getline(myfile, line);
			int upOrDown = std::stoi(line);

			getline(myfile, line);
			int height = std::stoi(line);

			getline(myfile, line);
			float force = std::stoi(line);

			// index = i*ROWS + j
			int i = index / INFORCE_ROWS;
			int j = index % INFORCE_ROWS;

			pins[i][j]->setForceCurve(upOrDown, height, force);
		}
		myfile.close();
	}
};

void Application::updatePins(float dt) {
	for (int i = 0; i < INFORCE_COLS; i++)
		for (int j = 0; j < INFORCE_ROWS; j++)
			pins[i][j]->update(dt, heightsForShapeDisplay[i][j], heightsFromShapeDisplay[i][j], forceFromShapeDisplay[i][j]);
}

void Application::getHeightsForShapeDisplay(int32_t heights[INFORCE_COLS][INFORCE_ROWS]) {
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			heights[i][j] = heightsForShapeDisplay[i][j];
		}
	}
};

void Application::getFrequencyForShapeDisplay(float frequency[INFORCE_COLS][INFORCE_ROWS]) {
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			frequency[i][j] = frequencyForShapeDisplay[i][j];
		}
	}
};


void Application::setHeightsFromShapeDisplayRef(int32_t heights[INFORCE_COLS][INFORCE_ROWS]) {
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			heightsFromShapeDisplay[i][j] = heights[i][j];
		}
	}

};

void Application::setForceFromShapeDisplayRef(int32_t force[INFORCE_COLS][INFORCE_ROWS]) {
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			forceFromShapeDisplay[i][j] = force[i][j];
		}
	}
}
void Application::setTouchFromShapeDisplayRef(string touch[INFORCE_COLS][INFORCE_ROWS]) {
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			touchFromShapeDisplay[i][j] = touch[i][j];
		}
	}
}

void Application::setStiffnessForDisplay(int32_t stiffness[INFORCE_COLS][INFORCE_ROWS]) {
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			stiffnessForShapeDisplay[i][j] = stiffness[i][j];
		}
	}
}

void Application::setMaxHeightForDisplay(int32_t maxHeight_[INFORCE_COLS][INFORCE_ROWS]) {
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			maxHeightForShapeDisplay[i][j] = maxHeight_[i][j];
		}
	}
}

void Application::setMinHeightForDisplay(int32_t minHeight_[INFORCE_COLS][INFORCE_ROWS]) {
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			minHeightForShapeDisplay[i][j] = minHeight_[i][j];
		}
	}
}

int32_t Application::getTargetForce(int x, int y) {
	return forceN[x][y];

}