#include "MaterialVolumeApp.h"
#include <iostream>
#include <fstream>

MaterialVolumeApp::MaterialVolumeApp() {
	cout << "###### Application created: " << MaterialVolumeApp::getName() << endl;

	
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			setPinParameter(i, j, maxHeight, minHeight, 900, 0);

			heightsForShapeDisplay[i][j] = maxHeightForShapeDisplay[i][j];
			pinVelocity[i][j] = 0;
			lastNetForce[i][j] = 0;
		}
	}

	projectionMapper = new ProjectionMapper(minHeight, maxHeight);

	

	volumeDataSetup();

	linearFilter = false;

	//    for(int i = 0; i<10; i++ ){
	//        for(int j = 0; j < 5; j++){
	//            threshold[i][j] = 50;
	//        }
	//    }

	setAllFbo();

	cam.setDistance(1000);
	cam.enableMouseInput();

	cam.setPosition(0, -529.325, 372.016);
	cam.setOrientation(ofQuaternion(0.460974, 0, 0, 0.887413));
}

void MaterialVolumeApp::update(float dt) {

	

	time += dt;
	if (time > 2 && time < 5) {
		for (int i = 0; i < INFORCE_COLS; i++) {
			for (int j = 0; j < INFORCE_ROWS; j++) {
				pins[i][j]->calibrate(dt, forceFromShapeDisplay[i][j]);
			}
		}
		return;
	}

	if (threshold < 49) {
		for (int i = 0; i < INFORCE_COLS; i++) {
			for (int j = 0; j < INFORCE_ROWS; j++) {
				int centerOfPinX = ((i + 0.5) * inForceX_W) / INFORCE_COLS;
				int centerOfPinY = ((j + 0.5) * inForceY_D) / INFORCE_ROWS;

				int setPinHeight = ofMap(threshold, 0, inForceZ_H - 1, minHeight, maxHeight);
				for (int k = threshold; k < inForceZ_H; k++) {
				int i4 = ((centerOfPinX + inForceX_W*centerOfPinY) + k*inForceX_W*inForceY_D) * 4;
					
					if ( !(volumeData[i4] == 100 && volumeData[i4 + 1] == 100 && volumeData[i4+2] == 100 ) ) {
						int v = ofMap(k, 0, inForceZ_H - 1, minHeight, maxHeight);
						setPinHeight = MAX(v, setPinHeight);
					}
					maxHeightForShapeDisplay[INFORCE_COLS-i-1][INFORCE_ROWS-j-1] = setPinHeight;

				}
			}
		}
		
	}

	volumeDataUpdate(); 	// Update Volume Data based on PinHeight

	processPinPositionUsingParameter(dt);

	projectionMapper->update(dt, heightsFromShapeDisplay);

	updateTopProjectionFbo(topProjectionFbo.getWidth(), topProjectionFbo.getHeight() );
	updateSideProjectionFbo(sideProjectionFbo[0].getWidth(), sideProjectionFbo[0].getHeight() );
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


}

