#include "ScannerApp.h"
#include <iostream>
#include <fstream>

ScannerApp::ScannerApp() {
	cout << "###### Application created: " << ScannerApp::getName() << endl;

	// Create pins
	for (int i = 0; i < INFORCE_COLS; i++)
		for (int j = 0; j < INFORCE_ROWS; j++)
			scannerPins[i][j] = new PinScanner(i, j, minHeight, maxHeight);

	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			heightsForShapeDisplay[i][j] = minHeight;
		}
	}

	// debug orientation
	//	cam.setAutoDistance(false);
	//	cam.setPosition(98.0635, -110.076, 233.727);
	//	cam.setOrientation(ofQuaternion(0.331259, -0.00127885, 0.000367873, 0.943539));

	//demo orientation
	cam.setAutoDistance(false);
	cam.setPosition(99.7573, 240.108, 153.508);
	cam.setOrientation(ofQuaternion(-0.00326013, 0.501557, 0.866154, 0.00281557));

}

void ScannerApp::update(float dt) {
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

	if (scanning) {
		for (int i = 0; i < INFORCE_COLS; i++) {
			for (int j = 0; j < INFORCE_ROWS; j++) {
				scannerPins[i][j]->update(dt, heightsFromShapeDisplay[i][j], pins[i][j]->getForce());
				heightsForShapeDisplay[i][j] = scannerPins[i][j]->getHeightForShapeDisplay();
			}
		}

		//cout << "numScanned: " << numScanned() << endl;

		if (doneScanned()) {
			cout << "[Scanning App] Done scanning." << endl;
			scanning = false;
		}
	}
	else {
		if (doneScanned()) {
			if (showScan) {
				for (int i = 0; i < INFORCE_COLS; i++)
					for (int j = 0; j < INFORCE_ROWS; j++)
						heightsForShapeDisplay[i][j] = minHeight + (maxHeight - scannerPins[i][j]->lowerBoundary);
			}
			else {
				for (int i = 0; i < INFORCE_COLS; i++)
					for (int j = 0; j < INFORCE_ROWS; j++)
						heightsForShapeDisplay[i][j] = scannerPins[i][j]->lowerBoundary;
			}
		
		}
		else {
			for (int i = 0; i < INFORCE_COLS; i++)
				for (int j = 0; j < INFORCE_ROWS; j++)
					heightsForShapeDisplay[i][j] = minHeight;

		}

	}
}


void ScannerApp::scan() {
	scanning = true;
	resetScan();
	scannerPins[9][2]->verbose = true;

	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			scannerPins[i][j]->pauseTime = 1.5*i + 0.2;
			scannerPins[i][j]->scanUpperBoundary();
		}
	}
}


void ScannerApp::resetScan() {
	for (int i = 0; i < INFORCE_COLS; i++)
		for (int j = 0; j < INFORCE_ROWS; j++)
			scannerPins[i][j]->reset();
}


int ScannerApp::numScanned() {
	int num = 0;
	for (int i = 0; i < INFORCE_COLS; i++)
		for (int j = 0; j < INFORCE_ROWS; j++)
			num += scannerPins[i][j]->doneScanning;
	return num;
}


bool ScannerApp::doneScanned() {
	return numScanned() == 50;
}


string ScannerApp::appInstructionsText() {
	string instructions = (string) "" +
		"This is for testing things."
		"";

	return instructions;
}

void ScannerApp::writeToFile(string filename) {
	ofstream outputFile;
	outputFile.open(filename);

	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {

			int pointer = 0; // #### stores number of data points in scannedHeights and scannedForces
			int scannedHeights[1000]; // ####  at scannedHeights[i] it felt scannedForces[i]
			int scannedForces[1000]; // ####

			/* Structure of output file:

			<index=i*ROWS + j>
			<noObject>
			<lowerBoundary>
			<upperBoundary>
			<pointer>
			<scannedHeights[0]>
			<scannedForces[0]>
			<scannedHeights[1]>
			<scannedForces[1]>
			...
			<scannedHeights[pointer-1]>
			<scannedForces[pointer-1]>
			*/

			// <index>
			int index = i*INFORCE_ROWS + j;
			outputFile << index << "\n";

			// <noObject>
			outputFile << scannerPins[i][j]->noObject << "\n";

			// <lowerBoundary>
			outputFile << scannerPins[i][j]->lowerBoundary << "\n";

			// <upperBoundary>
			outputFile << scannerPins[i][j]->upperBoundary << "\n";

			// <pointer>
			outputFile << scannerPins[i][j]->pointer << "\n";

			// <scannedHeights[p]>
			// <scannedForces[p]>
			for (int p = 0; p < scannerPins[i][j]->pointer; p++) {
				outputFile << scannerPins[i][j]->scannedHeights[p] << "\n";
				outputFile << scannerPins[i][j]->scannedForces[p] << "\n";
			}
		}
	}
	outputFile.close();
	cout << "[Scanning App] Wrote to " << filename << endl;
}

