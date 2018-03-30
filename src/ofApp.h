#pragma once

#include <cmath>

#include "ofMain.h"
#include "ofxOsc.h"
#include "Config.h"
#include "math.h"
#include "Application.h"


const ofPoint DISPLAY_POSITION(-PIN_SPACING_MM * INFORCE_COLS / 2, -PIN_SPACING_MM * INFORCE_ROWS / 2, 0.0f);

// Rendering colors
const ofColor PIN_BODY_COLOR(250, 250, 250);
//const ofColor BACKGROUND_COLOR(ofColor::grey);
const ofColor PIN_BODY_WIREFRAME_COLOR(0, 0, 0);

const ofColor PIN_FORCE_HIGH_COLOR_TOUCHED(0, 255, 0);
const ofColor PIN_FORCE_LOW_COLOR_TOUCHED(0, 128, 0);
const ofColor PIN_FORCE_HIGH_COLOR_UNTOUCHED(255, 0, 0);
const ofColor PIN_FORCE_LOW_COLOR_UNTOUCHED(128, 0, 0);
//const ofColor FORCE_ARROW_COLOR(255,0,0);
const float MAX_FORCE_ARROW_LENGTH = 50.0;
const float FORCE_ARROW_HEAD_SIZE = 2.0;

const int MAX_STIFFNESS = 1000;
const int MIN_STIFFNESS = 0;

class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		ofxOscReceiver receiver;
		ofxOscSender sender;

		int32_t receivePinHeight[INFORCE_COLS][INFORCE_ROWS];
		int32_t receivePinForce[INFORCE_COLS][INFORCE_ROWS];
		string receivePinTouch[INFORCE_COLS][INFORCE_ROWS];

		int32_t sendPinHeight[INFORCE_COLS][INFORCE_ROWS];
		float sendPinFrequency[INFORCE_COLS][INFORCE_ROWS];
		float p_sendPinFrequency[INFORCE_COLS][INFORCE_ROWS];


		string msg_strings[NUM_MSG_STRINGS];
		float timers[NUM_MSG_STRINGS];

		ofEasyCam cam;

		int32_t val_stiffness[INFORCE_COLS][INFORCE_ROWS]; //up to 630?
		int32_t val_minHeight[INFORCE_COLS][INFORCE_ROWS];
		int32_t val_maxHeight[INFORCE_COLS][INFORCE_ROWS];

	private:
		bool paused = false;
		double timeOfLastUpdate = -1;

		Application *currentApplication;
		
		int pinDrawIndex = 0;
		int velocityHistory[100];

		int gridPosX = 20;
		int gridPosY = 20;
		int gridSize = 15;
		int gridSpaceY = 10 * INFORCE_ROWS + 50;

		bool showLineChart = true; // toggle with 'l'
		bool showGrid = true; // toggle with 'g'
		bool showForceArrow = true; // toggle with 'f'
		bool showStiffness = false; // toggle with 'z'
		bool showDebug = false; // toggle with 'h'
		bool showDisplay3DModel = true; // toggle with 'd'
		bool showAppDraw = false; // toggle with 'a'

		string currentDemo; // 

		ofImage stiffnessGradient;

		ofMesh mesh;


		int phantomGraphOffset = 0;

		ofTrueTypeFont modeText;

		
		bool UseNewDataSendingMethod = true;
};
