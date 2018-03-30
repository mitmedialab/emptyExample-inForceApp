#pragma once

#include "Application.h"
#include "ofxVolumetrics.h"
#include "ProjectionMapper.h"


class MaterialVolumeApp : public Application {
public:
	MaterialVolumeApp();

	void update(float dt);
	string appInstructionsText();
	void keyPressed(int key);

	void drawApp();

	string getName() { return "Material Volume Application"; };

	

private:
	float time = 0.0;

	// pin motion control
	float kp = 500;
	float kd = 100;
	float lastDeltaForce[INFORCE_COLS][INFORCE_ROWS];
	bool pinAcceleratingUp[INFORCE_COLS][INFORCE_ROWS];

	float getTargetForce(int height);
	float getTargetForce2(int height);
	float getTargetForce3(int height);
	float getTargetForce4(int height);

	float getTargetFrequency(int height);

	int32_t minHeight = 10000;
	int32_t maxHeight = 80000;

	float minForce = 150;
	float midForce = 500;
	float maxForce = 900;

	float speedConstant = 500;

	float lastNetForce[INFORCE_COLS][INFORCE_ROWS];
	float speedGoingUp = 2.0;


	void setPinParameter(int x, int y, int maxH, int minH, int force, float frequency);
	void processPinPositionUsingParameter(float dt);

	void resetPinCalculationParameter(int x, int y);

	float variance = 0.0000000000001;




	//for Volumetric data 
	ofEasyCam cam;

	ofxVolumetrics myVolume;
	unsigned char * volumeData;
	unsigned char * volumeDataStored;
	bool linearFilter;

	void drawBoxWire(int w, int d, int h);

	void volumeDataSetup();
	void volumeDataUpdate();
	bool isVolumeDataUpdated;

	int threshold = 50; // remove??

	float scale = 0.5;
	int inForceX_W = 100 * scale; // 100  - 10pins 
	int inForceZ_H = 50;  // -5 pins
	int inForceY_D = 50 * scale; // 50

	

	int transparentAlpha = 25;
	int shapeAlpha = 150;
	int bgAlpha = 50;

	int heightFromDisplayRenderingMax = 80000;


	void addShape(int x, int y, int z, int w, int h, int d, ofColor c, string SHAPENAME);
	int aS_X[10], aS_Y[10], aS_Z[10], aS_W[10], aS_H[10], aS_D[10];
	ofColor aS_C[10];
	string aS_Shape[10];
	int shapeNum = 0;

	void assignColorBasedOnShape(int x, int y, int z, int i4);


	// projection related
	bool enableProjection = true;


	ProjectionMapper *projectionMapper;
	ofFbo topProjectionFbo;
	ofFbo sideProjectionFbo[INFORCE_COLS];
	ofFbo frontProjectionFbo[INFORCE_ROWS];

	void updateTopProjectionFbo(int w, int h);
	void updateSideProjectionFbo(int w, int h);
	void updateFrontProjectionFbo(int w, int h);

	void setAllFbo();

};