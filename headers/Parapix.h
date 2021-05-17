//SequentialEdgeDetection.cpp definitions
void edgeDetect();
void readImage(char fileName[]);
void writeImage(char fileName[]);

//ParallelEdgeDetection.cpp definitions
void edgeDetection(char filename[],char out[]);

//LOG_Parallel.cpp definitions
void LOGParallel(char fileName[], char outputFileName[]);

//LOG_Serial.cpp definitions
void readImageFile(char fileName[]);
void writeImageFile (char fileName[]);
void LOGSerial();

//BlurParallel.cpp definitions
void blurParallel(char inputName[],char outputName[]);

//BlurSerial definitions
void blurSerial(char inputName[],char outputName[]);