/* ------------------------------ Created By Daniel Hatakeyama ------------------------------ */
/* ------------------------------ Steve Harvey Image Generator ------------------------------ */

#include <iostream> 
#include <string>
#include <sstream>
#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <filesystem>
#include <vector>
#include <time.h>
#include "Image.h"
#include "Log.h"
namespace fs = std::filesystem; // Requires C++17

#define endlog Log::printStream()

#define minInt -1000000;

/* ---------------------------------------- Headers ---------------------------------------- */

inline void generateInputFileList(std::vector <std::string>& inFiles, const std::vector <std::string>& outFiles, const std::string path);
inline void generateOutputFileList(std::vector <std::string>& fileList, const std::string path, bool& isValid);
void printFileList(const std::vector<std::string>& fileList, std::string name = "", std::string path = "");
void generateProfileImages(const std::vector<std::string>& inFiles, const bool& validOutput);
inline void displayOutput(const std::vector<string>& outFiles);
inline void keyContinue();
inline bool exists(const std::string& name);
inline bool validateExtension(const std::string& path);
inline void logSettings();
void ioHandler(std::vector<std::string>& inFiles, std::vector<std::string>& outFiles, const std::string& inputPath, const std::string& outputPath, bool& validOutput);

/* ---------------------------------------- SETTINGS ---------------------------------------- */

// FileIO : File/Folder Input : Folder Output
const string inputPath = ".\\Resources\\Input\\";
const string outputPath = "\\Users\\djhat\\OneDrive\\Pictures\\Output_Heve\\";
const string failPath = ".\\Resources\\Failures\\"; // Optional : ! THERE ARE NO CHECKS ON THIS SO BE CAREFUL !

// Meta Settings :
const bool headless = false; // Run without UI
const Size profileSize = Size(720, 720);

// Input Settings
const bool deleteFailures = true;      // Deletes negative heve profiles from input path : Quickens Future Runs
const bool storeFailures = true;       // Only set if failPath is valid : ! THERE ARE NO CHEKS ON THIS SO BE CAREFUL !
const bool deleteSuccesses = false;    // KEEP AS FALSE : Delete positive heve profiles from input path : Use overrideDuplicates instead for 95% of cases

// Ouput Settings
const bool storeImage = true;		   // If store image in outputDestination
const bool overrideDuplicates = false; // Generate item even if duplicate already exists in output
const bool showOutput = true;	       // Show all contents of output 

// Display Settings
const bool displayLog = false;			// Display each image as it is generated
const bool showDebugImage = false ;	   // Debug draw all rectangle cascades
const bool showCascadeImage = true;    // Draw final cascades
const bool showProfileImage = true;    // Draw final image
const bool skipFails = false;		   // Draw negative matches

// Log Settings
inline void logSettings() {
	Log::headless = false;				// Headless Option - No Logging

	Log::whitelist("ERROR");			// Log Errors
	Log::whitelist("TITLE");			// Fancy Title Box
	Log::whitelist("VALIDATE_INPUT");	// Log Input Summary
	Log::whitelist("INPUT_LIST");		// Print File Input List
	Log::blacklist("OUTPUT_LIST");		// Print File Output List
	Log::whitelist("GENERATE");			// Generation Titles / Frame
	Log::whitelist("GENERATE_TITLE");	// Line Title of Generation Data
	Log::blacklist("GENERATE_INFO");	// All function info exept for Cascade and Face Data
	Log::whitelist("CASCADE");			// Log detectMultiScale cascades
	Log::blacklist("FACE");				// Log faceFeature function
	Log::blacklist("DRAW_FACE");		// Log drawFace function
	Log::whitelist("RESULT");			// Display Result of Generation

	// Use Log::printIds() to view all mapped blacklist/whitelist keys
}

/* ------------------------------------ [ TODO LIST ] ----------------------------------- /*
* generateProfileImage() [Crop PFP around face]
* ~Image() , ~Cascade() [Avoid Memory Leaks]
* Increase Speed further: Optimise construction
* Determine once and for all if you should be dynamically allocating to improve performance.
* Add Static Size Variable
* Move Helper / Condenser Functions to Seperate File
* MAKE LOG CLASS BASED OFF OF NAMESPACE AND USE INLINE OR SOMETHING LIKE THAT I THINK LOLOL
/* ---------------------------------------- Main ---------------------------------------- */

