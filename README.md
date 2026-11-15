# Basic Keylogger

A minimal Windows keylogger written in C++. Captures keyboard input and writes it to
`keylogger.txt`.

> ⚠️ **For educational and personal use only.** Do not deploy this on any machine you
> do not own or have explicit written permission to monitor. Unauthorized use is
> illegal in most jurisdictions.

## Features

- Captures keystrokes system-wide via the Win32 low-level keyboard hook
- Writes captured input to a plain-text log (`keylogger.txt`)
- Single lightweight C++ binary — no external dependencies
- Ships with a Visual Studio solution (`Basic Keylogger.slnx`)

## Requirements

- **Windows** (uses `SetWindowsHookEx` / `WH_KEYBOARD_LL`)
- **Visual Studio 2022** with the *Desktop development with C++* workload

## Build

1. Open `Basic Keylogger.slnx` in Visual Studio 2022.
2. Set the configuration to **Release** and platform to **x64** (or **x86**).
3. Build the solution (`Ctrl+Shift+B`).
4. The executable is output to `x64\Release\` (or your chosen config folder).

## Usage

1. Run the compiled executable.
2. Type on the keyboard — input is logged in real time.
3. The log is written to `keylogger.txt` in the working directory.

To stop, close the process (or end it from Task Manager).

## How it works

The program installs a low-level keyboard hook (`WH_KEYBOARD_LL`) with
`SetWindowsHookEx`, which receives every key event before it reaches the target
application. Each event is translated and appended to the log file. The process keeps a
message loop alive so the hook stays installed.

## Notes

- The log file is plain text and is **not** encrypted or hidden — it sits in the working
  directory as `keylogger.txt`.
- Some antivirus/EDR products flag low-level keyboard hooks as suspicious behavior. This
  is expected for this class of program.
- No persistence, network exfiltration, or obfuscation is included — it's a *basic*
  keylogger by design.

## License

No license is specified in this repository. All rights reserved by the author unless
stated otherwise.

---

> **Disclaimer:** This project is provided for learning about Windows input hooks and
> Win32 APIs. The author is not responsible for any misuse. Only run it on systems you
> own.
