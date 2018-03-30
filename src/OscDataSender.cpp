#include "OscDataSender.h"

OscDataSender::OscDataSender() {
	sender.setup("localhost", OUT_PORT);
}



void OscDataSender::sendHeightData(int32_t heightForShapeDisplay[INFORCE_COLS][INFORCE_ROWS]){
	ofxOscMessage mSend;
	for (int i = 0; i < INFORCE_COLS; i++) {
		for (int j = 0; j < INFORCE_ROWS; j++) {
			mSend.addInt32Arg(heightForShapeDisplay[i][j]);
		}
	}

	mSend.setAddress("/inForce");
	sender.sendMessage(mSend, false);

}


void OscDataSender::sendOperationParameter(int32_t data[], int numberOfData) {
	ofxOscMessage mSend;
	mSend.addInt32Arg(numberOfData);
	for (int i = 0; i < numberOfData; i++) {
			mSend.addInt32Arg(data[i]);
	}

	mSend.setAddress("/operationParameter");
	sender.sendMessage(mSend, false);
}

void OscDataSender::switchOperationMode(int32_t i) {
	ofxOscMessage mSend;
		mSend.addInt32Arg(i);

	mSend.setAddress("/switchOperation");
	sender.sendMessage(mSend, false);
}

void OscDataSender::sendBackAllPinHeight() {
	ofxOscMessage mSend;
	mSend.addInt32Arg(0);

	mSend.setAddress("/sendBackAllPinHeight");
	sender.sendMessage(mSend, false);

}

void OscDataSender::sendAllSameOperationParameter(int32_t data[], int numberOfData) {
	//if (numberOfData > 700)
	ofxOscMessage mSend;
	mSend.addInt32Arg(numberOfData);
	for (int i = 0; i < numberOfData; i++) {
		mSend.addInt32Arg(data[i]);
	}

	mSend.setAddress("/allSameOperationParameter");
	sender.sendMessage(mSend, false);
	Sleep(200);
}

void OscDataSender::sendOperationParameterToSingleArduino(int32_t data[], int numberOfData, int arduinoAddress) {
	ofxOscMessage mSend;
	mSend.addInt32Arg(numberOfData);
	mSend.addInt32Arg(arduinoAddress);
	for (int i = 0; i < numberOfData; i++) {
		mSend.addInt32Arg(data[i]);
	}

	mSend.setAddress("/operationParameterToSingleArduino");
	sender.sendMessage(mSend, false);
	Sleep(200);
}


void OscDataSender::sendPinCalibrationParameter() {
	// Read in data from calibration file and send to arduino
	// Format:
	//    myfile[0] = <numLines = number of lines not including this one>
	//	  myfile[1] = <pin index>
	//    myfile[2] = <upOrDown>
	//    myfile[3] = <height>
	//    myfile[4] = <force>
	//	  myfile[5] = <pin index>
	//    myfile[6] = <upOrDown>
	//    myfile[7] = <height>
	//    myfile[8] = <force>
	//    etc ...
	//	  myfile[numLines-1] = <height>
	//	  myfile[numLines] = <force>
	//
	//    fileData has numLines+1 values
	//    fileData[0]=PARAM_FOR_OPERATION_PIN_CALIBRATION_DATA
	//    fileData[i] = myfile[i], i>=1

	int numMessagesSent = 0;

	for (int address = 0; address < 10; address++) {
		int fileData[MAX_OFXOSCMESSAGE_LENGTH];
		int numRead = 0;

		string line;
		ifstream myfile("calibrationData 3-19-2018.txt");

		getline(myfile, line);
		int numLines = std::stoi(line);

		int pFileData;
		while (numRead < numLines)
		{
			fileData[0] = PARAM_FOR_OPERATION_PIN_CALIBRATION_DATA;

			pFileData = 1;
			while (pFileData < MAX_OFXOSCMESSAGE_LENGTH - 4 && numRead < numLines) {
				// Packing into 1 int_32t

				getline(myfile, line);
				int index = std::stoi(line); // range: [0,49], 6 bits

				getline(myfile, line);
				int upOrDown = std::stoi(line); // range: [-1,1], translated to [0,2], 2 bits

				getline(myfile, line);
				int height = std::stoi(line); // range: [0,130000], divided by 1000 to [0,130], 8 bits

				getline(myfile, line);
				int force = std::stoi(line); // range: [0,2000], 12 bits

				int packed = index;
				packed = packed << 2;
				packed |= (upOrDown + 1);
				packed = packed << 8;
				packed |= (height / 1000);
				packed = packed << 12;
				packed |= force;


				// index = i*ROWS + j
				int col = index / INFORCE_ROWS;
				int row = index % INFORCE_ROWS;
				int dataArduinoAddress = 2 * row + (col + 1) % 2;

				if (address == dataArduinoAddress) {
					fileData[pFileData] = packed;
					pFileData++;
				}
				numRead += 4;
			}
			sendOperationParameterToSingleArduino(fileData, pFileData, address);
			cout << "Sent message of size " << pFileData << " to Arduino " << address << endl;
			numMessagesSent++;
		}
		myfile.close();
	}
	cout << "Finished sending " << numMessagesSent << " pin calibration messages." << endl;
}


void OscDataSender::sendHeightDependentForceParameters(int pinI, int pinJ, int maxHeight, int minHeight, int heightMapping[], int forceMapping[], int numMappings) {
	int index = pinI*INFORCE_ROWS + pinJ;
	int arduinoAddress = 2 * pinJ + (pinI + 1)%2;

	int fileData[MAX_OFXOSCMESSAGE_LENGTH];
	fileData[0] = index;
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

	sendOperationParameterToSingleArduino(fileData, pData, arduinoAddress);
	cout << "Sent height dependent force parameters to arduino " << arduinoAddress << " for pin " << pinI << ", " << pinJ << endl;
}