void MaterialVolumeApp::drawApp() {
	ofDisableDepthTest();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);

	ofFill();
	ofSetLineWidth(1);

	ofQuaternion camQuat = cam.getOrientationQuat();
	float rotX = camQuat.x();
	float rotY = camQuat.y();
	float rotZ = camQuat.z();


	cam.begin();

	ofPushMatrix();

	float rotationAmount;
	ofVec3f rotationAngle;
	ofQuaternion rotation;
	camQuat.getRotate(rotationAmount, rotationAngle);


	float angleX = rotationAmount * rotationAngle.x;
	float angleZ = rotationAmount * rotationAngle.z;

	ofRotateX(2 * angleX);
	ofRotateZ(2 * angleZ);

	ofSetColor(255, 255, 255, 255);
	myVolume.drawVolume(0, 0, 0, ofGetHeight(), 0); //float x, float y, float z, float size, int zTexOffset

	ofPopMatrix();

	ofPushMatrix();
	/*
	ofBoxPrimitive box2;
	box2.set(20, 20, 20);
	box2.setPosition(0, 0, 210);
	vector<ofMeshFace> triangles2 = box2.getMesh().getUniqueFaces();

	ofSetColor(255, 0, 0, 100);
	box2.draw();
	*/

	ofPopMatrix();



	ofSetColor(255, 255, 255, 70);
	int boxSize = 80;
	for (int i = 0; i< INFORCE_COLS; i++) {
		for (int j = 0; j< INFORCE_ROWS; j++) {
			ofPushMatrix();

			int boxH = ofMap(heightsFromShapeDisplay[i][j], 0, heightFromDisplayRenderingMax, 0, 300) ;
			//boxH = 300;
			ofTranslate(-INFORCE_COLS * boxSize / 2 + (INFORCE_COLS - 1 -i) * boxSize + boxSize / 2,
				       -INFORCE_ROWS * boxSize / 2 + j * boxSize + boxSize / 2,  -(300 - boxH)/2);

			drawBoxWire(boxSize, boxSize, boxH);
			ofPopMatrix();

		}

	}
	ofNoFill();

	ofPushMatrix();
	int transZ = ofMap(threshold, 0, 50, -150, 150);
	ofTranslate(0, 0, transZ);
	ofDrawRectangle( - INFORCE_COLS * boxSize / 2 - 100, -INFORCE_ROWS * boxSize / 2 -100 , INFORCE_COLS * boxSize  + 200, INFORCE_ROWS * boxSize + 200);

	

	ofPopMatrix();

	cam.end();

	if (enableProjection) {
		projectionMapper->drawProjectionMapper();
	}

	//ofDrawRectangle(900, 10, 200, 100);
	ofSetColor(255, 255, 255);
	topProjectionFbo.draw(900, 10, 200, 100);
}

float MaterialVolumeApp::getTargetForce(int height) {
	return ofMap(height, 20000, maxHeight, 1000, 500);
}

float MaterialVolumeApp::getTargetForce2(int height) {
	return minForce;
}

float MaterialVolumeApp::getTargetForce3(int height) {
	if (height > 65000)
		return minForce;
	else if (height > 50000)
		return midForce;
	else if (height > 35000)
		return minForce;
	else if (height > 20000)
		return midForce;
	else
		return minForce;
}

float MaterialVolumeApp::getTargetForce4(int height) {
	if (height > 65000)
		return 200;
	else
		return 900;
}

float MaterialVolumeApp::getTargetFrequency(int height) {
	float fq = 5;
	if (height < 40000) {
		fq = 1;
	}
	else if (height < 78000) {
		fq = 20;
	}


	return fq;

}

