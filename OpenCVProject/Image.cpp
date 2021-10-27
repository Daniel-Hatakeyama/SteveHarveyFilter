#include <iostream> 
#include <opencv2/opencv.hpp>
#include <math.h>
#include <vector>
#include <string>

#include "Image.h"
#include "Log.h"

#define endlog Log::printStream()

using namespace std;
using namespace cv;

const string Image::frontalFaceCascadePath = ".\\Resources\\HaarCascade\\haarcascade_frontalface_tree_alt.xml";
const string Image::eyeCascadePath = ".\\Resources\\HaarCascade\\haarcascade_eyes_update.xml";
const string Image::animeFaceCascadePath = ".\\Resources\\HaarCascade\\haarcascade_anime_face.xml";
const string Image::animeEyeCascadePath = ".\\Resources\\HaarCascade\\haarcascade_anime_eyes.xml";

Image::Image(string _path) {
	path = _path;
	size = Size(720,720); // Default Size goes here for now i guess

	loadImage(path);

	// CHANGE THESE TO ADJUST SENSITIVITY

	// Face Cascade Settings :
	faceCascade.settings(1.05, 5, Size(40, 40));
	eyeCascade.settings(1.1, 6, Size(25, 25));
	animeFaceCascade.settings(1.05, 5, Size(40, 40));
	animeEyeCascade.settings(1.1, 3, Size(25, 25));

	// Face Cascade Colors :
	faceCascade.color = Scalar(255, 0, 0);
	eyeCascade.color = Scalar(255, 0, 255);
	animeFaceCascade.color = Scalar(0, 255, 0);
	animeEyeCascade.color = Scalar(0, 0, 255);

}

void Image::generateAll() {
	generateNormalizedImage();
	generateGrayscaleImage();
	generateCascades();
	generateFaceImage();
	//generateProfileImage(); ?? TODO
}

// Reset Function with New Path
void Image::loadImage(string _path) {

	path = _path;
	name = path.substr(path.rfind('\\') + 1, path.rfind('.') - path.rfind('\\') - 1);
	ext = path.substr(path.rfind('.'));

	Log::stream << "Load Image : \"" << path << "\" : " << endlog;

	// Reset Data Checks :
	original.release();
	checkForOriginal = false;
	normalized.release();
	checkForNormalized = false;
	grayscale.release();
	checkForGrayscale = false;
	checkForCascades = false;
	faceImage.release();
	checkForFaceImage = false;
	profileImage.release();
	checkForProfileImage = false;
	debugImage.release();
	checkForDebugImage = false;

	original = imread(path);

	if (original.empty()) {
		Log::stream << "[-Failed-]" << endl << endlog;
	}
	else {
		Log::stream << "[-Successful-]" << endl << endlog;
		checkForOriginal = true;
	}

	Log::popKey();
}

void Image::generateNormalizedImage() {

	// Requires Original :
	Log::stream << "Normalized Image : " << endlog;
	if (!checkForOriginal) {
		Log::stream << "[-Failed-] (CheckForOriginal required)" << endl << endlog;
	}

	// Requires Size Property
	if (size == Size(0, 0)) {
		Log::stream << "[-Failed-] (Invalid Size)" << endl << endlog;
	}

	Size targetSize = size;

	normalized = original.clone();
	// If image width > image height
	if (original.cols > original.rows) {
		// Change the width to target size

		// Aspect Height
		int aspectHeight = (int)(((float)targetSize.width / (float)normalized.cols) * (float)normalized.rows);

		// If width > target size -> Shrinking
		if (original.cols > targetSize.width) {
			resize(normalized, normalized, Size(targetSize.width, aspectHeight), INTER_AREA);
		}
		else {
			resize(normalized, normalized, Size(targetSize.width, aspectHeight), INTER_CUBIC);
		}
	}
	else {
		// Change the height to target size

		// Aspect Width
		int aspectWidth = (int)(((float)targetSize.height / (float)normalized.rows) * (float)normalized.cols);

		// If height > target size -> Shrinking
		if (original.cols > targetSize.width) {
			resize(normalized, normalized, Size(aspectWidth, targetSize.height), INTER_AREA);
		}
		else {
			resize(normalized, normalized, Size(aspectWidth, targetSize.height), INTER_CUBIC);
		}
	}
	Log::stream << "[" << normalized.rows << "," << normalized.cols << "] : " << "[-Successful-]" << endl;
	debugImage = normalized.clone();
	faceImage = normalized.clone();
	checkForNormalized = true;
}

