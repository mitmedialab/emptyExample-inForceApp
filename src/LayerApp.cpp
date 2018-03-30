#include "LayerApp.h"
#include <iostream>
#include <fstream>

LayerApp::LayerApp() {
	cout << "###### Application created: " << LayerApp::getName() << endl;

	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			maxHeightForShapeDisplay[i][j] = maxHeight;
			minHeightForShapeDisplay[i][j] = minHeight;
		}
	}

	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			heightsForShapeDisplay[i][j] = maxHeightForShapeDisplay[i][j];
			frequencyForShapeDisplay[i][j] = 0;
			touched[i][j] = false;
			timeSinceLastTouch[i][j] = 0;
		}
	}

	// Read in data
	for (int t = 0; t < numTimesteps; t++)
		for (int l = 0; l < numLayers; l++)
			for (int i = 0; i < 50; i++) {
				data[t][l][i / INFORCE_ROWS][i % INFORCE_ROWS] = 0;
				marked[t][l][i / INFORCE_ROWS][i % INFORCE_ROWS] = false;
			}
	string line;
	ifstream myfile("sampleLayerData.txt");

	if (myfile.is_open())
	{
		getline(myfile, line);
		int timesteps = std::stoi(line);
		getline(myfile, line);
		int layers = std::stoi(line);

		for (int t = 0; t < timesteps; t++) {
			for (int l = 0; l < layers; l++) {
				for (int i = 0; i < 50; i++) {
					getline(myfile, line);
					data[t][l][i / INFORCE_ROWS][i % INFORCE_ROWS] = std::stoi(line);

				}
			}
		}
		myfile.close();
	}


	//store the width and height for convenience
	cout << "debug1" << endl;

	int TransX = -5;
	int TransY = 0;


	
	//add one vertex to the mesh for each pixel
	for (int z = 0; z < numReducedLayers; z++) {
		for (int y = meshHeight - 1; y > -1; y--) {
			for (int x = 0 ; x < meshWidth ; x++) {

				

				int zz = ofMap(data[0][z*numLayers/numReducedLayers][meshWidth-x-1][meshHeight - y - 1], minHeight, maxHeight, -150, 150);

				renderMesh[z].addVertex(ofPoint(x*70 - meshWidth*70 / 2, y*70 - meshHeight * 70 / 2, zz));	// mesh index = x + y*width
														// this replicates the pixel array within the camera bitmap...

				int v = ofMap(z, 0, numReducedLayers, 0, 255);

				ofColor setCol(255 - v, 0, v, 60);
				renderMesh[z].addColor(setCol);  // placeholder for colour data, we'll get this from the camera
			}
		}
	}

	
	for (int z = 0; z < numReducedLayers; z++) {
		for (int y = 0; y < meshHeight - 1; y++) {
			for (int x = 0; x < meshWidth - 1; x++) {
				renderMesh[z].addIndex(x + y*meshWidth);				// 0
				renderMesh[z].addIndex((x + 1) + y*meshWidth);			// 1
				renderMesh[z].addIndex(x + (y + 1)*meshWidth);			// 10

				renderMesh[z].addIndex((x + 1) + y*meshWidth);			// 1
				renderMesh[z].addIndex((x + 1) + (y + 1)*meshWidth);		// 11
				renderMesh[z].addIndex(x + (y + 1)*meshWidth);			// 10
			}
		}
	}

	cam.setDistance(1000);
	cam.enableMouseInput();

	//debug orientation
	cam.setAutoDistance(false);
	cam.setPosition(-26.1306, -528.969, 374.567);
	cam.setOrientation(ofQuaternion(0.46682, 0.00604467, 0.00359336, 0.884325));


	//-35.5795, 556.963, 352.442, 7.98976e-06, 0.0802457, 0.148157, 0.000172985

	//demo orientation
