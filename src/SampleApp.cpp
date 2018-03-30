#include "LayerApp2.h"
#include <iostream>
#include <fstream>

LayerApp2::LayerApp2() {
	cout << "###### Application created: " << LayerApp2::getName() << endl;



	dataSender.switchOperationMode(0);
}


float linearCombination(float x1, float x2, float alpha) {
	return x1 * (1.0 - alpha) + x2 * alpha;
}

void LayerApp2::update(float dt) {
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


	updateOscSend();
}

void LayerApp2::updateOscSend() {
	dataSender.sendHeightData(heightsForShapeDisplay);
}



string LayerApp2::appInstructionsText() {
	string instructions = (string) "" +
		"This is for testing things."
		"";

	return instructions;
}

void LayerApp2::keyPressed(int key) {
	cout << "keyPressed" << endl;

	if (key == 'w') {

	}

}