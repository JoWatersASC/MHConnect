# MHConnect: Multithreaded Text, Audio, and Video Chat Application in C++
A video conferencing application built with C++ that enables real-time audio, video, and text communication between users. The application utilizes native C++ libraries and established frameworks for handling multimedia and networking components.

![Demo](MHConnectDemo.gif)
> *Top-left: Server(Windows), Bottom-left: Headless Client(WSL/Linux), Right: GUI Client(Windows)*

## Table of Contents
- [Setup](#setup)
- [Build](#build)
- [Usage](#usage)
- [Features](#features)

## Setup

Clone the repository:
```bash
git clone https://github.com/your-org/MHConnect.git
cd MHConnect
```

### Prerequisites
- C++17-compatible compiler  
- CMake ≥ 3.15  
- For GUI client: wxWidgets, OpenCV, RtAudio, GTK3 (Linux only)  
- For headless client: no extra GUI libraries required  

### Linux
Install all dependencies:
Run the setup script:
```bash
./setup.sh
```
Or manually install dependencies
```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake libasound2-dev \
  librtaudio-dev libopencv-dev libgtk-3-dev pkg-config \
  libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
  libnotify-dev libsecret-1-dev libgl1-mesa-dev libwxgtk3.2-dev
```

### Windows
1. Install Visual Studio 2019 (or newer) and Windows SDK
2. Install Dependencies
  - Option 1: Allow cmake to handle it, go to [CMake Options](#cmake-options)
  - Option 2: Use vcpkg  install dependencies:
   
      a. Install vcpkg
      ```powershell
      git clone https://github.com/Microsoft/vcpkg.git
      cd vcpkg
      .\bootstrap-vcpkg.bat
      .\vcpkg install wxwidgets rtaudio opencv gstreamer libnotify libsecret gtk3
      ```
      b. Configure CMake to use vcpkg toolchain:
      ```
      -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
      ```
      c. (Optional) If you have a system wxWidgets build, set:
      ```powershell
      setx WXWIN "C:\path\to\wxWidgets"
      ```

## Build

Create a build directory and run CMake.
### CMake Options
- `HEADLESS_BUILD` (ON/OFF): Build the client in headless mode (default: ON)  
- `BUILD_SHARED_LIBS` (ON/OFF): Build client library as shared (default: OFF)  

### Linux
```bash
mkdir build && cd build
cmake -DHEADLESS_BUILD=ON ..
cd client (or server)
make
```
To build GUI client, set:
```bash
cmake -DHEADLESS_BUILD=OFF ..
cd client (or server)
make
```

### Windows (Visual Studio)
```bat
mkdir build && cd build
cmake -DHEADLESS_BUILD=ON ..
cmake -S ..
```
*If you're getting 'main not found' or another error like it in Visual Studio, right-click the Visual Studio target 'MHConnectClient', click on propertes(or press alt+enter), go to Linker-> System->SubSystem, and change it to either Windows(GUI) or Console(Headless)*

For GUI client, use `-DHEADLESS_BUILD=OFF`.

## Usage

### Server
- Linux
```bash
./build/server/MHConnectServer
```
- Windows
```powershell
.\build\server\Debug\MHConnectServer.exe
```

### Client(Headless)
- Linux
```bash
./build/client/MHConnectClient <ip_address> <port>
```
- Windows
```bash
.\build\client\Debug\MHConnectClient.exe <ip_address> <port>
```
### Client(GUI)
- Linux
```bash
./build/client/MHConnectClient
```
- Windows
```bash
.\build\client\Debug\MHConnectClient.exe
```

## Features
- Real-time audio and video communication  
- Text chat  
- Cross-platform GUI with wxWidgets  
- Configurable headless client mode