void ScannerApp::keyPressed(int key) {
	cout << "keyPressed" << endl;

	if (key == 'w') {
		scan();
	}
	else if (key == 's') {
		showScan = !showScan;
	}
	else if (key == 'd') {
		writeToFile("foamScan.txt");
	}

	
}


void ScannerApp::drawApp() {
	cam.begin();

	ofPushMatrix();



	// Height Plane
	ofFill();
	ofSetColor(150, 150, 150, 150);
	int planeW = 100 + PIN_SPACING_MM*INFORCE_COLS;
	int planeH = 50 + PIN_SPACING_MM*INFORCE_ROWS;
	int planeD = DISPLAY_HEIGHT_MM;
	int planeX = planeW / 2 - 100 / 2;
	int planeY = planeH / 2 - 50 / 2;
	int planeZ = -planeD / 2;
	ofDrawBox(planeX, planeY, planeZ, planeW, planeH, planeD);




	// Draw the state of the display
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			float pinHeightMM = heightsFromShapeDisplay[i][j] / 1000.0;

			ofPushMatrix();

			// Move to the pin
			ofTranslate(PIN_SPACING_MM*INFORCE_COLS - (i + 1)*PIN_SPACING_MM, j*PIN_SPACING_MM, 0);

			float boxOriginX = PIN_SPACING_MM / 2.0;
			float boxOriginY = PIN_SPACING_MM / 2.0;
			float boxOriginZ = pinHeightMM - PIN_HEIGHT_MM / 2.0; // Draw pin body's under their height level

																  // Draw the pin body
			ofFill();

			if (scannerPins[i][j]->doneScanning) {
				int v = abs(scannerPins[i][j]->upperBoundary - scannerPins[i][j]->lowerBoundary);
				int c = 255 - ofMap(v, 0, 20000, 255,0);

				ofSetColor(c, c, c);
			}
			else {

				ofSetColor(250, 250, 250); // const ofColor PIN_BODY_COLOR(250, 250, 250);
			}
			
			
			ofDrawBox(boxOriginX, boxOriginY, boxOriginZ, PIN_WIDTH_MM, PIN_WIDTH_MM, PIN_HEIGHT_MM);


			// Draw the wireframe outline of each pin body (so we can differentiate them)
			ofNoFill();

			if (scannerPins[i][j]->scanningStiffness) {
				ofSetColor(255, 0, 0);
			}
			else if (scannerPins[i][j]->scanningUpperBoundary) {
				ofSetColor(255, 0, 255);
			}
			else if (scannerPins[i][j]->scanningLowerBoundary) {
				ofSetColor(0, 0, 255);
			}
			else if (scannerPins[i][j]->doneScanning) {
				ofSetColor(0, 255, 0);
			}
			else {
				ofSetColor(0, 0, 0); // const ofColor PIN_BODY_WIREFRAME_COLOR(0, 0, 0);
			}
			
			ofSetLineWidth(2);
			ofDrawBox(boxOriginX, boxOriginY, boxOriginZ, PIN_WIDTH_MM, PIN_WIDTH_MM, PIN_HEIGHT_MM);

		//	if (showForceArrow) {
				// Draw the pin force as a red transparent square on the top of the pin

				ofFill();
				ofSetColor(255, 0, 0);
				ofSetLineWidth(5);
				float pinForceMN = pins[i][j]->getForce(); //receivePinForce[i][j];
				float forceFrac = pinForceMN / PIN_FORCE_MAX_MN;
				float arrowLength = forceFrac * 50 * 2; //const float MAX_FORCE_ARROW_LENGTH = 50.0;


				//cout << arrowLength << endl;
				if (arrowLength > 0.2) {

					ofDrawArrow(ofVec3f(boxOriginX, boxOriginY, pinHeightMM + arrowLength), ofVec3f(boxOriginX, boxOriginY, pinHeightMM - 0.001), 2); 	//const float FORCE_ARROW_HEAD_SIZE = 2.0;

				}
			//}


			ofPopMatrix();

		}
	}

	//model->_currentMode->draw(this);

	ofPopMatrix();


	// TODO: Render a visualization of each command and it's data
	//    model->mode.draw();

	//ofSetColor(0);
	//mesh.drawWireframe();
	//mesh.drawFaces();

	cam.end();
}