void Image::generateGrayscaleImage() {

	// Requires Normalized :
	Log::stream << "Grayscale Image : " << endlog;
	if (!checkForNormalized) {
		Log::stream << "[-Failed-] (checkForNormalized required)" << endl << endlog;
		return;
	}

	cvtColor(normalized, grayscale, COLOR_BGR2GRAY);
	Log::stream << "[-Successful-]" << endl << endlog;
	checkForGrayscale = true;
}

void Image::generateCascades() {
	Log::pushKey("CASCADE");
	Log::stream << "Detetect Multi Scale : Cascading : " << endlog;

	// Requires grayscale :
	if (!checkForGrayscale) {
		Log::stream << "[-Failed-] (checkForGrayscale required)" << endl << endlog;
		return;
	}

	// Run Face Detection :
	faceCascade.detectMultiScale(grayscale); Log::print("-");
	animeFaceCascade.detectMultiScale(grayscale); Log::print("-");
	// Run Eye Detection :
	eyeCascade.detectMultiScale(grayscale); Log::print("-");
	animeEyeCascade.detectMultiScale(grayscale); Log::print("-");

	Log::stream << " : [-Successful-]" << endl << endlog;
	checkForCascades = true;

	Log::popKey(); // CASCADE
}

void Image::generateFaceImage() {
	
	Log::pushKey("FACE");
	Log::stream << "Face Image : " << endlog;
	
	// Requires cascades
	if (!checkForCascades) {
		Log::stream << "[-Failed-] (checkForCascades required)" << endl << endlog;
		return;
	}

	// Validate Features :
	if (faceCascade.rects.size() > 0) {
		Rect largestFace;
		int numEyes = eyesInLargestFace(faceCascade.rects, eyeCascade.rects, largestFace);

		// JANK SOLUTION FOR WHEN ANIME EYES IN REAL FACE
		/*if (numEyes == 0) {
			numEyes = eyesInLargestFace(faceCascade.rects, animeEyeCascade.rects, largestFace);
		}*/

		if (numEyes == 2) {
			Log::stream << "Requirements [default face] have been met : [-Successful-] " << endlog;
			checkForFaceImage = true;
			// Requirements have been met to draw features
			Rect face = largestFace;
			Rect eyeL = (eyeCascade.rects.at(0).x < eyeCascade.rects.at(1).x) ? eyeCascade.rects.at(0) : eyeCascade.rects.at(1);
			Rect eyeR = (eyeL == eyeCascade.rects.at(1)) ? eyeCascade.rects.at(0) : eyeCascade.rects.at(1);

			Log::pushKey("DRAW_FACE");
			drawFace(face, eyeL, eyeR);
			Log::popKey(); // DRAW_FACE
		}
	}
	if (animeFaceCascade.rects.size() > 0) {
		Rect largestAnimeFace;
		int numAnimeEyes = eyesInLargestFace(animeFaceCascade.rects, animeEyeCascade.rects, largestAnimeFace);
		
		// JANK SOLUTION FOR WHEN REAL EYES IN ANIME FACE
		/*if (numAnimeEyes == 0) {
			numAnimeEyes = eyesInLargestFace(animeFaceCascade.rects, eyeCascade.rects, largestAnimeFace);
		}*/

		if (numAnimeEyes == 2) {
			Log::stream << "Requirements [anime face] have been met : [-Successful-] " << endlog;
			checkForFaceImage = true;
			// Requirements have been met to draw features
			Rect face = largestAnimeFace;
			Rect eyeL = (animeEyeCascade.rects.at(0).x < animeEyeCascade.rects.at(1).x) ? animeEyeCascade.rects.at(0) : animeEyeCascade.rects.at(1);
			Rect eyeR = (eyeL == animeEyeCascade.rects.at(1)) ? animeEyeCascade.rects.at(0) : animeEyeCascade.rects.at(1);

			Log::pushKey("DRAW_FACE");
			drawFace(face, eyeL, eyeR);
			Log::popKey(); // DRAW_FACE
		}
	}
	Log::print("\n");
	Log::popKey(); // FACE
}

