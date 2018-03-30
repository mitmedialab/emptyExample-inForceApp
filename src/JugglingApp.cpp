#include "JugglingApp.h"
#include <iostream>
#include <fstream>

JugglingApp::JugglingApp() {
	cout << "###### Application created: " << JugglingApp::getName() << endl;

	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {

		}
	}



}

void JugglingApp::update(float dt) {
	time += dt;
	//throwTime += dt;
	// give some time to calibrate
	if (time < 5) {
		if (time > 2) {

		}
		return;
	}

	if (throwTime > 3.12f) {
		height = minHeight;

	}



	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {

			float distanceFromCentreSquared = (i - centerI)*(i - centerI) + (j - centerJ)*(j - centerJ);
			float heightDiff = max(0.0f, sqrt(ballRadius*ballRadius - distanceFromCentreSquared) - ballHeight);
			heightsForShapeDisplay[i][j] = height - scale*heightDiff;
		}
	}
}



string JugglingApp::appInstructionsText() {
	string instructions = (string) "" +
		"This is for testing things."
		"";

	return instructions;
}

void JugglingApp::keyPressed(int key) {
	cout << "keyPressed" << endl;

	if (key == 'w') {
		height = maxHeight;
		//throwTime = 0.0f;
	}
	else if (key == 's') {
		height = minHeight;
	}

}