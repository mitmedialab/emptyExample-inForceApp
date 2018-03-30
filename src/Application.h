#pragma once

#include "Config.h"
#include "PinModel.h"
#include "ofxOsc.h"

#include "OscDataSender.h"


class Application {
public:
	Application();

	void getHeightsForShapeDisplay(int32_t heights[INFORCE_COLS][INFORCE_ROWS]);
	void setHeightsFromShapeDisplayRef(int32_t heights[INFORCE_COLS][INFORCE_ROWS]);
	void setForceFromShapeDisplayRef(int32_t force[INFORCE_COLS][INFORCE_ROWS]);
	void setTouchFromShapeDisplayRef(string touch[INFORCE_COLS][INFORCE_ROWS]);

	void getFrequencyForShapeDisplay(float frequency[INFORCE_COLS][INFORCE_ROWS]);

	void setStiffnessForDisplay(int32_t stiffness_[INFORCE_COLS][INFORCE_ROWS]);
	void setMaxHeightForDisplay(int32_t maxHeight_[INFORCE_COLS][INFORCE_ROWS]);
	void setMinHeightForDisplay(int32_t minHeight_[INFORCE_COLS][INFORCE_ROWS]);
	void updatePins(float dt);

	virtual void update(float dt) {};
	virtual void drawApp() {};
	virtual string appInstructionsText() { return ""; };
	virtual void keyPressed(int key) {};

	virtual void updateOscSend() {};

	virtual void mouseDragged(int x, int y, int button) {};
	virtual void mousePressed(int x, int y, int button) {};
	virtual void mouseReleased(int x, int y, int button) {};

	virtual string getName() { return "Application"; };


	//virtual getCommandMode()

	PinModel* pins[INFORCE_COLS][INFORCE_ROWS];

	float pinVelocity[INFORCE_COLS][INFORCE_ROWS];

	bool disablePin[INFORCE_COLS][INFORCE_ROWS];

	int32_t stiffnessForShapeDisplay[INFORCE_COLS][INFORCE_ROWS];

	int32_t getTargetForce(int x, int y);

protected:
	int32_t heightsForShapeDisplay[INFORCE_COLS][INFORCE_ROWS];// [INFORCE_COLS][INFORCE_ROWS];
	int32_t heightsFromShapeDisplay[INFORCE_COLS][INFORCE_ROWS];
	int32_t forceFromShapeDisplay[INFORCE_COLS][INFORCE_ROWS];
	string touchFromShapeDisplay[INFORCE_COLS][INFORCE_ROWS];
	
	//String commandMode;
	//int32_t paramsForShapeDisplay[1000];
	



	int32_t maxHeightForShapeDisplay[INFORCE_COLS][INFORCE_ROWS];
	int32_t minHeightForShapeDisplay[INFORCE_COLS][INFORCE_ROWS];

	float frequencyForShapeDisplay[INFORCE_COLS][INFORCE_ROWS];
	int32_t forceN[INFORCE_COLS][INFORCE_ROWS];

};

