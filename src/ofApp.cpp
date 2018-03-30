#include "ofApp.h"

#include "TestApp.h"
#include "CalibrationApp.h"

#include "MaterialApp.h"
#include "LayerApp.h"
#include "MaterialVolumeApp.h"
#include "IntouchApp.h"
#include "MirrorApp.h"
#include "JugglingApp.h"
#include "ScannerApp.h"
#include "ScannerDisplayApp.h"

#include "MaterialAppArduino.h"
#include "ScannerDisplayAppArduino.h"
#include "WaterAppArduino.h"
#include "PianoAppArduino.h"

#include "LayerApp2.h"
#include "MaterialScannerApp.h"

#include "MaterialAppPID.h"
#include "MaterialApp3.h"
#include "MaterialApp2.h"
#include "GradientApp.h"
#include "MountainApp.h"
#include "DataCollectionApp.h"
#include "MaterialFuncApp.h"
#include "ProjectionApp.h"

void ofApp::setup(){

	receiver.setup(IN_PORT);
	sender.setup("localhost", OUT_PORT);

	ofSetFrameRate(COMMAND_FREQUENCY);

	modeText.loadFont("LeelaUIb.ttf", 20);

	ofEnableDepthTest();

	//ofEnableNormalizedTexCoords();
	
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {

//			val_stiffness[i][j] = ofMap(INFORCE_COLS - i, 0, INFORCE_COLS, MIN_STIFFNESS, MAX_STIFFNESS);
//			val_maxHeight[i][j] = 80000;
//			val_minHeight[i][j] = val_maxHeight[i][j] - ofMap(INFORCE_COLS - i, 0, INFORCE_COLS, 20000, 5000);
//			sendPinHeight[i][j] = val_maxHeight[i][j];
			sendPinFrequency[i][j] = 0;
			p_sendPinFrequency[i][j] = 0;
		}
	}
	


	// For user study

	//currentApplication = new LayerApp2();
	//currentApplication = new MaterialAppArduino();
	currentApplication = new MaterialScannerApp();

	//currentApplication = new TestApp();
	//currentApplication = new CalibrationApp();

	//currentApplication = new MaterialApp();
    //currentApplication = new LayerApp();
	//currentApplication = new MaterialVolumeApp();
	//currentApplication = new IntouchApp();
	//currentApplication = new MirrorApp();
	//currentApplication = new GradientApp();
	//currentApplication = new MountainApp();
	//currentApplication = new JugglingApp();
	//currentApplication = new ScannerApp();
	//currentApplication = new ScannerDisplayApp();
	//currentApplication = new ProjectionApp();
	//currentApplication = new ScannerDisplayAppArduino();
	//currentApplication = new WaterAppArduino();
	//currentApplication = new PianoAppArduino();


	//currentApplication = new MaterialApp3();
	//currentApplication = new MaterialAppPID();
	//currentApplication = new MaterialApp2();
	//currentApplication = new DataCollectionApp();




	currentDemo = "Gradient"; // Gradient, Map, Graph, Medical, Animal?

	stiffnessGradient.loadImage("figure/stiffness-gradient-01.png");

	int width = 100 + PIN_SPACING_MM*INFORCE_COLS;
	int height = 50 + PIN_SPACING_MM*INFORCE_ROWS;


	//add one vertex to the mesh for each pixel
	for (int y = 0; y < height; y++) {
		for (int x = 0; x<width ; x++) {
			mesh.addVertex(ofPoint(x, y, 0));	// mesh index = x + y*width
													// this replicates the pixel array within the camera bitmap...
			mesh.addColor(ofFloatColor(x/width*255, 0, 0, 0));  // placeholder for colour data, we'll get this from the camera
		}
	}

	for (int y = 0; y<height - 1; y++) {
		for (int x = 0; x<width - 1; x++) {
			mesh.addIndex(x + y*width);				// 0
			mesh.addIndex((x + 1) + y*width);			// 1
			mesh.addIndex(x + (y + 1)*width);			// 10

			mesh.addIndex((x + 1) + y*width);			// 1
			mesh.addIndex((x + 1) + (y + 1)*width);		// 11
			mesh.addIndex(x + (y + 1)*width);			// 10
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




	ofxOscMessage mSendV;
	//cout << "new data" << endl;
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			mSendV.addFloatArg(0);
			//cout << sendPinHeight[i][j] << ", ";
		}
	}
	//cout << endl;

	mSendV.setAddress("/inForceVibrate");
	sender.sendMessage(mSendV, false);

}