//	cam.setAutoDistance(false);
//	cam.setPosition(-35.5795, 556.963, 352.442);
//	cam.setOrientation(ofQuaternion(7.98976e-06, 0.0802457, 0.148157, 0.000172985));



	projectionMapper = new ProjectionMapper(minHeight, maxHeight);
	setAllFbo();

	dataSender.switchOperationMode(0);
}

int LayerApp::heightToLayer(float minHeight, float maxHeight, float height) {
	float fractional = (maxHeight - height) / (maxHeight - minHeight + 1.0);
	return int(fractional * (numLayers - 1));
}
float LayerApp::heightToLayerFractional(float minHeight, float maxHeight, float height) {
	float fractional = (maxHeight - height) / (maxHeight - minHeight + 1.0);
	return fractional * (numLayers - 1) - int(fractional * (numLayers-1));
}

float linearCombination(float x1, float x2, float alpha) {
	return x1 * (1.0 - alpha) + x2 * alpha;
}

void LayerApp::update(float dt) {
	time += dt;
	// give some time to calibrate
	if (time < 5) {
		if (time > 2) {
			for (int i = 0; i < INFORCE_COLS; i++) {
				for (int j = 0; j < INFORCE_ROWS; j++) {
					pins[i][j]->calibrate(dt, forceFromShapeDisplay[i][j]);
				}
			}
		}
		return;
	}

	if (autoPlay) {
		currentTime += playSpeed;
		if (currentTime > numTimesteps - 1) {
			currentTime = 0;
		}
		cout << "autoplaying" << endl;
	}

	// calculate force pushing down
	float forceFromFinger = 0;
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			forceFromFinger += max(0.0f, pins[i][j]->getForce() - 300);
		}
	}

	float forceFromSpring = 250;
	float netForce = forceFromFinger - forceFromSpring;

	float currentLayerStiffness = 350; //(100 -currentLayer) * 80 + 200;
	cout << "currentlayer " << currentLayer << ", " << currentLayerStiffness << ", " << forceFromFinger  << endl;

	if (!lockLayer) {
		if (netForce > currentLayerStiffness) {
			float deltaHeight = 500 * (1 + log(netForce / currentLayerStiffness));
			if (buffer > 0)
				buffer -= deltaHeight;
			else
				height -= deltaHeight;
				buffer = -3000;
		}
		if (netForce < -150) {
			velocity += 20;
			if (buffer < 0)
				buffer += velocity;
			else
				height += velocity;
				buffer = 3000;
	
		}
		else {
			velocity = 0;
		}
	}
	else {
		if (netForce > 1000)
			lockLayer = false;
	}
	if (height > maxHeight) 
		height = maxHeight;
	else if (height < minHeight)
		height = minHeight;

	//cout << "forceFromFinger: " << forceFromFinger << ", netForce: " << netForce << ", height: " << height << endl;

	currentLayer = heightToLayer(minHeight, maxHeight, height);
	float currentLayerFractional = heightToLayerFractional(minHeight, maxHeight, height);

	//int currentReducedLayer = 0;
	for (int i = 0; i < numReducedLayers + 1; i++) {
	
		if (i * numLayers / numReducedLayers < currentLayer && currentLayer<= (i + 1) * numLayers / numReducedLayers) {
			currentReducedLayer = i;
		}


	}


	// marking wave 
	if (isMarking) {
		waveDistance += waveVelocity;

		if (waveVelocity > 0 && waveDistance > 14) {
			isMarking = false;
			waveVariance = 1.0f;
			waveHeight = 10000.0f;
		}
		else if (waveVelocity < 0 && waveDistance < 0) {
			marked[int(currentTime)][currentReducedLayer * numLayers / numReducedLayers][markI][markJ] = false;
			isMarking = false;

		}
	}

	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			timeSinceLastTouch[i][j]++;
			// set new pin heights

			//float h = linearCombination(data[int(currentTime)][currentLayer][i][j], data[int(currentTime)][currentLayer + 1][i][j], currentLayerFractional);
			//heightsForShapeDisplay[i][j] = h;
			
			heightsForShapeDisplay[i][j] = data[int(currentTime)][currentReducedLayer * numLayers/numReducedLayers][i][j];

			if (isMarking) {
				float distanceToMarkedPin = sqrt((i - markI)*(i - markI) + (j - markJ)*(j - markJ));
				heightsForShapeDisplay[i][j] += waveHeight*max(0.0f, exp(-(distanceToMarkedPin - waveDistance)*(distanceToMarkedPin - waveDistance) / waveVariance) - 0.2f);
			}
			if (marked[int(currentTime)][currentReducedLayer * numLayers / numReducedLayers][i][j])
				frequencyForShapeDisplay[i][j] = 10;
			else
				frequencyForShapeDisplay[i][j] = 0;

			// double tap for marking
			if (lockLayer) {
				if (pins[i][j]->getForce() < 200)
					touched[i][j] = false;
				else if (pins[i][j]->getForce() > 300) {
					if (!touched[i][j]) {
						if (timeSinceLastTouch[i][j] < 60 && !isMarking) {
								if (!marked[int(currentTime)][currentReducedLayer * numLayers / numReducedLayers][i][j])
									markPin(i, j);
								else
									unmarkPin(i, j);
						}
						timeSinceLastTouch[i][j] = 0;
					}
					touched[i][j] = true;
				}
			}
		}
	}

	

	for (int z = 0; z < numReducedLayers; z++) {
		for (int x = 0; x < meshWidth; x++) {
			for (int y = 0; y < meshHeight; y++) {
				ofVec3f tmpVec = renderMesh[z].getVertex(y*meshWidth + x);
				tmpVec.z = ofMap(data[int(currentTime)][z * numLayers/numReducedLayers][meshWidth - x - 1][meshHeight - y - 1], minHeight, maxHeight, -150, 150);
				renderMesh[z].setVertex(y * meshWidth + x, tmpVec);
			}
		}
	}

	//cout << currentLayer << endl;

	/*
	projectionMapper->update(dt, heightsFromShapeDisplay);

	updateTopProjectionFbo(topProjectionFbo.getWidth(), topProjectionFbo.getHeight());
	updateSideProjectionFbo(sideProjectionFbo[0].getWidth(), sideProjectionFbo[0].getHeight());
	updateFrontProjectionFbo(frontProjectionFbo[0].getWidth(), frontProjectionFbo[0].getHeight());

	ofImage t;
	topProjectionFbo.readToPixels(t);
	projectionMapper->setPinTopImage(t);

	ofImage t1[INFORCE_COLS];
	for (int i = 0; i < INFORCE_COLS; i++) {
		sideProjectionFbo[i].readToPixels(t1[i]);
	}
	projectionMapper->setPinSideImage(t1);

	ofImage t2[INFORCE_COLS];
	for (int i = 0; i < INFORCE_ROWS; i++) {
		frontProjectionFbo[i].readToPixels(t2[i]);
	}
	projectionMapper->setPinFrontImage(t2);
	*/
	updateOscSend();
}

