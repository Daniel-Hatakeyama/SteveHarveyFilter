#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>

#include "Cascade.h"

#pragma once

using namespace std;
using namespace cv;

class Image {

private:
    static const string frontalFaceCascadePath;
    static const string eyeCascadePath;
    static const string animeFaceCascadePath;
    static const string animeEyeCascadePath;

public:
    string path, name, ext;
    Size size;
    Mat original, normalized, grayscale, faceImage, profileImage, debugImage;
    Cascade faceCascade = Cascade(frontalFaceCascadePath);
    Cascade eyeCascade = Cascade(eyeCascadePath);
    Cascade animeFaceCascade = Cascade(animeFaceCascadePath);
    Cascade animeEyeCascade = Cascade(animeEyeCascadePath);

public:
    bool checkForOriginal = false;
    bool checkForNormalized = false;
    bool checkForGrayscale = false;
    bool checkForCascades = false;
    bool checkForFaceImage = false;
    bool checkForProfileImage = false;
    bool checkForDebugImage = false;
    
public:
    // Constructor
    Image(string _path);
    void loadImage(string _path);

    void generateAll();
    void generateNormalizedImage();
    void generateGrayscaleImage();
    void generateCascades();
    void generateFaceImage();
    //void generateProfileImage(); TODO

    void drawDebugCascades();
    void drawDebugAllCascades();

private:

    vector<Rect> runFaceCascade();
    vector<Rect> runEyeCascade();
    vector<Rect> runAnimeFaceCascade();
    vector<Rect> runAnimeEyeCascade();

    int eyesInLargestFace(vector<Rect>& faceRects, vector<Rect>& eyeRects, Rect& largestFace);

    void drawFace(Rect face, Rect eyeL, Rect eyeR);
    void drawRectList(Mat image, vector<Rect> rects, Scalar color = Scalar(255,255,255));

    bool rectContainsRect(Rect parent, Rect child);
    bool rectContainsRectCenter(Rect parent, Rect child);

};
