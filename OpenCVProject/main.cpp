/* ------------------------------ Created By Daniel Hatakeyama ------------------------------ */
/* ------------------------------ Steve Harvey Image Generator ------------------------------ */

#include <iostream> 
#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <filesystem>
#include <vector>
#include <time.h>
#include "Image.h"
namespace fs = std::filesystem; // Requires C++17


#define minInt -1000000;

/* ---------------------------------------- Headers ---------------------------------------- */

inline void generateInputFileList(std::vector <std::string>& inFiles, const std::vector <std::string>& outFiles, const std::string path);
inline void generateOutputFileList(std::vector <std::string>& fileList, const std::string path, bool& isValid);
void generateProfileImages(const std::vector<std::string>& inFiles, const bool& validOutput);
inline void displayOutput(const std::vector<string>& outFiles);
inline void keyContinue();
inline void printFileList(const std::vector<std::string>& fileList, std::string name = "", std::string path = "");
inline bool exists(const std::string& name);
inline bool validateExtension(const std::string& path);
void ioHandler(std::vector<std::string>& inFiles, std::vector<std::string>& outFiles, const std::string& inputPath, const std::string& outputPath, bool& validOutput);

/* ---------------------------------------- SETTINGS ---------------------------------------- */

// Profile Parameters :
const Size profileSize = Size(720, 720); // TODO This dont do shit rn xd // This is somewhere in cascade or image idk, which is fine, but def address cropping in general

// FileIO : File/Folder Input : Folder Output
const string inputPath = ".\\Resources\\Input\\"; //"\\Users\\djhat\\OneDrive\\Pictures\\Input_Heve\\";
const string outputPath = "\\Users\\djhat\\OneDrive\\Pictures\\Output_Heve\\";
const string failPath = ".\\Resources\\Failures\\"; // Optional : ! THERE ARE NO CHECKS ON THIS SO BE CAREFUL !

// Input Settings
const bool deleteFailures = true;      // Deletes negative heve profiles from input path : Quickens Future Runs
const bool storeFailures = true;       // Only set if failPath is valid : ! THERE ARE NO CHEKS ON THIS SO BE CAREFUL !
const bool deleteSuccesses = false;    // Delete positive heve profiles from input path : Use overrideDuplicates instead for 95% of cases


// Ouput Settings
const bool storeImage = true;		   // If store image in outputDestination
const bool overrideDuplicates = false; // Generate item even if duplicate already exists in output
const bool showOutput = true;	       // Show all contents of output 

// Display Settings
const bool displayLog = false;		   // Display each image as it is generated
const bool showDebugImage = false ;	   // Debug draw all rectangle cascades
const bool showCascadeImage = true;    // Draw final cascades
const bool showProfileImage = true;    // Draw final image
const bool skipFails = false;		   // Draw negative matches

// Print Settings
const bool printValidate = false;	   // Debug Input Path Validation

/* -------------------------------------- TODO LIST ------------------------------------- */
/* 
* generateProfileImage() [Crop PFP around face]
* ~Image() , ~Cascade() [Avoid Memory Leaks]
* Increase Speed further: Optimise construction
* Determine once and for all if you should be dynamically allocating to improve performance.
* Add Static Size Variable
* Move Helper / Condenser Functions to Seperate File
*/
/* ---------------------------------------- Main ---------------------------------------- */

int main() {
	using namespace std;
	using namespace cv;
	utils::logging::setLogLevel(utils::logging::LogLevel::LOG_LEVEL_ERROR); // Log Level : Errors :

	// Persistent Variables :
	vector<string> inFiles;
	vector<string> outFiles;
	bool validOutput; // True if outputPath exists

	// Input / Output Setup :
	ioHandler(inFiles, outFiles, inputPath, outputPath, validOutput);

	// GENERATE 
	generateProfileImages(inFiles, validOutput);
	
	// Display Output :
	displayOutput(outFiles);
}

/* ---------------------------------------- Functions ---------------------------------------- */