void LayerApp::updateOscSend() {
	dataSender.sendHeightData(heightsForShapeDisplay);
}


void LayerApp::markPin(int i, int j) {
	cout << "Marked " << i << " " << j << endl;
	marked[int(currentTime)][currentReducedLayer * numLayers / numReducedLayers][i][j] = true;

	markI = i;
	markJ = j;
	isMarking = true;
	waveDistance = 1.0;
	waveVelocity = 0.3f;
}

void LayerApp::unmarkPin(int i, int j) {
	cout << "Unmarked " << i << " " << j << endl;
	
	markI = i;
	markJ = j;
	isMarking = true;
	waveDistance = 11.0;
	waveVelocity = -0.3f;
}

void LayerApp::drawSequencer(int x, int y, int w, int h) {
	ofFill();
	ofSetColor(230);
	ofDrawRectangle(x, y, w, h);

	ofNoFill();
	ofSetColor(50);
	ofDrawRectangle(x, y, w, h);

	ofPushMatrix();
	ofTranslate(0, 0, 1);
	int sl_X = ofMap(currentTime, 0, numTimesteps-1, 0, w);
	ofSetLineWidth(5);
	ofDrawLine(x+sl_X, y-2, x+sl_X, y+h+3);
	ofPopMatrix();

	ofSetLineWidth(2);

}

