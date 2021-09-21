#include <iostream>
#include "Cascade.h"

using namespace std;
using namespace cv;

class Image {

private:
    static const string frontalFaceCascadePath;
    static const string eyeCascadePath;
    static const string animeFaceCascadePath;
    static const string animeEyeCascadePath;
    static const Size defaultSize;

public:
    Cascade faceCascade = Cascade(frontalFaceCascadePath);
    Cascade eyeCascade = Cascade(eyeCascadePath);
    Cascade animeFaceCascade = Cascade(animeFaceCascadePath);
    Cascade animeEyeCascade = Cascade(animeEyeCascadePath);
    Mat original, normalized, grayscale, faceImage, profileImage, debugImage;
    string path;
    string name;
    Size size;

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
    Image(string _path, Size _size = Size(720, 720));
    void loadImage(string path);

    void generateAll();
    void generateNormalizedImage(Size targetSize = defaultSize);
    void generateGrayscaleImage();
    void generateCascades();
    void generateFaceImage();
    //void generateProfileImage();

    // Debug :
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
