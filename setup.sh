#!/bin/bash

set -e

SUDO=''
if [ "$EUID" -ne 0 ]; then
  SUDO='sudo'
fi

if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS_ID=$ID
else
    echo "Cannot detect Linux distribution."
    exit 1
fi

echo "Linux distro: $OS_ID"

if [ "$OS_ID" = "ubuntu" ] || [ "$OS_ID" = "debian" ]; then
    echo "Installing dependencies for Debian/Ubuntu..."
    $SUDO apt-get update
    $SUDO apt-get install -y build-essential cmake \
		libasound2-dev \
		libopencv-dev \
		libgtk-3-dev libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libnotify-dev libsecret-1-dev libgl1-mesa-dev # GUI libraries

elif [ "$OS_ID" = "fedora" ]; then
    echo "Installing dependencies for Fedora..."
	$SUDO dnf install -y alsa-lib-devel \
		opencv-devel \
		gtk3-devel gstreamer1-devel gstreamer1-plugins-base-devel libnotify-devel libsecret-devel mesa-libGL-devel # GUI libraries

else
    echo "Unsupported Linux distribution: $OS_ID"
    exit 1
fi