void MaterialVolumeApp::drawBoxWire(int w, int d, int h) {
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

//--------------------------------------------------------------

void MaterialVolumeApp::volumeDataSetup() {
	ofColor Cube1Col;
	Cube1Col.set(255, 0, 0);
	addShape(20, 10, 15,  30, 40, 20,  Cube1Col, "BOX");

	ofColor Cube2Col;
	Cube2Col.set(0, 0, 255);
	addShape(0, 0, 8, 20, 30, 20, Cube2Col, "BOX");

	ofColor Sphere1Col;
	Sphere1Col.set(0, 255, 0);
	addShape(80, 30, 20, 18, 18, 18, Sphere1Col, "SPHERE");

	ofColor Sphere2Col;
	Sphere2Col.set(0, 200, 200);
	addShape(70, 10, 20, 20, 20, 20, Sphere2Col, "SPHERE");


	volumeData = new unsigned char[inForceX_W*inForceZ_H*inForceY_D * 4];
	volumeDataStored = new unsigned char[inForceX_W*inForceZ_H*inForceY_D * 4];


	for (int z = 0; z<inForceZ_H; z++)
	{
		//imageSequence.loadFrame(z);
		for (int x = 0; x<inForceX_W; x++)
		{
			for (int y = 0; y<inForceY_D; y++)
			{
				// convert from greyscale to RGBA, false color
				int i4 = ((x + inForceX_W*y) + z*inForceX_W*inForceY_D) * 4;
				assignColorBasedOnShape(x, y, z, i4);
				for (int i = 0; i < 4; i++) {
					volumeDataStored[i + i4] = volumeData[i + i4];
				}
			}
		}
	}


	myVolume.setup(inForceX_W, inForceY_D, inForceZ_H,  ofVec3f(1, 1, 0.5), true);
	myVolume.updateVolumeData(volumeData, inForceX_W, inForceY_D, inForceZ_H,  0, 0, 0);
	myVolume.setRenderSettings(1.0, 1.0, 0.75, 0.1);


}

void MaterialVolumeApp::addShape(int x, int y, int z, int w, int h, int d, ofColor c, string SHAPENAME) {
	aS_X[shapeNum] = x * scale;
	aS_Y[shapeNum] = y * scale;
	aS_Z[shapeNum] = z;
	aS_W[shapeNum] = w * scale;
	aS_H[shapeNum] = h * scale;
	aS_D[shapeNum] = d;

	aS_C[shapeNum] = c;
	aS_Shape[shapeNum] = SHAPENAME;

	shapeNum++;
}

void MaterialVolumeApp::assignColorBasedOnShape(int x, int y, int z, int i4) {
	bool flag = false;
	int n = 0;
	

	while (!flag && n < shapeNum) {
		if (aS_Shape[n] == "SPHERE") {
			if (pow(x - aS_X[n], 2)/ pow(aS_W[n], 2) + pow(y - aS_Y[n], 2) / pow(aS_H[n], 2) + pow(z - aS_Z[n], 2) / pow(aS_D[n], 2) < 1) {
				volumeData[i4] = aS_C[n].r;
				volumeData[i4+1] = aS_C[n].g;
				volumeData[i4+2] = aS_C[n].b;
				volumeData[i4+3] = shapeAlpha;
				flag = true;
			}
			
		}
		else if (aS_Shape[n] == "BOX") {
			if ((aS_X[n] < x && x < aS_W[n] + aS_X[n]) && (aS_Y[n] < y && y < aS_Y[n] + aS_H[n]) && (aS_Z[n] < z && z < aS_Z[n] + aS_D[n])) {
				volumeData[i4] = aS_C[n].r;
				volumeData[i4 + 1] = aS_C[n].g;
				volumeData[i4 + 2] = aS_C[n].b;
				volumeData[i4 + 3] = shapeAlpha;
				flag = true;
			}

		}
		n++;
	}

	if (flag == false) {
		volumeData[i4] = 100;
		volumeData[i4 + 1] = 100;
		volumeData[i4 + 2] = 100;
		volumeData[i4 + 3] = bgAlpha;
	}



}

void MaterialVolumeApp::volumeDataUpdate() {
	// Update Volume Data based on PinHeight

	//volumeData = new unsigned char[inForceX_W*inForceZ_H*inForceY_D*4];

	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			for (int z = 0; z<inForceZ_H; z++)
			{
				for (int x = i * inForceX_W / INFORCE_COLS; x< (i + 1) * inForceX_W / INFORCE_COLS; x++)
				{
					for (int y = j * inForceY_D / INFORCE_ROWS; y< (j + 1) * inForceY_D / INFORCE_ROWS; y++)
					{
						int i4 = ((x + inForceX_W*y) + z*inForceX_W*inForceY_D) * 4;

						int boxH = ofMap(heightsFromShapeDisplay[INFORCE_COLS - i - 1][INFORCE_ROWS - j - 1], minHeight, maxHeight, 0, inForceZ_H-1);

						if (boxH < z) {
							volumeData[i4 + 3] = transparentAlpha;
						}
						else {
							volumeData[i4 + 3] = volumeDataStored[i4 + 3];
						}
						
						int centerOfPinX = ((i + 0.5) * inForceX_W ) / INFORCE_COLS;
						int centerOfPinY =  ((j + 0.5) * inForceY_D ) / INFORCE_ROWS;
						//cout << centerOfPinX << " - " << centerOfPinY << endl;

						if (boxH == z && x == centerOfPinX && y == centerOfPinY) {
							//cout << "FOUND CENTER = "<< i << ", " << j << endl;
							if (volumeData[i4] == 255) { //r
								setPinParameter(INFORCE_COLS - 1 - i, INFORCE_ROWS -1 - j, maxHeightForShapeDisplay[i][j], minHeight, maxForce, 15);
							} 
							else if (volumeData[i4+1] == 255) { //g
								setPinParameter(INFORCE_COLS - 1 - i , INFORCE_ROWS - 1 - j, maxHeightForShapeDisplay[i][j], minHeight, midForce, 10);
							}
							else if (volumeData[i4+2] == 255) { //b
								setPinParameter(INFORCE_COLS - 1 - i, INFORCE_ROWS - 1 - j, maxHeightForShapeDisplay[i][j], minHeight, maxForce+maxHeight, 0);
								resetPinCalculationParameter(i, j);
							}
							else if (volumeData[i4 + 1] == 200) { //g 200, b 200
								int setPinHeight = 0;
									for(int k = 0; k < inForceZ_H;k++){
										int i4_ = ((x + inForceX_W*y) + k*inForceX_W*inForceY_D) * 4;

										if (volumeData[i4_ + 1] == 200) {
											int v = ofMap(k, 0, inForceZ_H -1, minHeight, maxHeight);
											setPinHeight = MAX(v, setPinHeight);
										}
									}
									//cout << setPinHeight << endl;
								setPinParameter(INFORCE_COLS - 1 - i, INFORCE_ROWS - 1 - j, maxHeightForShapeDisplay[i][j], setPinHeight, maxForce, 0);
							}
							else { //others
								setPinParameter(INFORCE_COLS - 1 - i, INFORCE_ROWS - 1 - j, maxHeightForShapeDisplay[i][j], minHeight, 200, 0);
							}
						}

						
					}
				}
			}
		}
	}

	myVolume.updateVolumeData(volumeData, inForceX_W, inForceY_D, inForceZ_H, 0, 0, 0);
	isVolumeDataUpdated = false;

}

