#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <stack>
#include <iterator>
#include "Log.h"

using namespace std;

// ------------------------------ Static Variables ------------------------------ //
map<string, bool> Log::idMap{ {"DEFAULT", false} };
bool Log::headless = false;
int Log::priorityLevel = 0;
stack<string> Log::keyStack;
/// <summary>
/// Log::stream << "Message" << Log::print_stream() || endlog;
/// </summary>
stringstream Log::stream;

// ------------------------------- "Streaming" --------------------------------- //
string Log::printStream() {
	if (headless) return ""; // If headless, cout will never be called
	string key = (keyStack.empty()) ? "DEFAULT" : keyStack.top(); // Handle edge case where keyStack is empty
	if (!idMap.at(key)) {
		cout << stream.str();
	}
	stream.str("");	// Clear sstream
	return "";		// Return padder value so function can be called inline
}

// --------------------------------- Printing ----------------------------------- //
/// <summary>
/// Print(message) if key is whitelisted.
/// If no key scope has been specified "DEFAULT" will be used.
/// </summary>
/// <param name="message"> Message to write </param>
void Log::print(string message) {
	if (headless) return;
	string key = (keyStack.empty()) ? "DEFAULT" : keyStack.top();
	if (!idMap.at(key)) {
		cout << message;
	}
}
/// <summary>
/// Print(message) with specific ID valid.
/// ID will be in scope only for this print.
/// Independent of keyStack
/// </summary>
/// <param name="key"></param>
/// <param name="message"></param>
void Log::print(string message, string key) {
	if (headless) return;
	idMap.try_emplace(key, false);
	if (!idMap.at(key)) {
		cout << message;
	}
}
/// <summary>
/// Println(message) if key is whitelisted
/// If no key scope has been specified "DEFAULT" will be used.
/// </summary>
/// <param name="message"></param>
void Log::println(string message) {				
	if (headless) return;
	string key = (keyStack.empty()) ? "DEFAULT" : keyStack.top();
	if (!idMap.at(key)) {
		cout << message << endl;
	}
}
/// <summary>
/// Println(message) with specific ID valid.
/// ID will be in scope only for this print.
/// Independent of keyStack
/// </summary>
/// <param name="key"></param>
/// <param name="message"></param>
void Log::println(string message, string key) {
	if (headless) return;
	idMap.try_emplace(key, false);
	if (!idMap.at(key)) {
		cout << message << endl;
	}
}


// -------------------------------- ID Managing --------------------------------- //
/// <summary> 
/// Push set a local ID/Key for all log statements until Log::popKey() is called. 
/// Structured as a stack to easily create blacklist scope in a specific function.
/// </summary>
/// <param name="key">key/id : Functions as Blacklist Target </param>
void Log::pushKey(string key) {
	if (headless) return;
	idMap.try_emplace(key, false);	// Try to Add Key To Key Map (Default of whitelisted)
	keyStack.push(key);
}
void Log::popKey() {
	if (headless) return;
	if(!keyStack.empty()) keyStack.pop();
}

void Log::blacklist(string key) {
	if (headless) return;
	idMap.try_emplace(key, true);	// Emplace if key not in map
	idMap.at(key) = true;			// Set Value
}

void Log::whitelist(string key) {
	if (headless) return;
	idMap.try_emplace(key, false);	// Emplace if key not in map
	idMap.at(key) = false;			// Set Value
}

// -------------------------------- Debugging ----------------------------------- //

void Log::printIds() {
	int count = 0;
	for (auto& x : idMap) {
		cout << "[" << x.first << ':' << x.second << ']';
		if (++count % 4 == 0) {
			cout << '\n';
		}
	}
	if (count % 4 != 0) cout << '\n';
}

void Log::printEmptyStack() {
	while (!keyStack.empty())
	{
		cout << ' ' << keyStack.top();
		keyStack.pop();
	}
}