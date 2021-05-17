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

double calculateParallelEdge(vector<string> files, string inPath, string outPath){
    double total=0;
    std::cout<<std::endl;
    for(int i=0;i<files.size();i++){
        double startTime=omp_get_wtime();
        string temp=inPath+files[i];
        char* tempArr=&temp[0];
        string temp2=outPath+files[i];
        char* temp2Arr=&temp2[0];
        edgeDetection(tempArr,temp2Arr);
        int percent=(100 * (i + 1)) / files.size();
        std::cout << "\r" << "[" << std::string(percent / 5, (char)254u) << std::string(100 / 5 - percent / 5, ' ') << "]";
		std::cout << percent << "%" << " [Image " << i + 1 << " of " << files.size() << "]";
        total+=omp_get_wtime()-startTime;
    }
    return total;
}

double calculateSerialEdge(vector<string> files, string inPath, string outPath){
    double total=0;
    std::cout<<std::endl;
    for(int i=0;i<files.size();i++){
        double startTime=omp_get_wtime();
        string tempPath=inPath+files[i];
        char* tempArray=&tempPath[0];
        string temp2Path=outPath+files[i];
        char* temp2Array=&temp2Path[0];
        seqEdge(tempArray,temp2Array);
        int percent=(100 * (i + 1)) / files.size();
        std::cout << "\r" << "[" << std::string(percent / 5, (char)254u) << std::string(100 / 5 - percent / 5, ' ') << "]";
		std::cout << percent << "%" << " [Image " << i + 1 << " of " << files.size() << "]";
        total+=omp_get_wtime()-startTime;
    }
    return total;
}

double calculateParallelLOG(vector<string> files,string inPath, string outPath){
    double total=0;
    std::cout<<std::endl;
    for(int i=0;i<files.size();i++){
        double startTime=omp_get_wtime();
        string path=inPath+files[i];
        char* tempArray=&path[0];
        string path2=outPath+files[i];
        char* temp2Array=&path2[0];
        LOGParallel(tempArray,temp2Array);
        int percent=(100 * (i + 1)) / files.size();
        std::cout << "\r" << "[" << std::string(percent / 5, (char)254u) << std::string(100 / 5 - percent / 5, ' ') << "]";
		std::cout << percent << "%" << " [Image " << i + 1 << " of " << files.size() << "]";
        total+=omp_get_wtime()-startTime; 
    }
    return total;
}

double calculateSerialLOG(vector<string> files, string inPath, string outPath){
    double total=0;
    std::cout<<std::endl;
    for(int i=0;i<files.size();i++){
        double startTime=omp_get_wtime();
        string path=inPath+files[i];
        char* tempArray=&path[0];
        string path2=outPath+files[i];
        char* temp2Array=&path2[0];
        readImageFile(tempArray);
        LOGSerial();
        writeImageFile(temp2Array);
        int percent=(100 * (i + 1)) / files.size();
        std::cout << "\r" << "[" << std::string(percent / 5, (char)254u) << std::string(100 / 5 - percent / 5, ' ') << "]";
		std::cout << percent << "%" << " [Image " << i + 1 << " of " << files.size() << "]";
        total+=omp_get_wtime()-startTime;
    }
    return total;
}

double calculateParallelBlur(vector<string> files, string inPath, string outPath){
    double total=0;
    std::cout<<std::endl;
    for(int i=0;i<files.size();i++){
        double startTime=omp_get_wtime();
        string path=inPath+files[i];
        char* tempArray=&path[0];
        string path2=outPath+files[i];
        char* temp2Array=&path2[0];
        blurParallel(tempArray,temp2Array);
        int percent=(100 * (i + 1)) / files.size();
        std::cout << "\r" << "[" << std::string(percent / 5, (char)254u) << std::string(100 / 5 - percent / 5, ' ') << "]";
		std::cout << percent << "%" << " [Image " << i + 1 << " of " << files.size() << "]";
        total+=omp_get_wtime()-startTime;
    }
    return total;
}

double calculateSerialBlur(vector<string> files, string inPath, string outPath){
    double total=0;
    std::cout<<std::endl;
    for(int i=0;i<files.size();i++){
        double startTime=omp_get_wtime();
        string path=inPath+files[i];
        char* tempArray=&path[0];
        string path2=outPath+files[i];
        char* temp2Array=&path2[0];
        blurSerial(tempArray,temp2Array);
        int percent=(100 * (i + 1)) / files.size();
        std::cout << "\r" << "[" << std::string(percent / 5, (char)254u) << std::string(100 / 5 - percent / 5, ' ') << "]";
		std::cout << percent << "%" << " [Image " << i + 1 << " of " << files.size() << "]";
        total+=omp_get_wtime()-startTime;
    }
    return total;
}

double calculateSerialStretch(vector<string> files, string inPath, string outPath){
    double total=0;
    std::cout<<std::endl;
    for(int i=0;i<files.size();i++){
        double startTime=omp_get_wtime();
        string path=inPath+files[i];
        char* tempArray=&path[0];
        string path2=outPath+files[i];
        char* temp2Array=&path2[0];
        contrastStretchSerial(tempArray,temp2Array);
        int percent=(100 * (i + 1)) / files.size();
        std::cout << "\r" << "[" << std::string(percent / 5, (char)254u) << std::string(100 / 5 - percent / 5, ' ') << "]";
		std::cout << percent << "%" << " [Image " << i + 1 << " of " << files.size() << "]";
        total+=omp_get_wtime()-startTime;
    }
    return total;
}

double calculateParallelStretch(vector<string> files, string inPath, string outPath){
    double total=0;
    std::cout<<std::endl;
    for(int i=0;i<files.size();i++){
        double startTime=omp_get_wtime();
        string path=inPath+files[i];
        char* tempArray=&path[0];
        string path2=outPath+files[i];
        char* temp2Array=&path2[0];
        contrastStretchParallel(tempArray,temp2Array);
        int percent=(100 * (i + 1)) / files.size();
        std::cout << "\r" << "[" << std::string(percent / 5, (char)254u) << std::string(100 / 5 - percent / 5, ' ') << "]";
		std::cout << percent << "%" << " [Image " << i + 1 << " of " << files.size() << "]";
        total+=omp_get_wtime()-startTime;
    }
    return total;
}

int main(){
    string dataPath="./data/test_data/test/";
    string outPathParallel="./outputs/parallel/";
    string outPathSequential="./outputs/sequential/";
    vector<string> files=generateFileList(dataPath);
}