void MaterialVolumeApp::setAllFbo() {
	float fboScale = 20;
	topProjectionFbo.allocate(fboScale*INFORCE_COLS, fboScale*INFORCE_ROWS);
	for (int x = 0; x < INFORCE_COLS; x++) {
		sideProjectionFbo[x].allocate(fboScale*INFORCE_ROWS, fboScale*INFORCE_ROWS);
		
	}

	for (int y = 0; y < INFORCE_ROWS; y++) {
		frontProjectionFbo[y].allocate(fboScale*INFORCE_COLS, fboScale*INFORCE_ROWS);
	}

}

void MaterialVolumeApp::updateTopProjectionFbo(int w, int h) {
	topProjectionFbo.begin();
	ofClear(255, 255, 255);

	ofSetColor(255);
	ofFill();
	ofDrawRectangle(0, 0, w, h);
	
	for (int i = 0; i < inForceX_W; i++) {
		for (int j = 0; j < inForceY_D; j++) {
			bool flag = false;
			int z = inForceZ_H - 1;
			int i4;
			while (flag == false) {
				i4 = ((i + inForceX_W*j) + z*inForceX_W*inForceY_D) * 4;
				if (volumeData[i4 + 3] > transparentAlpha) {
					flag = true;
				}
				else {
					z--;
					if (z < 0) {
						flag = true;
						z = 0;
					}
				}
			}


			//  int i4 = ((i+inForceX_W*j)+z*inForceX_W*inForceZ_H)*4;
			ofColor col;
			col.set(volumeData[i4], volumeData[i4 + 1], volumeData[i4 + 2]);

			ofSetColor(col);
			ofDrawRectangle(i*w / inForceX_W, j*h / inForceY_D, w / inForceX_W, h / inForceY_D);
		}
	}
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			ofSetColor(0);
			ofDrawLine(i*(w/INFORCE_COLS), j*(h / INFORCE_ROWS), (i+1)*(w / INFORCE_COLS), j*(h / INFORCE_ROWS));
			ofDrawLine(i*(w / INFORCE_COLS), j*(h / INFORCE_ROWS), i*(w / INFORCE_COLS), (j+1)*(h / INFORCE_ROWS));
		}
	}

	topProjectionFbo.end();
}

