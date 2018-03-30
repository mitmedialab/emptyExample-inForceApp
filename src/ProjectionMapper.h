#pragma once
#include "Config.h"
#include "ofxXmlSettings.h"
#include <map>


class ProjectionMapper {
public:
	ProjectionMapper(int _minHeight, int _maxHeight);


	void update(float dt, int32_t heightFromShapeDisplay[INFORCE_COLS][INFORCE_ROWS]);
	void drawProjectionMapper();

	void setupProjectionMesh();
	void loadCalibrationData();
	void updateMeshRendering(int32_t heightsFromShapeDisplay[INFORCE_COLS][INFORCE_ROWS]);
	void updateHighLowMesh();

	void setPinTopImage(ofImage img);
	void setPinSideImage(ofImage img[INFORCE_COLS]);
	void setPinFrontImage(ofImage img[INFORCE_ROWS]);

	int transX = 100, transY = 100;

	ofImage mapImageTop;
	ofImage mapImageFront[INFORCE_ROWS];
	ofImage mapImageSide[INFORCE_COLS];


	vector<ofVec2f> vertexPinHigh;
	vector<ofVec2f> vertexPinLow;

	ofMesh pin_TopMesh[INFORCE_COLS][INFORCE_ROWS];
	ofMesh pin_SideMesh[INFORCE_COLS][INFORCE_ROWS]; // slice x
	ofMesh pin_FrontMesh[INFORCE_ROWS][INFORCE_COLS]; // slice x

	ofMesh highMesh;
	ofMesh lowMesh;

	ofVec2f imageCoordinateTop[INFORCE_COLS + 1][INFORCE_ROWS + 1];
	ofVec2f imageCoordinateFront[INFORCE_ROWS][INFORCE_COLS + 1][2][2];
	ofVec2f imageCoordinateSide[INFORCE_COLS][INFORCE_ROWS + 1][2][2]; // [slice][widthNum][mesh+ heightNum]


	ofxXmlSettings XML;
	string message;


private:
	int pinHeight_MIN = 0;
	int pinHeight_MAX = 80000;


};