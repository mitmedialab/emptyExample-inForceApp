#pragma once

#include "Application.h"
#include "ProjectionMapper.h"


class LayerApp : public Application {
public:
	LayerApp();

	void update(float dt);
	string appInstructionsText();
	void keyPressed(int key);

	string getName() { return "Layer Application"; };

	OscDataSender dataSender;
	void updateOscSend();

private:
	float time = 0.0;

	// pin motion control
	float kp = 300;
	float kd = 10;
	float speedGoingUp = 3.0;

	float height = 80000;
	float velocity = 0;
	float buffer = 0;

	bool lockLayer = false;


	// layer data
	const static int numTimesteps = 100;
	const static int numLayers = 100;

	const static int numReducedLayers = 10; //still debugging, it is not working except 10

	const int meshWidth = 10;
	const int meshHeight = 5;

	float currentTime = 0.0f;
	int currentLayer = 0;

	int currentReducedLayer = 0;

	int data[numTimesteps][numLayers][INFORCE_COLS][INFORCE_ROWS]; //height data


	int32_t minHeight = 10000;
	int32_t maxHeight = 80000;

	int heightToLayer(float minHeight, float maxHeight, float height);
	float heightToLayerFractional(float minHeight, float maxHeight, float height);

	// pin marking
	bool touched[INFORCE_COLS][INFORCE_ROWS];
	int timeSinceLastTouch[INFORCE_COLS][INFORCE_ROWS];
	bool isMarking = false;
	float waveDistance = -1;
	float waveVariance = 1.0f;
	float waveHeight = 10000.0f;
	float waveVelocity = 0.3f;
	int markI = -1;
	int markJ = -1;
	bool marked[numTimesteps][numLayers][INFORCE_COLS][INFORCE_ROWS];

	void markPin(int i, int j);
	void unmarkPin(int i, int j);



	ofVboMesh renderMesh[numReducedLayers];

	void drawApp();

	ofEasyCam cam;

	void drawBoxWire(int w, int d, int h);
	void drawSequencer(int x, int y, int w, int h);


	bool autoPlay = false;
	float playSpeed = 0.2;



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