void MaterialVolumeApp::updateSideProjectionFbo(int w, int h) {
	for (int x = 0; x < INFORCE_COLS; x++) {
		sideProjectionFbo[x].begin();
		ofClear(255, 255, 255);

		ofSetColor(255);
		ofFill();
		ofDrawRectangle(0, 0, w, h);

		for (int j = 0; j < inForceY_D; j++) {
			for (int z = 0; z < inForceZ_H; z++) {
				bool flag = false;
				int i = x * (inForceX_W / INFORCE_COLS) ;
				int i4 = ((i + inForceX_W*j) + z*inForceX_W*inForceY_D) * 4;
				//  int i4 = ((i+inForceX_W*j)+z*inForceX_W*inForceZ_H)*4;
				ofColor col;
				col.set(volumeData[i4], volumeData[i4 + 1], volumeData[i4 + 2]);

				ofSetColor(col);
				ofDrawRectangle(j*w / inForceY_D, z*h / inForceZ_H, w / inForceY_D, h / inForceZ_H);
			}
		}

		sideProjectionFbo[x].end();
	}
}

void MaterialVolumeApp::updateFrontProjectionFbo(int w, int h) {
	for (int y = 0; y < INFORCE_ROWS; y++) {
		frontProjectionFbo[y].begin();
		ofClear(255, 255, 255);

		ofSetColor(255);
		ofFill();
		ofDrawRectangle(0, 0, w, h);

		for (int i = 0; i < inForceX_W; i++) {
			for (int z = 0; z < inForceZ_H; z++) {
				bool flag = false;
				int j = (y+1) * (inForceY_D / INFORCE_ROWS) -1;
				int i4 = ((i + inForceX_W*j) + z*inForceX_W*inForceY_D) * 4;
				//  int i4 = ((i+inForceX_W*j)+z*inForceX_W*inForceZ_H)*4;
				ofColor col;
				col.set(volumeData[i4], volumeData[i4 + 1], volumeData[i4 + 2]);

				ofSetColor(col);
				ofDrawRectangle(i*w / inForceX_W, z*h / inForceZ_H, w / inForceX_W, h / inForceZ_H);
			}
		}

		frontProjectionFbo[y].end();
	}
}

void MaterialVolumeApp::setPinParameter(int x, int y, int maxH, int minH, int force, float frequency) {
	maxHeightForShapeDisplay[x][y] = maxH;
	minHeightForShapeDisplay[x][y] = minH;

	if (pinVelocity[x][y] < 100) {
		frequencyForShapeDisplay[x][y] = frequency;
	}
	else {
		frequencyForShapeDisplay[x][y] = 0;
	}
	forceN[x][y] = force;

}

void MaterialVolumeApp::resetPinCalculationParameter(int x, int y) {
	pinVelocity[x][y] = 0;
	lastNetForce[x][y] = 0;
	pinVelocity[x][y] = 0;

}

