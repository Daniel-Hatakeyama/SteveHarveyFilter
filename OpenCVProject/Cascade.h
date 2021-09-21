#include <iostream> 
#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

using namespace std;
using namespace cv;

class Cascade {

public:
    vector<Rect> rects;
    vector<Rect> debugRects;
    CascadeClassifier cascade;
    string path;
    Scalar color = Scalar(255, 255, 255);

    double scaleFactor = 1.1;
    int minNeighbors = 4;
    Size minSize = Size(10,10);

public:
    Cascade(string cascadePath) {
        path = cascadePath;
        cascade.load(cascadePath);
    }
    
    void detectMultiScale(Mat grayscaleImage);
    void settings(double _scaleFactor, int _minNeighbors, Size _minSize);
    void generateDebugAllCascades(Mat grayscaleImage);

};
