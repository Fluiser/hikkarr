#!/bin/bash

pkg=""

if [ $1 == "pacman" ] 
then
	pkg="pacman -S "
elif [ $1 == "apt" ] 
then
	pkg="apt install "
else
	echo "what the hell"
	exit 1
fi

echo "$pkg"

$pkg gcc liboggz boost cmake ffmpeg yt-dlp python3 libsodium opus