int main() {
	using namespace std;
	using namespace cv;
	utils::logging::setLogLevel(utils::logging::LogLevel::LOG_LEVEL_ERROR); // Log Level : Errors :

	// Setting Parity 
	logSettings();
	
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

	Log::pushKey("GENERATE");
	Log::print("\n");
	Log::print("----------------------------------------\n");
	Log::print("|        [ +++ Generating +++ ]        |\n");
	Log::print("----------------------------------------\n\n");
	Log::popKey();

	int count = 0;
	int successCount = 0;
	for (string path : inFiles) {

		// Log :
		Log::pushKey("GENERATE_INFO");

		Log::pushKey("GENERATE_TITLE");
		Log::stream << "Generating : [" << ++count << " / " << inFiles.size() << "] [" << successCount << " Positives]" << endl << endlog;
		Log::popKey(); // GENERATE_TITLE

		// GENERATE :
		Image image = Image(path);
		image.generateAll();
		image.drawDebugCascades();
		if (showDebugImage) image.drawDebugAllCascades();
		Log::popKey(); // GENERATE_INFO

		// Result :
		Log::pushKey("RESULT");
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
				Log::stream << "Storing Image : " << endlog;
				string writePath = outputPath + image.name + image.ext;
				imwrite(writePath, image.faceImage);
				Log::stream << "[-Successful-]" << endl << endlog;
			} else if (storeImage) {
				Log::stream << "Invalid Output Directory [Could not save image]" << endl << endlog;
			}

			if (deleteSuccesses) {
				if (!remove(path.c_str())) {
					Log::stream << "Deleting Image : " << endlog;
					Log::stream << "[-Successful-]" << endl << endlog;
				}
				else {
					Log::stream << "Error Deleting \"" << path << "\" from input path" << endl << endlog;
				}
			}

			successCount++; // Used for debug printing
			Log::print("[ === POSITIVE MATCH === ]\n\n");

		} else {
			// SHOW :
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

				Log::stream << "Storing Fail \"" << image.name << image.ext << "\" in \"" << outputPath << "\"" << endl << endlog;
			}
			// Delete Fail From Input
			if (deleteFailures) {
				if (!remove(path.c_str())) {
					Log::stream << "Deleting Negative Image : " << endlog;
					Log::stream << "[-Successful-]" << endl << endlog;
				}
				else {
					Log::stream << "Error Deleting \"" << path << "\" from input path" << endl << endlog;
				}
			}
			Log::print("[ === NEGATIVE MATCH === ]\n\n");

		}
		Log::popKey(); // RESULT
	}
	Log::pushKey("GENERATE");
	Log::print("----------------------------------------\n");
	Log::print("|   [ +++ Generation Complete +++ ]    |\n");
	Log::print("----------------------------------------\n\n");
	Log::popKey(); // GENERATE

}

// Container to generate, validate, parse input and output directory.
void ioHandler(std::vector<std::string>& inFiles, std::vector<std::string>& outFiles, const std::string& inputPath, const std::string& outputPath, bool& validOutput) {

	// Pretty Print Title :
	Log::pushKey("TITLE");
	Log::println("----------------------------------------");
	Log::println("|  [ =-= HEVE STARVEY GENERATOR =-= ]  |");
	Log::println("----------------------------------------");
	Log::popKey();

	// Get output first to check for duplicates :
	generateOutputFileList(outFiles, outputPath, validOutput);

	Log::pushKey("VALIDATE_INPUT");
	generateInputFileList(inFiles, outFiles, inputPath);
	Log::popKey();

	Log::pushKey("INPUT_LIST");
	printFileList(inFiles, "Input File List", inputPath);
	Log::popKey();

	Log::pushKey("OUTPUT_LIST");
	printFileList(outFiles, "Output File List", outputPath);
	Log::popKey();
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
		Log::println("[ERROR] Enter Valid Path", "ERROR");
		//cout << "Enter Valid Path" << endl;
		return;
	}

	// Get output file name list (w/o path and ext) to compare for checking duplicates
	vector<string> outputNames;
	for (string path : outFiles) {
		string oName = path.substr(path.rfind('\\') + 1, path.rfind('.') - path.rfind('\\') - 1);
		outputNames.push_back(oName);
	}

	if (!overrideDuplicates) {
		Log::print("Validating Input : [Remove Duplicates] ");
	} else {
		Log::print("Validating Input : [Keep Duplicates] ");
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
				// Test for Duplicate
				if (!overrideDuplicates && isDuplicate) {
					// Print Handling :
					
					// If duplicate and no override : Path is removed from valid list
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

	// Print Handling for Exceptions : key=VALIDATE_INPUT
	if (!overrideDuplicates) Log::stream << "[" << duplicateCount << " Duplicates] " << endlog;
	Log::stream << "[" << fileList.size() << " Valid] " << endlog;
	Log::print("\n----------------------------------------\n");
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
		Log::print("[ERROR] Invalid Output Path\n", "ERROR");
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

// Log file path list
// Name and Path do not have to be specified
void printFileList(const std::vector<std::string>& fileList, std::string name, std::string path) {

	if (name.empty()) {
		name = "File List";
	}
	Log::stream << name << " : " << path << endlog;

	if (fileList.empty()) {
		Log::print(" : [EMPTY]\n");
	}
	else {
		Log::print("\n----------------------------------------\n");
	}

	for (string file : fileList) {
		Log::stream << file << endl << endlog;
	}
	Log::print("----------------------------------------\n");
}

// Display and Log output folder
inline void displayOutput(const std::vector<string>& outFiles) {
	if (!showOutput) return; // Setting

	Log::stream << "----------------------------------------" << endl << endlog;
	Log::stream << "|     [ === Positive Matches === ]     |" << endl << endlog;
	Log::stream << "----------------------------------------" << endl << endlog;
	for (string path : outFiles) {
		string name = path.substr(path.rfind('\\') + 1, path.rfind('.') - path.rfind('\\') - 1);
		Mat outImage = imread(path);
		imshow(name, outImage);
		Log::stream << "[Success] : " << name << endl << endlog;
		keyContinue();
	}
	Log::print("----------------------------------------\n");
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