/// <summary>
/// Super-impose Steve Harvey on all input : Save valid generated image profiles into output folder : Uses haarcascades with OpenCV library 
/// </summary>
/// <param name="inFiles"> : Validated Input File Paths </param>
/// <param name="validOutput"> : If output directory exists (might change how this works eventualy) </param>
void generateProfileImages(const std::vector<std::string>& inFiles, const bool& validOutput) {
	using namespace std;
	using namespace cv;

	cout << endl << "[Generating] : " << endl << endl;

	int count = 0;
	int successCount = 0;
	for (string path : inFiles) {

		// PREPARE :
		cout << "Constructing Image [" << ++count << "/" << inFiles.size() << "] [" << successCount << " successes]" << endl;
		Image image = Image(path);

		// GENERATE :
		image.generateAll();
		image.drawDebugCascades();
		if (showDebugImage) image.drawDebugAllCascades();

		// EVALUATE :
		if (image.checkForFaceImage) {
			// LOG :
			if (displayLog) { 
				if (showDebugImage || showCascadeImage) {
					imshow(image.name, image.debugImage); keyContinue();
				}
				if (showProfileImage) {
					imshow(image.name, image.faceImage); keyContinue();
				}
			}
			// SAVE :
			if (storeImage && validOutput) {
				string writePath = outputPath + image.name + image.ext;
				imwrite(writePath, image.faceImage);
				cout << "Saving \"" << image.name << image.ext << "\" in \"" << outputPath << "\"" << endl;
			} else if (storeImage) {
				cout << "Invalid Output Directory [Could not save image]" << endl;
			}

			if (deleteSuccesses) {
				if (!remove(path.c_str())) {
					cout << "Deleting \"" << path << "\" from input path" << endl;
				}
				else {
					cout << "Error Deleting \"" << path << "\" from input path" << endl;
				}
			}

			successCount++; // Used for debug printing
			cout << "[Success]" << endl << endl;
		} else {
			// LOG :
			if (displayLog && !skipFails) { // Display Logging : (Failed Image)
				if (showDebugImage || showCascadeImage) {
					imshow(image.name, image.debugImage); keyContinue();
				}
				if (showProfileImage) {
					imshow(image.name, image.normalized); keyContinue();
				}
			}
			// Save Fail into Fail Folder : ! THERE ARE NO CHECKS SO BE CAREFUL ! // TODO Add checks for fail folder [Low Priority]
			if (storeFailures) {
				string failurePath = failPath + image.name + image.ext;
				imwrite(failurePath, image.normalized);
				cout << "Storing Fail \"" << image.name << image.ext << "\" in \"" << failurePath << "\"" << endl;
			}
			// Delete Fail From Input
			if (deleteFailures) {
				if (!remove(path.c_str())) {
					cout << "Deleting \"" << path << "\" from input path" << endl;
				}
				else {
					cout << "Error Deleting \"" << path << "\" from input path" << endl;
				}
			}

			cout << "[Fail]" << endl << endl;
		}
	}
	cout << "[Complete] : " << endl << endl;
}

// Container to generate, validate, parse input and output directory.
void ioHandler(std::vector<std::string>& inFiles, std::vector<std::string>& outFiles, const std::string& inputPath, const std::string& outputPath, bool& validOutput) {

	// Get output for duplicate check :
	generateOutputFileList(outFiles, outputPath, validOutput);
	generateInputFileList(inFiles, outFiles, inputPath);

	printFileList(inFiles, "Input File List", inputPath);
	printFileList(outFiles, "Output File List", outputPath);
}

