#include <iostream> 
#include <opencv2/opencv.hpp>
#include <vector>

#include "Cascade.h"
#include "Log.h"

using namespace std;
using namespace cv;

void Cascade::detectMultiScale(Mat grayscaleImage) {

	if (!scaleFactor && !minNeighbors && (minSize == Size(0, 0))) {
		Log::stream << "Apply Settings to Cascade" << endl << Log::printStream();
	}

	cascade.detectMultiScale(grayscaleImage, rects, scaleFactor, minNeighbors, 0, minSize);
}

void Cascade::settings(double _scaleFactor, int _minNeighbors, Size _minSize) {
	scaleFactor = _scaleFactor;
	minNeighbors = _minNeighbors;
	minSize = _minSize;
}

void Cascade::generateDebugAllCascades(Mat grayscaleImage) {
	cascade.detectMultiScale(grayscaleImage, debugRects, 1.1, 0, 0);
}