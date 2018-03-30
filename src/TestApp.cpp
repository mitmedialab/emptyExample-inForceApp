#include "TestApp.h"
#include <iostream>
#include <fstream>

TestApp::TestApp() {
	cout << "###### Application created: " << TestApp::getName() << endl;
	dataSender.switchOperationMode(1);
}

void TestApp::update(float dt) {
	if (dx > 0)
		x += dt*dx;
	else
		x += dt*dx;
	y += dt*dy;
	if (x > INFORCE_COLS) {
		x = INFORCE_COLS;
		dx = -abs(dx);
	}
	if (x < 0) {
		x = 0;
		dx = abs(dx);
	}
	if (y > INFORCE_ROWS) {
		y = INFORCE_ROWS;
		dy = -abs(dy);
	}
	if (y < 0) {
		y = 0;
		dy = abs(dy);
	}
	updateHeights2();


	updateOscSend();
}
void TestApp::updateOscSend() {
	dataSender.sendBackAllPinHeight();
}

void TestApp::updateHeights() {
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			float distanceSquared = (i - x)*(i - x) + (j - y)*(j - y);
			float height = exp(-distanceSquared / variance) * (maxHeight - minHeight) + minHeight;

			heightsForShapeDisplay[i][j] = height;
		}
	}
}

void TestApp::updateHeights2() {
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			heightsForShapeDisplay[i][j] = float(x) / INFORCE_COLS*maxHeight;
		}
	}
}



void TestApp::updateHeights3() {
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			heightsForShapeDisplay[i][j] = setHeight;
		}
	}

}

void TestApp::updateHeights4() {

	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			heightsForShapeDisplay[i][j] = float(x) / INFORCE_COLS*maxHeight;
		}
	}
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			if (i != 0 || j != 0) {
				float distanceSquared = (i - x)*(i - x) + (j - y)*(j - y);
				float height = exp(-distanceSquared / variance) * (maxHeight - minHeight) + minHeight;
				heightsForShapeDisplay[i][j] = height;
			}
		}
	}
}

void TestApp::updateHeights5(int i, int j) {
	heightsForShapeDisplay[i][j] = float(x) / INFORCE_COLS*maxHeight;
}


string TestApp::appInstructionsText() {
	string instructions = (string) "" +
		"This is for testing things."
		"";

	return instructions;
}

void TestApp::keyPressed(int key) {
	cout << "keyPressed " << key << endl;

	if (key == 'w') {
		variance *= 1.1;
	}
	else if (key == 's') {
		variance /= 1.1;
	}
	else if (key == 'a') {
		dx /= 1.1;
		dy /= 1.1;
	}
	else if (key == 'd') {
		dx *= 1.1;
		dy *= 1.1;
	}
	else if (key == 'q') {
		dx *= -1;
	}
	/*else if (key == 'e') {
		for (int i = 0; i < INFORCE_COLS; i++) {
			for (int j = 0; j < INFORCE_ROWS; j++) {
				pins[i][j]->window -= 1;
			}
		}
	}
	else if (key == 'r') {
		for (int i = 0; i < INFORCE_COLS; i++) {
			for (int j = 0; j < INFORCE_ROWS; j++) {
				pins[i][j]->window += 1;
			}
		}
	}
	else if (key == 't') {
		for (int i = 0; i < INFORCE_COLS; i++) {
			for (int j = 0; j < INFORCE_ROWS; j++) {
				pins[i][j]->delay -= 1;
			}
		}
	}
	else if (key == 'y') {
		for (int i = 0; i < INFORCE_COLS; i++) {
			for (int j = 0; j < INFORCE_ROWS; j++) {
				pins[i][j]->delay += 1;
			}
		}
	}*/
	else if (key == 'k') {
		ofstream outputFile;
		outputFile.open("data 1-30-2018.txt");

		int ii = 0;
		int jj = 2;

		outputFile << "forceFromShapeDisplayHistory\n";
		for (int i = 0; i < 400; i++)
			outputFile << pins[ii][jj]->forceFromShapeDisplayHistory[i] << "\n";

		outputFile << "heightForShapeDisplayHistory\n";
		for (int i = 0; i < 400; i++)
			outputFile << pins[ii][jj]->heightForShapeDisplayHistory[i] << "\n";

		outputFile << "heightFromShapeDisplayHistory\n";
		for (int i = 0; i < 400; i++)
			outputFile << pins[ii][jj]->heightFromShapeDisplayHistory[i] << "\n";

		outputFile.close();
	}


}