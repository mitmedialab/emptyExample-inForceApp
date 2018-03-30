#pragma once

#include "Application.h"
#include "ofxXmlSettings.h"
#include "Config.h"


class ProjectionApp : public Application {
public:
	ProjectionApp();

	void update(float dt);
	string appInstructionsText();
	void keyPressed(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);

	void drawApp();

	string getName() { return "Testing Application"; };


	int transX = 100, transY = 100;

	vector<ofVec2f> vertexPinHigh;
	vector<ofVec2f> vertexPinLow;

	ofMesh pin_TopMesh[INFORCE_COLS][INFORCE_ROWS];
	ofMesh pin_SideMesh[INFORCE_COLS][INFORCE_ROWS]; // slice x
	ofMesh pin_FrontMesh[INFORCE_ROWS][INFORCE_COLS]; // slice x

	ofMesh highMesh;
	ofMesh lowMesh;

	int radius_grabPoint = 10;
	bool mouseGrabbing = false;
	ofVec2f grabbedPoint;


	ofVec2f imageCoordinateTop[INFORCE_COLS + 1][INFORCE_ROWS + 1];
	ofVec2f imageCoordinateFront[INFORCE_ROWS][INFORCE_COLS + 1][2][2];
	ofVec2f imageCoordinateSide[INFORCE_COLS][INFORCE_ROWS + 1][2][2]; // [slice][widthNum][mesh+ heightNum]


	int pinHeight_MIN = 0;
	int pinHeight_MAX = 80000;


	int currentX = 0;
	int currentY = 0;

	ofImage mapImageTop;
	ofImage mapImageFront[INFORCE_ROWS];
	ofImage mapImageSide[INFORCE_COLS];


	int editMode = 0; // 0 is off 1 is high pin, 2 is low pin.

	ofxXmlSettings XML;
	string message;

	void setupProjectionMesh();
	void saveCalibrationData();
	void loadCalibrationData();
	void autoCalibrationFromCornerPoints();
	void updateMeshRendering();
	void updateHighLowMesh();


private:
	void updateHeights();
	void updateHeights2();
	void updateHeights3();
	void updateHeights4();
	void updateHeights5(int i, int j);

	float x = 0;
	float y = 0;

	float dx = 2;
	float dy = 3;

	int32_t minHeight = 20000;
	int32_t maxHeight = 60000;
	float variance = 16;

	float setHeight = 50000;
};