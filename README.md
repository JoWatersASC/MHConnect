# MHConnect: Multithreaded Text, Audio, and Video Chat Application in C++

A video conferencing application built with C++ that enables real-time audio, video, and text communication between users. The application utilizes native C++ libraries and established frameworks for handling multimedia and networking components.

## Features

- Real-time audio communication
- Video capture and display
- Text-based chat functionality
- Cross-platform GUI using wxWidgets
- Windows and POSIX-compliant networking

## Technologies Used

- **GUI Framework**: wxWidgets (>= 3.2)
- **Audio Processing**: RtAudio
- **Video Processing**: OpenCV
- **Networking**: POSIX-style C sockets & winsock
- **Build System**: CMake (>= 3.15)

## Prerequisites

- C++17 compatible compiler
- CMake (version 3.15 or higher)
- wxWidgets library (3.2)
- OpenCV library
- RtAudio library
- GTK3 (Linux only)

### Windows-Specific Requirements
- Visual Studio 2019 or newer
- Windows SDK
- Environment variable `WXWIN` pointing to wxWidgets installation
- OpenCV installation with path set in CMAKE_PREFIX_PATH

### Linux-Specific Requirements
- pkg-config
- GTK3 development libraries
- wxWidgets development libraries
- RtAudio development libraries

## Project Structure

```
project/
├── client/
│   ├── src/
│   │   ├── client.cpp
│   │   ├── audio.cpp
│   │   ├── video.cpp
│   │   ├── main_gui.cpp
│   │   └── start_gui.cpp
│   ├── include/
│   │   ├── client.h
│   │   ├── audio.h
│   │   ├── core.h
│   │   ├── gui.h
│   │   └── video.h
│   └── CMakeLists.txt
├── server/
│   ├── src/
│   │   ├── client.cpp
│   │   ├── server.cpp
│   │   └── chat_room.cpp
│   ├── include/
│   │   ├── chat_room.h
│   │   ├── server.h
│   │   └── client.h
│   └── CMakeLists.txt
├── common/
│   ├── CMakeLists.txt
│   └── [common header files]
└── CMakeLists.txt
```

## Building from Source

### Windows

1. Set up environment variables:
   ```bash
   set WXWIN=path/to/wxWidgets
   ```

2. Configure RtAudio:
   - Either set `DOWNLOAD_RTAUDIO=ON` to automatically download and build RtAudio
   - Or manually install RtAudio and set `RTAUDIO_PATH` and `RTAUDIO_INSTALLATION_PATH`

3. Configure OpenCV:
   - Set `CMAKE_PREFIX_PATH` to point to your OpenCV installation

4. Generate build files:
   ```bash
   mkdir build && cd build
   cmake -G "Visual Studio 16 2019" -A x64 ..
   ```

5. Build the project:
   ```bash
   cmake --build . --config Debug
   # or
   cmake --build . --config Release
   ```

### Linux

1. Install dependencies:
   ```bash
   # Ubuntu/Debian
   sudo apt-get install libwxgtk3.0-gtk3-dev librtaudio-dev libopencv-dev libgtk-3-dev pkg-config
   ```

2. Build the project:
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```

## Output Locations

- Executables: `build/bin/`
- Libraries: `build/libs/`

## Build Options

- `BUILD_SHARED_LIBS`: ON/OFF - Build shared libraries instead of static
- `DOWNLOAD_RTAUDIO`: ON/OFF (Windows only) - Automatically download and build RtAudio
- `wxWidgets_USE_STATIC`: ON/OFF (Windows only) - Use static wxWidgets libraries

## Current Status

The application currently supports:
- ✅ Audio input and output
- ✅ Text chat functionality
- ✅ Network communication
- ✅ Video capture and display
- ⚠️ Video streaming over network (In progress)

## Usage

1. Launch the application and server
2. Enter connection details (IP address and port)
3. Connect to a session
4. Use the GUI controls to send text messages (or compile to headless mode)

## Future Improvements

- Implementation of video streaming over network
- Enhancement of video quality
- Enhancement of audio quality
- Multiple chats on one device
- Multiple video streams to client