// Parse valid file paths from directory OR file input
inline void generateInputFileList(std::vector <std::string>& fileList, const std::vector <std::string>& outFiles, const std::string path) {
	using namespace cv;
	using namespace std;

	// Validate Path :
	bool isFile = path.rfind('.') < path.npos && path.rfind('.') != 0;
	bool isDirectory = path.find('\\') < path.npos;
	bool ifExists = exists(path);
	if (isFile && ifExists && !isDirectory) { 
		// Input is valid file :
		fileList.push_back(path);
	} else if (isDirectory && ifExists && !isFile) { 
		// Input is valid directory :
		for (const auto& dirItem : fs::directory_iterator(path)) {
			string pathName = dirItem.path().string();
			fileList.push_back(pathName);
		}
	} else { 
		// Input Path is invalid :
		cout << "Enter Valid Path" << endl;
		return;
	}

	// Get output file name list (w/o path and ext) to compare for checking duplicates
	vector<string> outputNames;
	for (string path : outFiles) {
		string oName = path.substr(path.rfind('\\') + 1, path.rfind('.') - path.rfind('\\') - 1);
		outputNames.push_back(oName);
	}

	// Pretty Print :
	cout << "       - HEVE STARVEY GENERATOR -       " << endl;
	cout << "----------------------------------------" << endl;

	if (printValidate) {
		if (!overrideDuplicates) {
			cout << "Validating Input Paths : Duplicate List : ";
		}
		else {
			cout << "Validating Input Paths : ";
		}
	}
	
	int duplicateCount = 0;
	// Validate Files in Path :
	for (auto path = fileList.begin(); path != fileList.end();) {
		// Get file extension :
		int extIndex = path->rfind(".");
		if (extIndex != path->npos) {
			// Make fExtension lowercase :
			transform(path->begin() + extIndex, path->end(), path->begin() + extIndex, ::tolower);
			if (validateExtension(*path)) {
				// Input Path(i) is a valid image files
				bool isDuplicate = false;
				string iName = path->substr(path->rfind('\\') + 1, path->rfind('.') - path->rfind('\\') - 1);
				if (!overrideDuplicates) {
					for (string oName : outputNames) {
						if (oName == iName) {
							isDuplicate = true;
							duplicateCount++;
							break;
						}
					}
				}
				// Test Duplicate
				if (!overrideDuplicates && isDuplicate) {
					// Pretty Print for first Duplicate
					if (printValidate && duplicateCount == 1) cout << endl << "----------------------------------------";

					// If duplicate and no override : Path is removed from valid list
					if (printValidate) cout << endl << iName;
					path = fileList.erase(path);
					continue;
				} else {
					// Path is Valid
					path++;
					continue;
				}
			}
			else {
				// Invalid Extension (Invalid Path is Removed)
				path = fileList.erase(path);
				continue;
			}
		}
		else {
			// No Extension (Invalid Path is Removed)
			path = fileList.erase(path);
			continue;
		}
	}
	
	if (printValidate && !overrideDuplicates && duplicateCount == 0) cout << "No Duplicate Files Found : ";
	if (printValidate && fileList.size() == 0 && overrideDuplicates) {
		cout << "No Input Files Found : ";
	}
	if (printValidate) cout << endl << "----------------------------------------" << endl;
}

// Parse valid file paths from directory input
inline void generateOutputFileList(std::vector <std::string>& fileList, const std::string path, bool& validOutput) {
	using namespace cv;
	using namespace std;

	// Innocent until proven guilty :
	validOutput = true;

	// Validate Output Path :
	bool isFile = path.find(".") < path.npos;
	bool isDirectory = path.find("\\") < path.npos;
	bool ifExists = exists(path);
	if (isDirectory && ifExists) {
		// Only directory is valid for output
		for (const auto& dirItem : fs::directory_iterator(path)) {
			string pathName = dirItem.path().string();
			fileList.push_back(pathName);
		}
	} else {
		cout << "Invalid Output Path" << endl;
		validOutput = false;
		return;
	}

	// Validate Files in Path :
	for (auto path = fileList.begin(); path != fileList.end();) {
		// Get file extension :
		int extIndex = path->rfind(".");
		if (extIndex != path->npos) {
			// Make fExtension lowercase :
			transform(path->begin() + extIndex, path->end(), path->begin() + extIndex, ::tolower);
			if (validateExtension(*path)) {
				path++;
				continue;
			} else {
				// Remove items without file extension :
				path = fileList.erase(path);
				continue;
			}
		}
	}
}

/* ------------------------------------- Helper Functions ------------------------------------- */

// Display output folder
inline void displayOutput(const std::vector<string>& outFiles) {
	
	if (!showOutput) return;
	
	cout << "----------------------------------------" << endl;
	cout << "Displaying Output Folder [Successes]" << endl;
	cout << "----------------------------------------" << endl;
	for (string path : outFiles) {
		string name = path.substr(path.rfind('\\') + 1, path.rfind('.') - path.rfind('\\') - 1);
		Mat outImage = imread(path);
		imshow(name, outImage);
		cout << "[Success] : " << name << endl;
		keyContinue();
	}
	cout << "----------------------------------------" << endl;

}

// Wait for key press and clear windows on key event
inline void keyContinue() {
	cv::waitKey();
	cv::destroyAllWindows();
}

// Check if file exists in directory
inline bool exists(const std::string& name) {
	// Source : https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-14-17-c
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

// Pretty-print file path list
// Name and Path do not have to be specified
inline void printFileList(const std::vector<std::string>& fileList, std::string name, std::string path) {
	
	if (name.empty()) {
		name = "File List";
	}

	std::cout << name << " : " << path;

	if (fileList.empty()) {
		std::cout << " : [EMPTY]" << endl;
	}
	else {
		std::cout << endl << "----------------------------------------" << endl;
	}
	
	for (string file : fileList) {
		std::cout << file << endl;
	}
	std::cout << "----------------------------------------" << endl;
}

// Check if path has valid image extension (.png, .jpg, .jpeg)
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

/* ------------------------------------ Fuck you --------------------------------------- */
