#include "LayerApp2.h"
#include <iostream>
#include <fstream>

LayerApp2::LayerApp2() {
	cout << "###### Application created: " << LayerApp2::getName() << endl;
	dataSender.switchOperationMode(0);


	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			heightsForShapeDisplay[i][j] = maxHeight;
		}
	}

	/*
	for (int i = 0; i < NUM_LAYERS; i++) {
		layerStiffness[i] = 300 + i * 100;
	}*/

	layerStiffness[0] = 200;
	layerStiffness[1] = 1200;
	layerStiffness[2] = 2200;
	layerStiffness[3] = 500;
	layerStiffness[4] = 1500;
	layerStiffness[5] = 2500;
	layerStiffness[6] = 2000;
	layerStiffness[7] = 1200;
	layerStiffness[8] = 4000;
	layerStiffness[9] = 300;

	for (int i = 0; i < NUM_LAYERS; i++) {
		layerHeight[i] = maxHeight - (float(i) / (NUM_LAYERS - 1)) * (maxHeight - minHeight);
	}

}


float LayerApp2::layerToHeightOfLayer(int layer) {
	// layer 0: maxHeight
	// layer NUM_LAYERS-1: minHeight


	//return maxHeight - (float(layer) / (NUM_LAYERS - 1)) * (maxHeight - minHeight);

	return layerHeight[layer];
}

float LayerApp2::layerToStiffnessOfLayer(int layer) {
	return layerStiffness[layer];
}

void LayerApp2::update(float dt) {
	updateOscSend();

	time += dt;

	// give some time to calibrate
	if (time < 3) {
		if (time > 2) {
			for (int i = 0; i < INFORCE_COLS; i++) {
				for (int j = 0; j < INFORCE_ROWS; j++) {
					pins[i][j]->calibrate(dt, forceFromShapeDisplay[i][j]);
				}
			}
		}
		return;
	}

	

	if (pauseTime > 0) {
		pauseTime -= dt;
		return;
	}

	float forceFromFinger = 0;
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			forceFromFinger += max(0.0f, pins[i][j]->getForce() - 300);
		}
	}

	if (forceFromFinger > layerToStiffnessOfLayer(currentLayer) && currentLayer < NUM_LAYERS-1) {
		currentLayer++;
		cout << "Current layer: " << currentLayer << ", stiffness: " << layerToStiffnessOfLayer(currentLayer) << ", height: " << layerToHeightOfLayer(currentLayer) << endl;
		pauseTime = 1.5;
	}

	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			heightsForShapeDisplay[i][j] = layerToHeightOfLayer(currentLayer);
		}
	}
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
		currentLayer = 0;
		cout << "Current layer: " << currentLayer << ", stiffness: " << layerToStiffnessOfLayer(currentLayer) << ", height: " << layerToHeightOfLayer(currentLayer) << endl;
	} else if (key == 'r') {
		randomLayer();
		currentLayer = 0;
		cout << "Current layer: " << currentLayer << ", stiffness: " << layerToStiffnessOfLayer(currentLayer) << ", height: " << layerToHeightOfLayer(currentLayer) << endl;
	}
	else if (key == 'q') {
		currentLayer--;
		if (currentLayer < 0) {
			currentLayer = 0;
		}
		cout << "Current layer: " << currentLayer << ", stiffness: " << layerToStiffnessOfLayer(currentLayer) << ", height: " << layerToHeightOfLayer(currentLayer) << endl;
	}
	else if (key == 'e') {
		currentLayer++;
		if (currentLayer == NUM_LAYERS) {
			currentLayer = NUM_LAYERS-1;
		}
		cout << "Current layer: " << currentLayer << ", stiffness: " << layerToStiffnessOfLayer(currentLayer) << ", height: " << layerToHeightOfLayer(currentLayer) << endl;
	}

}

void LayerApp2::randomLayer() {
	
	float currentHeight = maxHeight;
	layerHeight[0] = maxHeight;
	layerStiffness[0] = 10 * (int)ofRandom(15, 600);
	NUM_LAYERS = 1;
	

	while (currentHeight > minHeight + 5000) {
		currentHeight = currentHeight - 100 * (int)ofRandom(50, 250);



		if (currentHeight > minHeight + 5000) {
			layerHeight[NUM_LAYERS] = currentHeight;
			layerStiffness[NUM_LAYERS] = 10 * (int)ofRandom(15,600);
			cout << "layer Number " << NUM_LAYERS << " has height of "<< currentHeight << " and stiffness of" << layerStiffness[NUM_LAYERS] << endl;
			NUM_LAYERS++;
		}
		else {
			layerHeight[NUM_LAYERS] = minHeight;
			layerStiffness[NUM_LAYERS] = 10 * (int)ofRandom(15, 600);
			NUM_LAYERS++;
		}
		
	}

	cout << "layer of Number is " << NUM_LAYERS << endl;


}