void ofApp::update(){
	//hide old messages
	for (int i = 0; i < NUM_MSG_STRINGS; i++) {
		if (timers[i] < ofGetElapsedTimef()) {
			msg_strings[i] = "";
		}
	}

	//check for waiting messages
	while (receiver.hasWaitingMessages()) {
		ofxOscMessage mReceive;
		receiver.getNextMessage(mReceive);

		if (mReceive.getAddress() == "/inForce") {
			// Save the pin height data to the model
			//cout << "new data" << endl;
			for (int i = 0; i < INFORCE_COLS; i++) {
				for (int j = 0; j < INFORCE_ROWS; j++) {
						receivePinHeight[i][j] = mReceive.getArgAsInt32(2*(i*INFORCE_ROWS + j));
						receivePinForce[i][j] = mReceive.getArgAsInt32(2*(i*INFORCE_ROWS + j) + 1);
						//receivePinTouch[i][j] = mReceive.getArgAsString(3*(i*INFORCE_ROWS + j) + 2);
				}
			}
		}
	}

	// Compute sendPinHeight

	float currentTime = clock() / (double)CLOCKS_PER_SEC;
	double dt = currentTime - timeOfLastUpdate;
	timeOfLastUpdate = currentTime;

	if (!paused) {

		//cout << currentTime << endl;

		currentApplication->setHeightsFromShapeDisplayRef(receivePinHeight);
		currentApplication->setForceFromShapeDisplayRef(receivePinForce);
		currentApplication->setTouchFromShapeDisplayRef(receivePinTouch);
		currentApplication->updatePins(dt);

		currentApplication->update(dt);
		currentApplication->getHeightsForShapeDisplay(sendPinHeight);
		currentApplication->getFrequencyForShapeDisplay(sendPinFrequency);

		if (UseNewDataSendingMethod) {
//			currentApplication->updateOscSend();
		}

		//currentApplication->setStiffnessForDisplay(val_stiffness);
		//currentApplication->setMaxHeightForDisplay(val_maxHeight);
		//currentApplication->setMinHeightForDisplay(val_minHeight);
	}



	bool isFrequencyUpdated = false;
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			if (sendPinFrequency[i][j] == p_sendPinFrequency[i][j]) {
				isFrequencyUpdated = true;
			}
			p_sendPinFrequency[i][j] = sendPinFrequency[i][j];
		}
	}

	if (isFrequencyUpdated) {
		ofxOscMessage mSendV;
		//cout << "new data" << endl;
		for (int i = 0; i < INFORCE_COLS; i++) {
			for (int j = 0; j < INFORCE_ROWS; j++) {
				mSendV.addFloatArg(sendPinFrequency[i][j]);
				//cout << sendPinHeight[i][j] << ", ";
			}
		}
		//cout << endl;

		mSendV.setAddress("/inForceVibrate");
		sender.sendMessage(mSendV, false);


	}


	// Send pin height to shape display

	if (!UseNewDataSendingMethod) {
		ofxOscMessage mSend;
		for (int i = 0; i < INFORCE_COLS; i++) {
			for (int j = 0; j < INFORCE_ROWS; j++) {
				mSend.addInt32Arg(sendPinHeight[i][j]);
			}
		}

		mSend.setAddress("/inForce");
		sender.sendMessage(mSend, false);
	}





}


