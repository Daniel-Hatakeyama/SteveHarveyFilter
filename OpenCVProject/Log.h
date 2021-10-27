#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <stack>

#pragma once

using namespace std;

/// <summary>
/// stream < ... < printStream()
/// </summary>
class Log {
public:
	static map<string, bool> idMap;		// Key, if_blacklisted
	static bool headless;				// Print Off//On Switch
	static stringstream stream;

	static int priorityLevel;			// TODO ????

private:
	static string defaultKey;			// Default ID
	static stack<string> keyStack;		// Keeps track of keys

public:

	// Set Local Key:
	static void pushKey(string key);
	static void popKey();

	// Key Based Printing:
	static string printStream();
	static void print(string message);
	static void print(string message, string key);
	static void println(string message);
	static void println(string message, string key);


	// Print Toggles:
	static void blacklist(string key);
	static void whitelist(string key);

	// Debugging Stuff:
	static void printIds();
	static void printEmptyStack();
};

