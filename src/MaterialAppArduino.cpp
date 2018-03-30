#include "MaterialAppArduino.h"
#include <iostream>
#include <fstream>

MaterialAppArduino::MaterialAppArduino() {
	dataSender.switchOperationMode(3);
	//dataSender.sendPinCalibrationParameter();
	sendControlParameters(3.0, 450, 50, 1000, 0.25, 0.9);


	graphMaterialFbo.allocate(800, 600);
	if (graphMaterialFbo.isAllocated()) {
		cout << "app drawing" << endl;

		graphMaterialFbo.begin();
		//graphMaterialFbo.clear();
		ofClear(255);
		ofFill();
		ofSetColor(255);
		ofRect(0, 0, graphMaterialFbo.getWidth(), graphMaterialFbo.getHeight());
		graphMaterialFbo.end();

	}
}

void MaterialAppArduino::sendHeightDependentForceParameters(int maxHeight, int minHeight, int heightMapping[], int forceMapping[], int numMappings) {
	/*
	Maps height to target force for a given pin. Same as GradientOp, but every pin has the same forces. This is dependent on the calibration data stored in heightMapping and forceMapping.
	heightMapping is a list of heights of size=numMappings, satisfying heightMapping[0] > heightMapping[1] > ... > heightMapping[numMappings-1]

	The height force mapping is as follows:
	if height >= heightMapping[0], force = forceMapping[0]; similarly, if height <= heightMapping[numMappings-1], force = forceMapping[numMappings-1]
	at height = heightMapping[i], force = forceMapping[i]
	if height is in between heightMapping[i] and heightMapping[i+1], force = linear interpolation of forceMapping[i] and forceMapping[i+1]
	
	Example:
	maxHeight = 80000;
	minHeight = 10000;
	numMappings = 2;
	heightMapping[0] = 80000;
	forceMapping[0] = 65;
	heightMapping[1] = 10000;
	forceMapping[1] = 2000;
	*/


	int fileData[MAX_OFXOSCMESSAGE_LENGTH];
	fileData[0] = 0; // Indicating we sent force parameters
	fileData[1] = maxHeight;
	fileData[2] = minHeight;
	fileData[3] = numMappings;

	int pData = 4;
	for (int i = 0; i < numMappings; i++) {
		

		fileData[pData] = heightMapping[i];
		pData++;
		fileData[pData] = forceMapping[i];
		pData++;
		
	}
	dataSender.sendAllSameOperationParameter(fileData, pData);


}

void MaterialAppArduino::sendControlParameters(float variance = 3.0, float kp = 450, float kd = 50, int speedGoingUp = 1000, float kd2_soft = 0.25, float kd2_hard = 0.9) {
	int fileData[MAX_OFXOSCMESSAGE_LENGTH];
	fileData[0] = 1; // Indicating we sent force parameters
	fileData[1] = int(variance*1000);
	fileData[2] = int(kp * 1000);
	fileData[3] = int(kd * 1000);
	fileData[4] = int(speedGoingUp * 1000);
	fileData[5] = int(kd2_soft * 1000);
	fileData[6] = int(kd2_hard * 1000);

	dataSender.sendAllSameOperationParameter(fileData, 7);
}


void MaterialAppArduino::readAndSendHeightDependentForceParametersFromFile(string filename) {
	string line;
	ifstream myfile(filename);

	if (myfile.is_open())
	{

		cout << "sending value from filename = " << filename << endl;



		getline(myfile, line);
		int _maxHeight = std::stoi(line);
		//int _maxHeight = 40000;// std::stoi(line);
		//int _trueMaxHeight = std::stoi(line);

		getline(myfile, line);
		int _minHeight = std::stoi(line); // -(_trueMaxHeight - _maxHeight);
		//int _minHeight = std::stoi(line) - (_trueMaxHeight - _maxHeight);

		getline(myfile, line);
		int _numMappings = std::stoi(line);

		int _heightMapping[1000];
		int _forceMapping[1000];

		for (int p = 0; p < _numMappings; p++) {
			getline(myfile, line);
			_heightMapping[p] = std::stoi(line);
			//_heightMapping[p] = std::stoi(line) - (_trueMaxHeight - _maxHeight);

			getline(myfile, line);
			_forceMapping[p] = std::stoi(line);

		}

		sendHeightDependentForceParameters(_maxHeight, _minHeight, _heightMapping, _forceMapping, _numMappings);
		myfile.close();


		graphMaterialFbo.begin();
		ofDisableDepthTest();
		//ofSmooth();
		//ofClear(255);
		ofFill();
		ofSetColor(255,255,255);
		//ofRect(0, 0, graphMaterialFbo.getWidth(), graphMaterialFbo.getHeight());

		ofSetColor(0);
		ofNoFill();
		ofSetLineWidth(2);
		ofDrawLine(0, graphMaterialFbo.getHeight()-10, graphMaterialFbo.getWidth(), graphMaterialFbo.getHeight()-10);
		ofDrawLine( 10, 0, 10, graphMaterialFbo.getHeight());

		ofSetLineWidth(1);
		ofSetColor(100);
		for (int i = 10000; i < 80000; i += 3000) {
			int h = ofMap(i + 1500, 10000, 80000, graphMaterialFbo.getHeight() - 10, graphMaterialFbo.getHeight() - 510);
			ofDrawLine(0, h, graphMaterialFbo.getWidth(), h);
		}

		ofSetColor(0);
		ofDrawBitmapString("height  = 80000", 10, graphMaterialFbo.getHeight() - 510 );
		ofDrawBitmapString("force  = 1200", 610, graphMaterialFbo.getHeight() - 10);



		
		int r = ofRandom(0, 255);
		int g = ofRandom(0, 255);
		int b = ofRandom(0, 255);
		
		ofSetLineWidth(1);
		for (int p = 0; p < _numMappings; p++) {

			int y = ofMap(_heightMapping[p], 10000, 80000, graphMaterialFbo.getHeight() - 10, graphMaterialFbo.getHeight() - 510);
			int x = ofMap(_forceMapping[p], 0, 1200, 10, 610);
			ofFill();
			ofSetColor(r, g, b);
			ofDrawEllipse(x, y,5,5);

			ofNoFill();
			ofSetColor(0);
			ofDrawEllipse(x, y, 5, 5);

		}
		graphMaterialFbo.end();
	}
}