int Image::eyesInLargestFace(vector<Rect>& faceRects, vector<Rect>& eyeRects, Rect& largestFace) {

	int maxSize = -1;
	int maxIndex = -1;
	int eyesInFace = 0;

	// Find largestFace
	for (int i = 0; i < faceRects.size(); i++) {
		Rect rect = faceRects.at(i);
		if (rect.area() > maxSize) {
			maxSize = rect.area();
			maxIndex = i;
		}
	}
	// If largestFace found :
	if (maxIndex >= 0) {
		largestFace = faceRects.at(maxIndex);
		// Find all eyes in largestFace
		for (int i = 0; i < eyeRects.size();) {
			Rect eyeRect = eyeRects.at(i);
			// If eye indide face :
			if (rectContainsRectCenter(largestFace, eyeRect)) {
				eyesInFace++;
				i++;
			}
			else {
				// Erase eye not inside face
				eyeRects.erase(eyeRects.begin() + i);
			}
		}
	}
	return eyesInFace;
}

void Image::drawFace(Rect face, Rect eyeL, Rect eyeR) {

	// Mat src;
	// Canny Testing Only
	// GaussianBlur(image, src, Size(5, 5), 3, 0);
	// Canny(src, image, 25, 75);

	// Eye Scale :
	double eyeScale = 1.5;

	// Colors :
	Scalar eyeWhiteColor = Scalar(255, 255, 255);
	Scalar eyeOutlineColor = Scalar(50, 50, 50);
	Scalar eyePupilColor = Scalar(0, 0, 0);

	// Draw Left Eye :
	int eyeLeftX = eyeL.x + eyeL.width / 2, eyeLeftY = eyeL.y + eyeL.height / 2, eyeLeftR = eyeL.width / 2;
	
	// Draw Right Eye :
	int eyeRightX = eyeR.x + eyeR.width / 2, eyeRightY = eyeR.y + eyeR.height / 2, eyeRightR = eyeR.width / 2;
	
	// Configure Rand for Offset
	srand(time(NULL));
	float maxOffsetX = 3; // 1/maxOffset = %ofRadius offset
	float maxOffsetY = 4; // 1/maxOffset = %ofRadius offset

	double dialation = 3.5; // 1/dialation = %ofRadius pupil

	// Draw Left Pupil
	int pupilLeftR = eyeLeftR / dialation * eyeScale;
	int leftOffsetX = -1 * (rand() % (int)(eyeLeftR / maxOffsetX));
	int leftOffsetY = (rand() % (int)(2 * eyeLeftR / maxOffsetY)) - (int)(eyeLeftR / maxOffsetY);

	// Draw Right Pupil
	int pupilRightR = eyeRightR / dialation * eyeScale;
	int RightOffsetX = rand() % (int)(eyeRightR / maxOffsetX);
	int RightOffsetY = (rand() % (int)(2 * eyeRightR / maxOffsetY)) - (int)(eyeRightR / maxOffsetY);

	// Calculate mouth angle :
	line(debugImage, Point(eyeRightX, eyeRightY), Point(eyeLeftX, eyeLeftY), Scalar(255, 0, 0)); // Eye line

	Point eyeCenter = Point((eyeRightX + eyeLeftX) / 2, (eyeRightY + eyeLeftY) / 2);
	circle(debugImage, eyeCenter, 3, Scalar(0,0,0), -1); // Eye center

	double lengthScale = 0.8;
	
	int eyeYDist = eyeLeftY - eyeRightY;
	int eyeXDist = eyeRightX - eyeLeftX;

	Point endPoint = Point(eyeCenter.x + eyeYDist, eyeCenter.y + eyeXDist);
	Point lengthPoint = Point(eyeCenter.x + lengthScale * (eyeYDist), eyeCenter.y + lengthScale * (eyeRightX - eyeLeftX));
	line(debugImage, eyeCenter, endPoint, Scalar(255, 255, 255)); // Eye perpendicular

	double angle = -atan((double)(eyeLeftY - eyeRightY) / (double)(eyeRightX - eyeLeftX)) * 180 / 3.141592;
	int width = (eyeXDist + eyeScale * (eyeRightR + eyeLeftR));
	int height = width / 3;

	RotatedRect teethBound = RotatedRect(Point(lengthPoint.x - (0.11*eyeXDist), lengthPoint.y + (0.13*eyeXDist)) , Size(width * 0.6, height * 0.9), angle);
	RotatedRect mouthBound = RotatedRect(lengthPoint, Size(width, height), angle);

	// Draw Teeth
	ellipse(faceImage, teethBound, Scalar(255, 255, 255), -1);
	ellipse(faceImage, teethBound, eyeOutlineColor, 1);
	// Draw Mustache
	ellipse(faceImage, mouthBound, Scalar(0, 0, 0), -1);
	// Draw Left Eye
	circle(faceImage, Point(eyeLeftX, eyeLeftY), eyeLeftR * eyeScale, eyeWhiteColor, -1);
	circle(faceImage, Point(eyeLeftX, eyeLeftY), eyeLeftR * eyeScale, eyeOutlineColor, 1);
	// Draw Right Eye
	circle(faceImage, Point(eyeRightX, eyeRightY), eyeRightR * eyeScale, eyeWhiteColor, -1);
	circle(faceImage, Point(eyeRightX, eyeRightY), eyeRightR * eyeScale, eyeOutlineColor, 1);
	// Draw Left Pupil
	circle(faceImage, Point(eyeLeftX + leftOffsetX, eyeLeftY + leftOffsetY), pupilLeftR, eyePupilColor, -1);
	// Draw Right Pupil
	circle(faceImage, Point(eyeRightX + RightOffsetX, eyeRightY + RightOffsetY), pupilRightR, eyePupilColor, -1);
	// Draw Eyebrows

}

