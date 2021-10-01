#include <iostream> 
#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

#pragma once

using namespace std;
using namespace cv;

class Cascade {

public:
    vector<Rect> rects;
    vector<Rect> debugRects;
    CascadeClassifier cascade;
    string path;
    Scalar color = Scalar(255, 255, 255);

    double scaleFactor = 0;
    int minNeighbors = 0;
    Size minSize = Size(0,0);

public:
    Cascade(string cascadePath) {
        path = cascadePath;
        cascade.load(cascadePath);
    }
    
    void detectMultiScale(Mat grayscaleImage);
    void settings(double _scaleFactor, int _minNeighbors, Size _minSize);
    void generateDebugAllCascades(Mat grayscaleImage);

};