void LayerApp::drawApp() {
	ofEnableAlphaBlending();
	cam.begin();
	
	for (int z = 0; z < numReducedLayers -currentReducedLayer; z++) {
		ofSetLineWidth(2);
		renderMesh[numReducedLayers -z-1].drawWireframe();
		renderMesh[numReducedLayers -z-1].drawFaces();


		ofFill();
		ofSetColor(0, 255, 0, 100);
		ofSetLineWidth(5);
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			if (marked[int(currentTime)][currentReducedLayer * numLayers / numReducedLayers][INFORCE_COLS -1 -i][j]) {
				ofPushMatrix();
				int zz = ofMap(data[int(currentTime)][currentReducedLayer * numLayers / numReducedLayers][INFORCE_COLS - i - 1][j], minHeight, maxHeight, -150, 150);
				ofTranslate(i * 70 - (70*INFORCE_COLS) / 2, j * 70 -(70* INFORCE_ROWS )/ 2, zz + 5);

				ofDrawArrow(ofVec3f(0, 0, 7), ofVec3f(0, 0, -2), 8);
				//ofSphere(5,5,5);
				ofPopMatrix();
			}
		}
	}
	}


	// draw transluscent pins
	ofSetColor(255,255,255,50);
	ofSetLineWidth(2);
	int boxSize = 70;
	for (int i = 0; i< INFORCE_COLS; i++) {
		for (int j = 0; j< INFORCE_ROWS; j++) {
			ofPushMatrix();

			int boxH = ofMap(heightsFromShapeDisplay[i][j], minHeight, maxHeight, 0, 300);
			//boxH = 300;
			ofTranslate(-INFORCE_COLS * boxSize / 2 + (INFORCE_COLS - 1 - i) * boxSize ,
				-INFORCE_ROWS * boxSize / 2 + j * boxSize , -(300 - boxH) / 2);

			drawBoxWire(boxSize, boxSize, boxH);
			ofPopMatrix();

		}

	}



	cam.end();


	drawSequencer(ofGetWidth()/2-200,50, 400,20);


	if (enableProjection) {
		projectionMapper->drawProjectionMapper();
	}

	ofSetColor(255, 255, 255);


	sideProjectionFbo[0].draw(10, 10);
}

void LayerApp::setAllFbo() {
	float fboScale = 20;
	topProjectionFbo.allocate(fboScale*INFORCE_COLS, fboScale*INFORCE_ROWS);
	for (int x = 0; x < INFORCE_COLS; x++) {
		sideProjectionFbo[x].allocate(fboScale*INFORCE_ROWS, fboScale*INFORCE_ROWS);

	}

	for (int y = 0; y < INFORCE_ROWS; y++) {
		frontProjectionFbo[y].allocate(fboScale*INFORCE_COLS, fboScale*INFORCE_ROWS);
	}

}

void LayerApp::updateTopProjectionFbo(int w, int h) {


	topProjectionFbo.begin();
	ofClear(255, 255, 255);

	int z = currentReducedLayer;
	//cout << z << endl;
	int v = ofMap(z, 0, numReducedLayers, 0, 255);

	ofColor setCol(255 - v, 0, v);

	ofFill();
	ofSetColor(setCol);

	ofDrawRectangle(0, 0, w, h);


	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {

			

			if (marked[int(currentTime)][z * numLayers / numReducedLayers][INFORCE_COLS - 1 - i][j]) {
				ofPushMatrix();
				int zz = ofMap(data[int(currentTime)][z * numLayers / numReducedLayers][INFORCE_COLS - i - 1][j], minHeight, maxHeight, -150, 150);
				ofTranslate((i+0.5) * (w/INFORCE_COLS), (INFORCE_ROWS - j - 1+0.5) * (h / INFORCE_ROWS));
				ofSetColor(0,255,0);
				ofDrawEllipse(0, 0, (w / INFORCE_COLS)*0.5, (w / INFORCE_COLS)*0.5);
				//ofSphere(5,5,5);
				ofPopMatrix();
			}
		}
	}


	topProjectionFbo.end();

}

