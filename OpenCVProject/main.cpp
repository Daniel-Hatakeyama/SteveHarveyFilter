#include <iostream> 
#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <filesystem>
#include <vector>
#include <time.h>
#include "Image.h"

namespace fs = std::filesystem; // Needs C++17

// Headers :
bool generateInputFileList(std::vector <std::string>& fileList, std::string inputPath);
void generateProfileImages(const std::vector<std::string>& inFiles, std::vector<Image>& images, cv::Size profileSize);
inline void keyContinue();
inline void printFileList(const std::vector<std::string>& fileList);
inline bool exists(const std::string& name);
inline bool validateExtension(const std::string& path);

int main() {
	using namespace std;
	using namespace cv;

	// Log Level : Errors
	utils::logging::setLogLevel(utils::logging::LogLevel::LOG_LEVEL_ERROR);

	// Settings / Input :
	Size profileSize = Size(720, 720);
	string inputPath = "Resources\\Images\\";
	string outputPath = ".\\Resources\\Output\\calliope_face.jpg";

	// Variables
	vector<string> inFiles;
	vector<string> outFiles;
	vector<Image> images;

	// Generate Heve Code :
	// 
	// Get Input :
	generateInputFileList(inFiles, inputPath);
	printFileList(inFiles);

	// Generate Image Data :
	cout << endl << "Generating : " << endl << endl;
	generateProfileImages(inFiles, images, profileSize);
	
	// Show Images :
	for (Image image : images) {
		imshow(image.path, image.debugImage);
		keyContinue();
		imshow(image.name, image.faceImage);
		keyContinue();

		string writePath = outputPath + image.name;
		imwrite(writePath, image.faceImage);
	}
	
	/*
	// Display Output :
	generateInputFileList(outFiles, outputPath);
	for (string path : outFiles) {
		images.push_back(Image(path));
	}
	for (Image image : images) {
		imshow(image.name, image.original);
		keyContinue();
	}*/

	keyContinue();
	return 0;
}

void generateProfileImages(const std::vector<std::string>& inFiles, std::vector<Image>& images, cv::Size profileSize) {
	using namespace std;
	using namespace cv;

	int count = 0;
	for (string path : inFiles) {
		
		cout << "Image " << ++count << "/" << inFiles.size() << " : " << images.size() << " Positive Match" << endl;
		Image image = Image(path, profileSize);
		
		image.generateAll();
		image.drawDebugCascades();
		// image.drawDebugAllCascades();

		if (image.checkForFaceImage) {
			images.push_back(image);
			cout << "[Success]" << endl << endl;
		}
		else {
			cout << "[Fail]" << endl << endl;
		}
	}
}

bool generateInputFileList(std::vector <std::string>& fileList, std::string inputPath) {
	using namespace cv;
	using namespace std;

	// Validate Path :
	bool isFile = inputPath.find(".") < inputPath.npos;
	bool isDirectory = inputPath.find("\\") < inputPath.npos;
	bool ifExists = exists(inputPath);

	// Input is a file :
	if (isFile && ifExists) {
		fileList.push_back(inputPath);
	}
	// Input is a directory :
	else if (isDirectory && ifExists) {
		for (const auto& dirItem : fs::directory_iterator(inputPath)) {
			string pathName = dirItem.path().string();
			fileList.push_back(pathName);
		}
	}
	// Input is invalid :
	else {
		cout << "Enter Valid Path" << endl;
		return false;
	}

	// Validate paths in file list :
	for (auto path = fileList.begin(); path != fileList.end();) {
		// Get file extension :
		int extIndex = path->rfind(".");
		if (extIndex != path->npos) {
			// Make fExtension lowercase :
			transform(path->begin() + extIndex, path->end(), path->begin() + extIndex, ::tolower);

			if (validateExtension(*path)) {
				path++;
				continue;
			}
			else {
				path = fileList.erase(path);
				continue;
			}
		}
		else {
			// Remove items without file extension :
			path = fileList.erase(path);
			continue;
		}
	}
	if (fileList.size() == 0) {
		// No files found
		return false;
	}
	else {
		return true;
	}
}

inline void keyContinue() {
	cv::waitKey();
	cv::destroyAllWindows();
}

inline bool exists(const std::string& name) {
	// Check if File Exists in Directory : https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-14-17-c
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

inline void printFileList(const std::vector<std::string>& fileList) {
	
	std::cout << "File List : " << endl;
	std::cout << "----------------------------------------" << endl;
	for (string file : fileList) {
		std::cout << file << endl;
	}
	std::cout << "----------------------------------------" << endl;
}

inline bool validateExtension(const std::string& path) {
	if (path.ends_with(".png")) {
		return true;
	}
	else if (path.ends_with(".jpg")) {
		return true;
	}
	else if (path.ends_with(".jpeg")) {
		return true;
	}
	else {
		return false;
	}
}