void ofApp::draw() {
	ofBackgroundGradient(230, 150, OF_GRADIENT_CIRCULAR);
	//ofBackground(200);
	ofEnableDepthTest();
	
	ofPushMatrix();
	ofTranslate(200, 50);
	ofSetColor(0);
	modeText.drawString(currentApplication->getName(), 0, 0);
	
	//ofDrawBitmapString(currentApplication->getName(), 0, 0);
	ofPopMatrix();

	if (showStiffness) {
		ofSetColor(255,255,255);
		ofEnableAlphaBlending();
		//cout << stiffnessGradient.type << endl;
		int w = stiffnessGradient.getWidth() / 2;
		int h = stiffnessGradient.getHeight() / 2;
		int x = ofGetWidth() - w - 50;
		int y = 50;
		stiffnessGradient.draw(x, y, w, h);
	}

	ofSetColor(0, 0, 0);
	ofDrawBitmapString(ofToString(ofGetFrameRate()), 100, 100);


	
	

	if (showGrid) {
	//receiveHeightPin
	ofPushMatrix();
	ofTranslate(gridPosX, gridPosY);
	ofSetColor(0);
	ofSetLineWidth(2);
	ofDrawBitmapString("receivePinHeight", 0, -5);
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {

			if (receivePinHeight[i][j] >= 0) {
				int c = ofMap(receivePinHeight[i][j], 0, 100000, 0, 255);
				ofSetColor(255 - c, 255, 255 - c);
			}
			else {
				int c = ofMap(-receivePinHeight[i][j], 0, 100000, 0, 255);
				ofSetColor(255, 255 - c, 255 - c);
			}
			ofFill();
			ofRect(gridSize*(INFORCE_COLS - i - 1), gridSize*(INFORCE_ROWS - j - 1), gridSize, gridSize);


			ofNoFill();
			ofSetColor(100, 100, 100);
			ofRect(gridSize*(INFORCE_COLS - i - 1), gridSize*(INFORCE_ROWS - j - 1), gridSize, gridSize);

			ofSetColor(0);
			ofDrawBitmapString(ofToString(int(receivePinHeight[i][j]/1000)),gridSize*(INFORCE_COLS - i - 1), (gridSize)*(INFORCE_ROWS - j ));
		}
	}

	ofPopMatrix();


	//sendHeightPin
	ofPushMatrix();
	ofTranslate(gridPosX, gridPosY + gridSpaceY);
	ofSetColor(0);
	ofDrawBitmapString("sendHeightPin", 0, -5);
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {

			if (sendPinHeight[i][j] >= 0) {
				int c = ofMap(sendPinHeight[i][j], 0, 100000, 0, 255);
				ofSetColor(255 - c, 255, 255 - c);
			}
			else {
				int c = ofMap(-sendPinHeight[i][j], 0, 100000, 0, 255);
				ofSetColor(255, 255 - c, 255 - c);
			}
			ofFill();
			ofRect(gridSize*(INFORCE_COLS - i - 1), gridSize*(INFORCE_ROWS - j - 1), gridSize, gridSize);

			ofNoFill();
			ofSetColor(100, 100, 100);
			ofRect(gridSize*(INFORCE_COLS - i - 1), gridSize*(INFORCE_ROWS - j - 1), gridSize, gridSize);
		}
	}

	ofPopMatrix();



	//differenceHeightPin
	ofPushMatrix();
	ofTranslate(gridPosX, gridPosY + gridSpaceY * 2);
	ofSetColor(0);
	ofDrawBitmapString("differenceHeightPin", 0, -5);
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			int diff = sendPinHeight[i][j] - receivePinHeight[i][j];
			if (diff >= 0) {
				int c = ofMap(diff, 0, 10000, 0, 255);
				ofSetColor(255 - c, 255, 255 - c);
			}
			else {
				int c = ofMap(-diff, 0, 10000, 0, 255);
				ofSetColor(255, 255 - c, 255 - c);
			}
			ofFill();
			ofRect(gridSize*(INFORCE_COLS - i - 1), gridSize*(INFORCE_ROWS - j - 1), gridSize, gridSize);

			ofNoFill();
			ofSetColor(100, 100, 100);
			ofRect(gridSize*(INFORCE_COLS - i - 1), gridSize*(INFORCE_ROWS - j - 1), gridSize, gridSize);
		}
	}

	ofPopMatrix();


	//receivepinforce

	ofPushMatrix();
	ofTranslate(gridPosX, gridPosY + gridSpaceY * 3);
	ofSetColor(0);
	ofDrawBitmapString("receivePinForce", 0, -5);
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			int c = ofMap(receivePinForce[i][j], 0, 2500, 0, 255);
			ofSetColor(255, 255 - c, 255 - c);

			ofFill();
			ofRect(gridSize*(INFORCE_COLS - i - 1), gridSize*(INFORCE_ROWS - j - 1), gridSize, gridSize);

			ofNoFill();
			ofSetColor(100, 100, 100);
			ofRect(gridSize*(INFORCE_COLS - i - 1), gridSize*(INFORCE_ROWS - j - 1), gridSize, gridSize);
		}
	}

	ofPopMatrix();

	// estimated force

	// input value name & range of input

	ofPushMatrix();
	ofTranslate(gridPosX, gridPosY + gridSpaceY * 4);
	ofSetColor(0);
	ofDrawBitmapString("Estimated Force", 0, -5);
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			float val = currentApplication->pins[i][j]->getForce();
			int c = ofMap(val, 0, 1500, 0, 255);
			ofSetColor(255, 255 - c, 255 - c);

			ofFill();
			ofRect(gridSize*(INFORCE_COLS - i - 1), gridSize*(INFORCE_ROWS - j - 1), gridSize, gridSize);

			ofNoFill();
			ofSetColor(100, 100, 100);
			ofRect(gridSize*(INFORCE_COLS - i - 1), gridSize*(INFORCE_ROWS - j - 1), gridSize, gridSize);
		}
	}

	ofPopMatrix();

	// Target Force

	ofPushMatrix();
	ofTranslate(gridPosX, gridPosY + gridSpaceY * 5);
	ofSetColor(0);
	ofDrawBitmapString("Target Force", 0, -5);
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			float val = currentApplication->getTargetForce(i,j);
			int c = ofMap(val, 0, 1000, 0, 255);
			ofSetColor(255, 255 - c, 255 - c);

			ofFill();
			ofRect(gridSize*(INFORCE_COLS - i - 1), gridSize*(INFORCE_ROWS - j - 1), gridSize, gridSize);

			ofNoFill();
			ofSetColor(100, 100, 100);
			ofRect(gridSize*(INFORCE_COLS - i - 1), gridSize*(INFORCE_ROWS - j - 1), gridSize, gridSize);
		}
	}
	ofPopMatrix();


	// frequency

	ofPushMatrix();
	ofTranslate(gridPosX, gridPosY + gridSpaceY * 6);
	ofSetColor(0);
	ofDrawBitmapString("Send Frequency", 0, -5);
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			float val = sendPinFrequency[i][j];
			int c = ofMap(val, 0, 30, 0, 255);
			ofSetColor(255, 255 - c, 255 - c);

			ofFill();
			ofRect(gridSize*(INFORCE_COLS - i - 1), gridSize*(INFORCE_ROWS - j - 1), gridSize, gridSize);

			ofNoFill();
			ofSetColor(100, 100, 100);
			ofRect(gridSize*(INFORCE_COLS - i - 1), gridSize*(INFORCE_ROWS - j - 1), gridSize, gridSize);
		}
	}

	ofPopMatrix();
}


	if (showLineChart) {
		int numDataPoints = 100;

		// line chart
		ofPushMatrix();
		ofTranslate(210, 20);
		int pinDrawI = pinDrawIndex / INFORCE_ROWS;
		int pinDrawJ = pinDrawIndex % INFORCE_ROWS;
		ofSetColor(0, 0, 0);
		ofDrawBitmapString("force graphs, pin[" + to_string(pinDrawI) + "][" + to_string(pinDrawJ) + "]", 0, -5);

		float graphWidth = 600;
		float graphHeight = 200;
		ofFill();
		ofSetColor(210);
		ofTranslate(0, 0, -2);
		ofRect(0, 0, graphWidth, graphHeight);
		ofTranslate(0, 0, 2);

		ofNoFill();
		PinModel* pinDraw = currentApplication->pins[pinDrawI][pinDrawJ];
		float maxForce = 1500;
		

		// axis labels
		ofTranslate(0, 0, -1);
		ofSetColor(180, 180, 180);
		for (int i = 250; i <= 1250; i += 250)
			ofLine(1, graphHeight - graphHeight * i / maxForce, graphWidth, graphHeight - graphHeight * i / maxForce);
		
		ofSetColor(40, 40, 40);
		for (int i = 250; i <= 1250; i += 250)
			ofDrawBitmapString(to_string(i), graphWidth + 3, graphHeight - graphHeight * i / maxForce + 3);
		ofTranslate(0, 0, 1);

		// lines
		
		for (int i = 0; i < numDataPoints - 1; i++) {
			float x1 = graphWidth - i*graphWidth / numDataPoints;
			float x2 = graphWidth - (i + 1)*graphWidth / numDataPoints;

			float y1, y2;

			y1 = graphHeight - graphHeight*pinDraw->smoothCurrentHistory[i] / maxForce;
			y2 = graphHeight - graphHeight*pinDraw->smoothCurrentHistory[i + 1] / maxForce;
			ofSetColor(175, 29, 29);
			ofLine(x1, y1, x2, y2);

			y1 = graphHeight - graphHeight*pinDraw->forceFromShapeDisplayHistory[i] / maxForce;
			y2 = graphHeight - graphHeight*pinDraw->forceFromShapeDisplayHistory[i + 1] / maxForce;
			ofSetColor(216, 128, 128);
			ofLine(x1, y1, x2, y2);

			y1 = graphHeight - graphHeight*pinDraw->movementForceHistory[i] / maxForce;
			y2 = graphHeight - graphHeight*pinDraw->movementForceHistory[i + 1] / maxForce;
			ofSetColor(8, 114, 50);
			ofLine(x1, y1, x2, y2);

			y1 = graphHeight - graphHeight*(pinDraw->unsmoothedForceHistory[i]) / maxForce;
			y2 = graphHeight - graphHeight*(pinDraw->unsmoothedForceHistory[i + 1]) / maxForce;
			ofSetColor(99, 131, 181);
			ofLine(x1, y1, x2, y2);

			y1 = graphHeight - graphHeight*(pinDraw->forceHistory[i]) / maxForce;
			y2 = graphHeight - graphHeight*(pinDraw->forceHistory[i + 1]) / maxForce;
			ofSetColor(55, 92, 150);
			ofTranslate(0, 0, 1);
			ofLine(x1, y1, x2, y2);
			ofTranslate(0, 0, -1);
		}

		ofSetColor(0, 0, 0);
		ofDrawBitmapString("rawForce (r): " + to_string(int(pinDraw->forceFromShapeDisplayHistory[0])), graphWidth + 50, 0);
		ofDrawBitmapString("smoothedCurrent (r): " + to_string(int(pinDraw->forceFromShapeDisplayHistory[0])), graphWidth + 50, 15);
		ofDrawBitmapString("estMovementForce (g): " + to_string(int(pinDraw->movementForceHistory[0])), graphWidth + 50, 30);
		ofDrawBitmapString("unsmoothedForce (b): " + to_string(int(pinDraw->unsmoothedForceHistory[0])), graphWidth + 50, 45);
		ofDrawBitmapString("smoothedForce (b): " + to_string(int(pinDraw->forceHistory[0])), graphWidth + 50, 60);
		ofDrawBitmapString("restMean: " + to_string(int(pinDraw->restMean)), graphWidth + 50, 75);
		ofDrawBitmapString("restMeanAdjusted: " + to_string(int(pinDraw->restMeanAdjusted)), graphWidth + 50, 90);

		// phantom line chart

		for (int i = 0; i < numDataPoints - 1; i++) {
			float x1 = graphWidth - i*graphWidth / numDataPoints;
			float y1 = graphHeight - graphHeight*pinDraw->forceFromShapeDisplayHistory[i+phantomGraphOffset] / maxForce;
			float x2 = graphWidth - (i + 1)*graphWidth / numDataPoints;
			float y2 = graphHeight - graphHeight*pinDraw->forceFromShapeDisplayHistory[i+phantomGraphOffset + 1] / maxForce;
			ofSetColor(201, 84, 84);
			ofLine(x1, y1, x2, y2);
		}

		ofPopMatrix();

		// line chart
		ofPushMatrix();
		ofTranslate(200, 500);
		ofSetColor(0, 0, 0);
		ofDrawBitmapString("height graphs, pin[" + to_string(pinDrawI) + "][" + to_string(pinDrawJ) + "]", 0, -5);

		graphWidth = 600;
		graphHeight = 200;
		ofFill();
		ofSetColor(210);
		ofTranslate(0, 0, -1);
		ofRect(0, 0, graphWidth, graphHeight);
		ofTranslate(0, 0, 1);

		ofNoFill();

		for (int i = 99; i > 0; i--)
			velocityHistory[i] = velocityHistory[i - 1];
		velocityHistory[0] = currentApplication->pinVelocity[pinDrawI][pinDrawJ];

		for (int i = 0; i < numDataPoints - 1; i++) {
			float x1 = graphWidth - i*graphWidth / numDataPoints;
			float y1 = graphHeight - graphHeight*pinDraw->heightForShapeDisplayHistory[i] / 85000;
			float x2 = graphWidth - (i + 1)*graphWidth / numDataPoints;
			float y2 = graphHeight - graphHeight*pinDraw->heightForShapeDisplayHistory[i + 1] / 85000;
			ofSetColor(175, 29, 29);
			ofLine(x1, y1, x2, y2);

			y1 = graphHeight - graphHeight*pinDraw->heightFromShapeDisplayHistory[i] / 85000;
			y2 = graphHeight - graphHeight*pinDraw->heightFromShapeDisplayHistory[i + 1] / 85000;
			ofSetColor(8, 114, 50);
			ofLine(x1, y1, x2, y2);

			y1 = graphHeight / 2 - graphHeight*(pinDraw->heightForShapeDisplayHistory[i+3] - pinDraw->heightFromShapeDisplayHistory[i]) / 3000;
			y2 = graphHeight / 2 - graphHeight*(pinDraw->heightForShapeDisplayHistory[i+4] - pinDraw->heightFromShapeDisplayHistory[i+1]) / 3000;
			ofSetColor(255, 87, 51);
			ofLine(x1, y1, x2, y2);

			y1 = graphHeight / 2;
			y2 = graphHeight / 2;
			ofSetColor(0);
			ofLine(x1, y1, x2, y2);

			y1 = graphHeight / 2 - graphHeight*velocityHistory[i] / 300000;
			y2 = graphHeight / 2 - graphHeight*velocityHistory[i + 1] / 300000;
			ofSetColor(55, 92, 150);
			ofLine(x1, y1, x2, y2);
		}

		ofSetColor(0, 0, 0);
		ofDrawBitmapString("heightFor (r): " + to_string(int(pinDraw->heightForShapeDisplayHistory[0])), 610, 0);
		ofDrawBitmapString("heightFrom (g): " + to_string(int(pinDraw->heightFromShapeDisplayHistory[0])), 610, 15);
		ofDrawBitmapString("heightDifference (o): " + to_string(int(pinDraw->heightForShapeDisplayHistory[0]-pinDraw->heightFromShapeDisplayHistory[0])), 610, 30);
		ofDrawBitmapString("velocity (b): " + to_string(int(velocityHistory[0])), 610, 45);
		ofDrawBitmapString("phantomGraphOffset: " + to_string(int(phantomGraphOffset)), 610, 60);
		
		
		float cons = pinDraw->heightForShapeDisplayHistory[0] - pinDraw->heightForShapeDisplayHistory[1];
		ofDrawBitmapString("speed: " + to_string(int(cons)), 610, 75);
		ofDrawBitmapString("window: " + to_string(pinDraw->window), 610, 90);
		ofDrawBitmapString("delay: " + to_string(pinDraw->delay), 610, 105);



		// phantom line chart

		for (int i = 0; i < numDataPoints - 1; i++) {
			float x1 = graphWidth - i*graphWidth / numDataPoints;
			float y1 = graphHeight - graphHeight*pinDraw->heightForShapeDisplayHistory[i+phantomGraphOffset] / 85000;
			float x2 = graphWidth - (i + 1)*graphWidth / numDataPoints;
			float y2 = graphHeight - graphHeight*pinDraw->heightForShapeDisplayHistory[i+ phantomGraphOffset + 1] / 85000;
			ofSetColor(201, 84, 84);
			ofLine(x1, y1, x2, y2);

			y1 = graphHeight - graphHeight*pinDraw->heightFromShapeDisplayHistory[i+ phantomGraphOffset] / 85000;
			y2 = graphHeight - graphHeight*pinDraw->heightFromShapeDisplayHistory[i+ phantomGraphOffset + 1] / 85000;
			ofSetColor(144, 206, 169);
			ofLine(x1, y1, x2, y2);

			y1 = graphHeight / 2 - graphHeight*(pinDraw->heightForShapeDisplayHistory[i+ phantomGraphOffset+3] - pinDraw->heightFromShapeDisplayHistory[i+ phantomGraphOffset]) / 3000;
			y2 = graphHeight / 2 - graphHeight*(pinDraw->heightForShapeDisplayHistory[i+ phantomGraphOffset + 4] - pinDraw->heightFromShapeDisplayHistory[i+ phantomGraphOffset + 1]) / 3000;
			ofSetColor(255, 127, 80);
			ofLine(x1, y1, x2, y2);


		}


		ofPopMatrix();
	}

	// 3D Model starts here //
	if (showDisplay3DModel) {
		cam.begin();

		ofPushMatrix();

		if (showDebug) {
			// Draw the origin
			ofSetLineWidth(4);
			ofSetColor(255, 0, 0);
			ofDrawLine(0, 0, 0, 100, 0, 0);
			ofSetColor(0, 255, 0);
			ofDrawLine(0, 0, 0, 0, 100, 0);
			ofSetColor(0, 0, 255);
			ofDrawLine(0, 0, 0, 0, 0, 100);

			//drawString for Cols and Rows
			ofPushMatrix();
			ofTranslate(PIN_SPACING_MM*INFORCE_COLS, 0, 0);
			for (int i = 0; i < INFORCE_COLS; i++) {
				ofTranslate(-PIN_SPACING_MM, 0, 0);
				ofDrawBitmapString(i, 0, -10);
			}
			ofPopMatrix();


			ofPushMatrix();
			for (int j = 0; j < INFORCE_ROWS; j++) {
				ofTranslate(0, PIN_SPACING_MM, 0);
				ofDrawBitmapString(j, -10, 0);
			}
			ofPopMatrix();
		}



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
				float pinHeightMM = receivePinHeight[i][j] / 1000.0;

				ofPushMatrix();

				// Move to the pin
				ofTranslate(PIN_SPACING_MM*INFORCE_COLS - (i + 1)*PIN_SPACING_MM, j*PIN_SPACING_MM, 0);

				float boxOriginX = PIN_SPACING_MM / 2.0;
				float boxOriginY = PIN_SPACING_MM / 2.0;
				float boxOriginZ = pinHeightMM - PIN_HEIGHT_MM / 2.0; // Draw pin body's under their height level

																	  // Draw the pin body
				ofFill();
				if (showStiffness) { //color for stiffness
					int col = ofMap(currentApplication->getTargetForce(i,j), MIN_STIFFNESS, MAX_STIFFNESS, 255, 0);
					ofSetColor(col,col,col);
				}
				else {
					ofSetColor(PIN_BODY_COLOR);
				}
				ofDrawBox(boxOriginX, boxOriginY, boxOriginZ, PIN_WIDTH_MM, PIN_WIDTH_MM, PIN_HEIGHT_MM);


				// Draw the wireframe outline of each pin body (so we can differentiate them)
				ofNoFill();
				ofSetColor(PIN_BODY_WIREFRAME_COLOR);
				ofSetLineWidth(2);
				ofDrawBox(boxOriginX, boxOriginY, boxOriginZ, PIN_WIDTH_MM, PIN_WIDTH_MM, PIN_HEIGHT_MM);

				if (showForceArrow) {
					// Draw the pin force as a red transparent square on the top of the pin
					
					ofFill();
					ofSetColor(255, 0, 0);
					ofSetLineWidth(5);
					float pinForceMN = currentApplication->pins[i][j]->getForce(); //receivePinForce[i][j];
					float forceFrac = pinForceMN / PIN_FORCE_MAX_MN;
					float arrowLength = forceFrac * MAX_FORCE_ARROW_LENGTH * 2;
					//cout << arrowLength << endl;
					if (arrowLength > 0.2) {
						
						ofDrawArrow(ofVec3f(boxOriginX, boxOriginY, pinHeightMM + arrowLength), ofVec3f(boxOriginX, boxOriginY, pinHeightMM - 0.001), FORCE_ARROW_HEAD_SIZE);
					}
				}


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


	if (showAppDraw) {
		currentApplication->drawApp();

	}

	

}