void LayerApp::updateSideProjectionFbo(int w, int h) {
	for (int x = 0; x < INFORCE_COLS; x++) {
		sideProjectionFbo[x].begin();
		ofClear(255, 255, 255);

		ofSetColor(0);
		ofFill();
		ofDrawRectangle(0, 0, w, h);

		ofSetColor(255);
		for (int y = 0; y < INFORCE_ROWS - 1; y++) {
			for (int layer = 0; layer <  numReducedLayers; layer++) {
				//cout << x << ", " << y << ", " << layer << endl;
				ofVec3f tmpVec1 = renderMesh[layer].getVertex(y*meshWidth + x);
				float h1 = ofMap(tmpVec1.z, 150, -150, 0, h);

				ofVec3f tmpVec2 = renderMesh[layer].getVertex((y+1)*meshWidth + x);
				float h2 = ofMap(tmpVec2.z, 150, -150, 0, h);

				float h3, h4;

				if (layer < numReducedLayers - 1) {
					ofVec3f tmpVec3 = renderMesh[layer + 1].getVertex((y + 1)*meshWidth + x);
					h3 = ofMap(tmpVec3.z, 150, -150, 0, h);

					ofVec3f tmpVec4 = renderMesh[layer + 1].getVertex(y*meshWidth + x);
					h4 = ofMap(tmpVec4.z, 150, -150, 0, h);
				}
				else {
					h3 = h;
					h4 = h;
				}


				//int z = (numReducedLayers - layer / numReducedLayers) - 1;
				int v = ofMap(numReducedLayers - 1-layer, 0, numReducedLayers, 255, 0);
				ofColor setCol(255 - v, 0, v);



				ofFill();
				ofSetColor(setCol);
				ofBeginShape();
				ofVertex((y + 0.5) * w / INFORCE_ROWS, h1);
				ofVertex((y + 1.5) * w / INFORCE_ROWS, h2);
				ofVertex((y + 1.5) * w / INFORCE_ROWS, h3);
				ofVertex((y + 0.5) * w / INFORCE_ROWS, h4);
				ofEndShape();
				
				
				//path.draw();
				//colMesh.drawFaces();
				//colMesh.drawWireframe();
			}
		}


		sideProjectionFbo[x].end();
	}

}

void LayerApp::updateFrontProjectionFbo(int w, int h) {
	for (int y = 0; y < INFORCE_ROWS; y++) {
		frontProjectionFbo[y].begin();
		ofClear(255, 255, 255);

		ofSetColor(0);
		ofFill();
		ofDrawRectangle(0, 0, w, h);

		ofSetColor(255);
		for (int x = 0; x < INFORCE_COLS - 1; x++) {
			for (int layer = 0; layer < numReducedLayers; layer++) {
				//cout << x << ", " << y << ", " << layer << endl;
				ofVec3f tmpVec1 = renderMesh[layer].getVertex(y*meshWidth + x);
				float h1 = ofMap(tmpVec1.z, 150, -150, 0, h);

				ofVec3f tmpVec2 = renderMesh[layer].getVertex(y*meshWidth + x+1);
				float h2 = ofMap(tmpVec2.z, 150, -150, 0, h);

				float h3, h4;

				if (layer < numReducedLayers - 1) {
					ofVec3f tmpVec3 = renderMesh[layer + 1].getVertex(y*meshWidth + x+1);
					h3 = ofMap(tmpVec3.z, 150, -150, 0, h);

					ofVec3f tmpVec4 = renderMesh[layer + 1].getVertex(y*meshWidth + x);
					h4 = ofMap(tmpVec4.z, 150, -150, 0, h);
				}
				else {
					h3 = h;
					h4 = h;
				}


				//int z = (numReducedLayers - layer / numReducedLayers) - 1;
				int v = ofMap(numReducedLayers - 1 - layer, 0, numReducedLayers, 255, 0);
				ofColor setCol(255 - v, 0, v);



				ofFill();
				ofSetColor(setCol);
				ofBeginShape();
				ofVertex((x + 0.5) * w / INFORCE_COLS, h1);
				ofVertex((x + 1.5) * w / INFORCE_COLS, h2);
				ofVertex((x + 1.5) * w / INFORCE_COLS, h3);
				ofVertex((x + 0.5) * w / INFORCE_COLS, h4);
				ofEndShape();


				//path.draw();
				//colMesh.drawFaces();
				//colMesh.drawWireframe();
			}
		}


		frontProjectionFbo[y].end();
	}



}