void Image::drawDebugCascades() {

	if (!checkForCascades) return;

	drawRectList(debugImage, faceCascade.rects, faceCascade.color);
	drawRectList(debugImage, eyeCascade.rects, eyeCascade.color);
	drawRectList(debugImage, animeFaceCascade.rects, animeFaceCascade.color);
	drawRectList(debugImage, animeEyeCascade.rects, animeEyeCascade.color);

}

void Image::drawDebugAllCascades() {

	faceCascade.generateDebugAllCascades(grayscale);
	eyeCascade.generateDebugAllCascades(grayscale);
	animeFaceCascade.generateDebugAllCascades(grayscale);
	animeEyeCascade.generateDebugAllCascades(grayscale);

	drawRectList(debugImage, faceCascade.debugRects, faceCascade.color);
	drawRectList(debugImage, eyeCascade.debugRects, eyeCascade.color);
	drawRectList(debugImage, animeFaceCascade.debugRects, animeFaceCascade.color);
	drawRectList(debugImage, animeEyeCascade.debugRects, animeEyeCascade.color);

}

void Image::drawRectList(Mat image, vector<Rect> rects, Scalar color) {
	for (Rect rect : rects) {
		// Draw Rectangle :
		int x = rect.x, y = rect.y, w = rect.width - 1, h = rect.height - 1;
		rectangle(image, Point(cvRound(x), cvRound(y)), Point(cvRound(x + w), cvRound(y + h)), color);
	}
}

// If Child fully inside Parent :
bool Image::rectContainsRect(Rect parent, Rect child) {
	// Check Bounds
	if ((child.x + child.width) <= (parent.x + parent.width) && (child.x) >= (parent.x) && (child.y) >= (parent.y) && (child.y + child.height) <= (parent.y + parent.height)) {
		return true;
	} else {
		return false;
	}
}

// If Child Center fully inside Parent :
bool Image::rectContainsRectCenter(Rect parent, Rect child) {

	// Calculate Center
	int childX = child.x + (child.width / 2);
	int childY = child.y + (child.height / 2);

	// Check Bounds
	if (childX <= (parent.x + parent.width) && childX >= parent.x && childY >= parent.y && childY <= (parent.y + parent.height)) {
		return true;
	}
	else {
		return false;
	}
}

vector<Rect> Image::runFaceCascade() {
	faceCascade.detectMultiScale(grayscale);
	return faceCascade.rects;

}
vector<Rect> Image::runEyeCascade() {
	eyeCascade.detectMultiScale(grayscale);
	return eyeCascade.rects;

}
vector<Rect> Image::runAnimeFaceCascade() {
	animeFaceCascade.detectMultiScale(grayscale);
	return animeFaceCascade.rects;

}
vector<Rect> Image::runAnimeEyeCascade() {
	animeEyeCascade.detectMultiScale(grayscale);
	return animeEyeCascade.rects;

}

// TODO
// void setDefaultSize(Size _size) {}