void MaterialVolumeApp::processPinPositionUsingParameter(float dt) {


	// calculate force pushing down
	float forceFromFinger[INFORCE_COLS][INFORCE_ROWS];
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			forceFromFinger[i][j] = 0;
		}
	}
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			for (int ii = 0; ii < INFORCE_COLS; ii++) {
				for (int jj = 0; jj < INFORCE_ROWS; jj++) {
					float distanceSquared = (i - ii)*(i - ii) + (j - jj)*(j - jj);
					float scaleFactor = exp(-distanceSquared / variance); // sf = 0: 0, sf = 1: 100
					if (i == ii && j == jj)
						forceFromFinger[ii][jj] += max(0.0f, pins[i][j]->getForce() - 60);
					if (i == ii && j == jj)
						forceFromFinger[ii][jj] += scaleFactor * max(0.0f, pins[i][j]->getForce() - 60 * scaleFactor);
				}
			}
		}
	}

	// calculate pin movement
	float newHeights[INFORCE_COLS][INFORCE_ROWS];
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {

			float forceFromSpring = getTargetForce(heightsFromShapeDisplay[i][j]); // DEFINE force pushing upwards   
			//forceN[i][j] = forceFromSpring;
			float netForce = forceFromFinger[i][j] - forceFromSpring;

			// create deadband: if netForce is in (-30, 0), set equal to 0
			if (netForce < 0) 
				netForce = min(0.0f, netForce + 30.0f);

			// PID loop for velocity with some memory
			float newVelocity = 0.05*pinVelocity[i][j] - 0.95*(kp*netForce + kd*(netForce - lastNetForce[i][j]));

			// getting faster requires acceleration; getting slower does not
			if (abs(newVelocity) < pinVelocity[i][j])
				pinVelocity[i][j] = newVelocity;
			else
				pinVelocity[i][j] = 0.94 * pinVelocity[i][j] + 0.03 * newVelocity;

			lastNetForce[i][j] = netForce;

			
			if (pinVelocity[i][j] < 0)
				newHeights[i][j] = 1.5*(heightsFromShapeDisplay[i][j] - heightsForShapeDisplay[i][j]) + heightsForShapeDisplay[i][j] + ofMap(forceFromSpring, 200, 1500, 1.0, 0.5) * dt*pinVelocity[i][j];
			else 
				newHeights[i][j] = 1.5*(heightsFromShapeDisplay[i][j] - heightsForShapeDisplay[i][j]) + heightsForShapeDisplay[i][j] + dt*speedGoingUp*pinVelocity[i][j];

		}
	}

	// calculate indentations
	float indentations[INFORCE_COLS][INFORCE_ROWS];
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			indentations[i][j] = maxHeightForShapeDisplay[i][j];
		}
	}
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			for (int ii = 0; ii < INFORCE_COLS; ii++) {
				for (int jj = 0; jj < INFORCE_ROWS; jj++) {

					float distanceSquared = (i - ii)*(i - ii) + (j - jj)*(j - jj);
					float height = exp(-distanceSquared / variance) * (maxHeightForShapeDisplay[i][j] - newHeights[i][j]);
					if (indentations[ii][jj] > maxHeightForShapeDisplay[ii][jj] - height)
						indentations[ii][jj] = maxHeightForShapeDisplay[ii][jj] - height;
				}
			}
		}
	}

	// set new pin heights
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			if (forceN[i][j] <= maxForce) {
				heightsForShapeDisplay[i][j] = indentations[i][j]; //DEFINE NEW PIN HEIGHT
			}
			else {
				heightsForShapeDisplay[i][j] = forceN[i][j] - maxForce;
				newHeights[i][j] = 0.95*heightsFromShapeDisplay[i][j] + 0.05*heightsForShapeDisplay[i][j] + dt*pinVelocity[i][j];
			}
			
		}
	}



	// boundaries
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			if (heightsForShapeDisplay[i][j] < minHeightForShapeDisplay[i][j]) {
				heightsForShapeDisplay[i][j] = minHeightForShapeDisplay[i][j]; //lower boundary
				pinVelocity[i][j] = 0;
			}
			if (heightsForShapeDisplay[i][j] > maxHeightForShapeDisplay[i][j]) {
				heightsForShapeDisplay[i][j] = maxHeightForShapeDisplay[i][j]; // higher boundary (shape)
				pinVelocity[i][j] = 0;
			}
		}
	}


	



}


string MaterialVolumeApp::appInstructionsText() {
	string instructions = (string) "" +
		"This is for testing things."
		"";

	return instructions;
}

void MaterialVolumeApp::keyPressed(int key) {
	switch (key)
	{
	case OF_KEY_UP:
		threshold++;
		isVolumeDataUpdated = true;
		threshold = MIN(threshold, 50);

		cout << "threshold = " << threshold << endl;
		//cam.getTarget().boom(-5);
		break;
	case OF_KEY_DOWN:
		threshold--;
		threshold =  MAX(threshold, 0);

		isVolumeDataUpdated = true;
		cout << "threshold = " << threshold << endl;
		//cam.getTarget().boom(5);
		break;
	case 'p':
		enableProjection = !enableProjection;
		break;
	case 'b':
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
		break;
	}
}