#include "./headers/Parapix.h"
#include <omp.h>
#include <iostream>
#include <vector>
#include <filesystem>

using namespace std;
namespace fs=std::filesystem;

vector<string> generateFileList(string path){
    vector<string> files;
    for(const auto & file: fs::directory_iterator(path)){
        files.push_back(file.path().filename());
    }
    return files;
}

double calculateParallel(vector<string> files, string inPath, string outPath){
    double total=0;
    bool flag=false;
    for(int i=0;i<files.size();i++){
        double startTime=omp_get_wtime();
        string temp=inPath+files[i];
        char* tempArr=&temp[0];
        string temp2=outPath+files[i];
        char* temp2Arr=&temp2[0];
        edgeDetection(tempArr,temp2Arr,flag);
        total+=omp_get_wtime()-startTime;
    }
    return total;
}

double calculateSequential(vector<string> files, string inPath, string outPath){
    double total=0;
    bool flag=false;
    for(int i=0;i<files.size();i++){
        double startTime=omp_get_wtime();
        string tempPath=inPath+files[i];
        char* tempArray=&tempPath[0];
        string temp2Path=outPath+files[i];
        char* temp2Array=&temp2Path[0];
        readImage(tempArray);
        edgeDetect();
        writeImage(temp2Array);
        total+=omp_get_wtime()-startTime;
    }
    return total;
}

int main(){
    string dataPath="./data/";
    string outPathParallel="./outputs/parallel/";
    string outPathSequential="./outputs/sequential/";
    vector<string> files=generateFileList(dataPath);
    double parallel=calculateParallel(files,dataPath,outPathParallel);
    double seq=calculateSequential(files,dataPath,outPathSequential);
    cout<<"Parallel time on "<<files.size()<<" images: " <<parallel<<endl;
    cout<<"Sequential time on "<<files.size()<<" images: "<<seq<<endl;
}