void ofApp::keyPressed(int key) {
	if (key == OF_KEY_LEFT) {
		pinDrawIndex = (pinDrawIndex + 49) % 50;
	}
	else if (key == OF_KEY_RIGHT) {
		pinDrawIndex = (pinDrawIndex + 1) % 50;
	}
	else if (key == OF_KEY_UP) {
		phantomGraphOffset++;
	}
	else if (key == OF_KEY_DOWN) {
		if (phantomGraphOffset > 0)
			phantomGraphOffset--;
	}
	else if (key == 'p') {
			paused = !paused;
	}
	else if (key == 'l') {
		showLineChart = !showLineChart;
	}
	else if (key == 'g') {
		showGrid = !showGrid;
	}
	else if (key == 'f') {
		showForceArrow = !showForceArrow;
	}
	else if (key == 'z') {
		showStiffness = !showStiffness;
	}
	else if (key == 'h') {
		showDebug = !showDebug;
	}
	else if (key == 'd') {
		
	}
	else if (key == 'a') {
		showAppDraw = !showAppDraw;
		showDisplay3DModel = !showDisplay3DModel;
	}
	else if (key == '1') {
		cout << "Running MaterialApp.cpp" << endl;
		currentApplication = new MaterialAppArduino();
		/*
		currentApplication = new MaterialApp();
		showAppDraw = false;
		showDisplay3DModel = true;
		showStiffness = true;*/
	}
	else if (key == '2') {
		cout << "Running LayerApp.cpp" << endl;
		currentApplication = new LayerApp();
		showAppDraw = true;
		showDisplay3DModel = false;
		showStiffness = false;
	}
	else if (key == '3') {
		cout << "Running MaterialVolumeApp.cpp" << endl;
		currentApplication = new MaterialVolumeApp();
		showAppDraw = true;
		showDisplay3DModel = false;
		showStiffness = false;
	}
	else if (key == '4') {
		cout << "Running GradientApp.cpp" << endl;
		currentApplication = new GradientApp();
		showAppDraw = false;
		showDisplay3DModel = true;
		showStiffness = true;
		/*
		//test for updating vibration 
		ofxOscMessage mSend;
		//cout << "new data" << endl;
		for (int i = 0; i < INFORCE_COLS; i++) {
			for (int j = 0; j < INFORCE_ROWS; j++) {
				mSend.addFloatArg(0);
				//cout << sendPinHeight[i][j] << ", ";
			}
		}
		cout << "set frequency to ZERO"<< endl;

		mSend.setAddress("/inForceVibrate");
		sender.sendMessage(mSend, false);
		*/
	}
	else if (key == '5') {
		cout << "Running MountainApp.cpp" << endl;
		currentApplication = new MountainApp();
		showAppDraw = false;
		showDisplay3DModel = true;
		showStiffness = true;
		/*
			float values[7];

			values[0] = cam.getPosition().x;
			values[1] = cam.getPosition().y;
			values[2] = cam.getPosition().z;
			ofQuaternion camQuat;
			camQuat = cam.getOrientationQuat();

			values[3] = camQuat.x();
			values[4] = camQuat.y();
			values[5] = camQuat.z();
			values[6] = camQuat.w();

			cout << values[0] << ", " << values[1] << ", " << values[2] << ", " << values[3] << ", " << values[4] << ", " << values[5] << ", " << values[6] << endl;
			*/
	}
	else if (key == '6') {
		cout << "Running MirrorApp.cpp" << endl;
		currentApplication = new MirrorApp();
		showAppDraw = false;
		showDisplay3DModel = true;
		showStiffness = true;
	}
	else if (key == '7') {
		cout << "Running IntouchApp.cpp" << endl;
		currentApplication = new IntouchApp();
		showAppDraw = false;
		showDisplay3DModel = true;
		showStiffness = true;
	}
	else if (key == '8') {
		cout << "Running ScannerApp.cpp" << endl;
		currentApplication = new ScannerApp();
		showAppDraw = false;
		showDisplay3DModel = true;
		showStiffness = false;
	}
	else if (key == '9') {
		cout << "Running ScannerDisplayApp.cpp" << endl;
		currentApplication = new ScannerDisplayApp();
		showAppDraw = false;
		showDisplay3DModel = true;
		showStiffness = false;
	}
	currentApplication->keyPressed(key);

}
void ofApp::keyReleased(int key) {}
void ofApp::mouseMoved(int x, int y) {}
void ofApp::mouseDragged(int x, int y, int button){
	currentApplication->mouseDragged(x, y, button);
}
void ofApp::mousePressed(int x, int y, int button) {

	currentApplication->mousePressed(x, y, button);
}
void ofApp::mouseReleased(int x, int y, int button){
	currentApplication->mouseReleased(x, y, button);
}
void ofApp::mouseEntered(int x, int y) {}
void ofApp::mouseExited(int x, int y) {}
void ofApp::windowResized(int w, int h) {}
void ofApp::gotMessage(ofMessage msg) {}
void ofApp::dragEvent(ofDragInfo dragInfo) {}