#!/bin/sh

echo "Cloning JUCE framework ..."
mkdir Includes
cd Includes
git clone https://github.com/juce-framework/JUCE.git

echo "Cloning finished !"
echo "Copying new CMakeLists to JUCE folder ..."
cp -f ../juce_build_static_lib_cmake.txt JUCE/CMakeLists.txt
echo "Setup finished ! Now open root CMakeLists.txt with qt creator (or another IDE with Qt >= 6), it will automatically build JUCE library"