void LayerApp::drawBoxWire(int w, int d, int h) {
	ofDrawLine(-w / 2, -d / 2, -h / 2, w / 2, -d / 2, -h / 2);
	ofDrawLine(-w / 2, d / 2, -h / 2, w / 2, d / 2, -h / 2);
	ofDrawLine(-w / 2, -d / 2, h / 2, w / 2, -d / 2, h / 2);
	ofDrawLine(-w / 2, d / 2, h / 2, w / 2, d / 2, h / 2);

	ofDrawLine(w / 2, -d / 2, h / 2, w / 2, d / 2, h / 2);
	ofDrawLine(-w / 2, -d / 2, h / 2, -w / 2, d / 2, h / 2);
	ofDrawLine(w / 2, -d / 2, -h / 2, w / 2, d / 2, -h / 2);
	ofDrawLine(-w / 2, -d / 2, -h / 2, -w / 2, d / 2, -h / 2);

	ofDrawLine(w / 2, d / 2, -h / 2, w / 2, d / 2, h / 2);
	ofDrawLine(-w / 2, d / 2, -h / 2, -w / 2, d / 2, h / 2);
	ofDrawLine(w / 2, -d / 2, -h / 2, w / 2, -d / 2, h / 2);
	ofDrawLine(-w / 2, -d / 2, -h / 2, -w / 2, -d / 2, h / 2);
}


string LayerApp::appInstructionsText() {
	string instructions = (string) "" +
		"This is for testing things."
		"";

	return instructions;
}

void LayerApp::keyPressed(int key) {
	cout << "keyPressed" << endl;

	float timeSpeed = 3.5f;
	if (key == 'w') {
		lockLayer = true;
		autoPlay = false;
		if (currentTime + timeSpeed < numTimesteps)
			currentTime += timeSpeed;
	}
	else if (key == 's') {
		lockLayer = true;
		autoPlay = false;
		if (currentTime - timeSpeed >= 0)
			currentTime -= timeSpeed;
	}
	else if (key == 'r') {
		for (int i = 0; i < INFORCE_COLS; i++) {
			for (int j = 0; j < INFORCE_ROWS; j++) {
				heightsForShapeDisplay[i][j] = maxHeight;
				pinVelocity[i][j] = 0;
				frequencyForShapeDisplay[i][j] = 0;
				time = 0;
			}
		}
//		for (int i = -1; i < INFORCE_COLS + 1; i++) {
//			for (int j = -1; j < INFORCE_ROWS + 1; j++) {
//				heightsForShapeDisplayBuffered[i + 1][j + 1] = maxHeight;
//				heightsFromShapeDisplayBuffered[i + 1][j + 1] = maxHeight;
//			}
//		} 
	}
	else if (key == 'x') {
		lockLayer = !lockLayer;
		markI = 5;
		markJ = 2;
		isMarking = true;
		waveDistance = 1.0;
		waveVelocity = 6.0f;
		waveVariance = 999999;
		waveHeight = -3500.0f;
	}
	else if (key == ' ') {
		autoPlay = !autoPlay;
		cout << "autoPlay is " << autoPlay << endl;
	}
	else if (key == '9') {
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


	}
}