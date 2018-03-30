#include "MaterialScannerApp.h"
#include <iostream>
#include <fstream>

MaterialScannerApp::MaterialScannerApp() {
	cout << "###### Application created: " << MaterialScannerApp::getName() << endl;
	
	// Create pin
	scannerPin = new PinScanner(scannerI, scannerJ, minHeight, maxHeight);

	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			heightsForShapeDisplay[i][j] = minHeight + 20000;
		}
	}

	dataSender.switchOperationMode(0);
}

void MaterialScannerApp::update(float dt) {
	dataSender.sendHeightData(heightsForShapeDisplay);

	time += dt;
	// give some time to calibrate
	if (time < 4) {
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
		for (int i = 0; i < INFORCE_COLS; i++)
			for (int j = 0; j < INFORCE_ROWS; j++)
				heightsForShapeDisplay[i][j] = minHeight + 20000;

		scannerPin->update(dt, heightsFromShapeDisplay[scannerI][scannerJ], pins[scannerI][scannerJ]->getForce());
		heightsForShapeDisplay[scannerI][scannerJ] = scannerPin->getHeightForShapeDisplay();

		if (scannerPin->doneScanning) {
			cout << "[Scanning App] Done scanning." << endl;
			scanning = false;
		}
	}
	else {
 		for (int i = 0; i < INFORCE_COLS; i++)
			for (int j = 0; j < INFORCE_ROWS; j++)
				heightsForShapeDisplay[i][j] = minHeight + 20000;
		heightsForShapeDisplay[scannerI][scannerJ] = minHeight;

	}
}


void MaterialScannerApp::scan() {
	scanning = true;
	resetScan();
	scannerPin->verbose = true;
	scannerPin->pauseTime = 0.5;
	scannerPin->scanUpperBoundary();
}


void MaterialScannerApp::resetScan() {
	scannerPin->reset();
}



string MaterialScannerApp::appInstructionsText() {
	string instructions = (string) "" +
		"This is for testing things."
		"";

	return instructions;
}

void MaterialScannerApp::writeToFile(string filename) {
	ofstream outputFile;
	outputFile.open(filename);

	// <lowerBoundary> / <maxHeight>
	outputFile << (minHeight + (maxHeight - scannerPin->lowerBoundary)) << "\n";

	// <upperBoundary> / <minHeight>
	outputFile << (minHeight + (maxHeight - scannerPin->upperBoundary)) << "\n";

	// <pointer> / <numMappings>
	outputFile << scannerPin->pointer << "\n";

	// <scannedHeights[p]> / <heightMapping[p]>
	// <scannedForces[p]> / <forceMapping[p]>
	for (int p = 0; p < scannerPin->pointer; p++) {
		outputFile << (minHeight + (maxHeight - scannerPin->scannedHeights[p])) << "\n";
		outputFile << scannerPin->scannedForces[p] << "\n";
	}

	outputFile.close();
	cout << "[Scanning App] Wrote to " << filename << endl;
}

void MaterialScannerApp::drawApp() {


}

void MaterialScannerApp::keyPressed(int key) {
	cout << "keyPressed" << endl;

	if (key == 'w') {
		cout << "Begin scanning." << endl;
		scan();
	}
	else if (key == 'd') {
		writeToFile("foamScan11.txt");
	}
	else if (key == 'g') {
		writeToFile("foamScan12.txt");
	}
	else if (key == 'h') {
		writeToFile("foamScan13.txt");
	}
	else if (key == 'j') {
		writeToFile("foamScan14.txt");
	}
	else if (key == 'k') {
		writeToFile("foamScan15.txt");
	}
	else if (key == 'l') {
		writeToFile("foamScan16.txt");
	} 
	else if (key == 'l') {
		writeToFile("foamScan16.txt");
	}
	else if (key == ';') {
		writeToFile("foamScan17.txt");
	}
	else if (key == ',') {
		writeToFile("foamScan18.txt");
	}

	
}