void MaterialAppArduino::update(float dt) {
	updateOscSend();
	/*
	graphMaterialFbo.begin();
	//graphMaterialFbo.clear();
	ofClear(255);
	ofFill();
	ofSetColor(255);
	ofRect(0, 0, graphMaterialFbo.getWidth(), graphMaterialFbo.getHeight());
	graphMaterialFbo.end();*/
}

void MaterialAppArduino::drawApp() {
	if (graphMaterialFbo.isAllocated()) {
		cout << "app drawing" << endl;
	}
	ofSetColor(255);

	
	graphMaterialFbo.draw(200,100);

	ofFill();
	ofSetColor(255);
	//ofRect(0, 0, graphMaterialFbo.getWidth(), graphMaterialFbo.getHeight());

}

void MaterialAppArduino::updateOscSend() {
	dataSender.sendBackAllPinHeight();
}

string MaterialAppArduino::appInstructionsText() {
	string instructions = (string) "" +
		"This is for testing things."
		"";

	return instructions;
}
	
void MaterialAppArduino::keyPressed(int key) {
	cout << "keyPressed" << endl;

	if (key == 'w') {
		variance *= 1.2;
		sendControlParameters(variance = variance);
		cout << "Variance: " << variance << endl;
	}
	else if (key == 's') {
		variance /= 1.2;
		sendControlParameters(variance = variance);
		cout << "Variance: " << variance << endl;
	}
	else if (key == 'd') {
		readAndSendHeightDependentForceParametersFromFile("foamScan11.txt"); // hard foam
		sendControlParameters(2.0, 450, 50, 1000, 0.25, 0.9);

	}
	else if (key == 'f') {
		readAndSendHeightDependentForceParametersFromFile("foamScan12.txt"); // soft foam
		sendControlParameters(2.0, 450, 50, 1000, 0.25, 0.9);
	}
	else if (key == 'e') {
		readAndSendHeightDependentForceParametersFromFile("foamScan13.txt"); // softer foam
		sendControlParameters(4.0, 450, 50, 1000, 0.25, 0.9);
	}
	else if (key == 'r') {
		readAndSendHeightDependentForceParametersFromFile("foamScan14.txt"); // soft memory foam
		sendControlParameters(4.0, 450, 50, 1000, 0.25, 0.9);

	}
	else if (key == 't') {
		readAndSendHeightDependentForceParametersFromFile("foamScan15.txt"); // softer memory foam
		sendControlParameters(3.0, 250, 50, 1000, 0.25, 0.9);
	}

	else if (key == 'y') {
		readAndSendHeightDependentForceParametersFromFile("foamScan16.txt"); // hard soft foam
		sendControlParameters(4.0, 450, 50, 1000, 0.25, 0.9);

		//sendHeightDependentForceParameters(int maxHeight, int minHeight, int heightMapping[], int forceMapping[], int numMappings);
	}
	else if (key == 'u') {
		readAndSendHeightDependentForceParametersFromFile("foamScan17.txt"); // hard foam
		sendControlParameters(2.0, 450, 50, 1000, 0.25, 0.9);

	}
	else if (key == 'i') {
		readAndSendHeightDependentForceParametersFromFile("foamScan18.txt"); // soft foam
		sendControlParameters(2.0, 450, 50, 1000, 0.25, 0.9);
	}
	else if (key == 'o') {
		readAndSendHeightDependentForceParametersFromFile("foamScan7.txt"); // softer foam
		sendControlParameters(4.0, 450, 50, 1000, 0.25, 0.9);
	}
	else if (key == 'p') {
		readAndSendHeightDependentForceParametersFromFile("foamScan8.txt"); // soft memory foam
		sendControlParameters(4.0, 450, 50, 1000, 0.25, 0.9);

	}
	else if (key == '[') {
		readAndSendHeightDependentForceParametersFromFile("foamScan9.txt"); // softer memory foam
		sendControlParameters(3.0, 250, 50, 1000, 0.25, 0.9);
	}

	else if (key == ']') {
		readAndSendHeightDependentForceParametersFromFile("foamScan10.txt"); // hard soft foam
		sendControlParameters(4.0, 450, 50, 1000, 0.25, 0.9);

		//sendHeightDependentForceParameters(int maxHeight, int minHeight, int heightMapping[], int forceMapping[], int numMappings);
	}


}