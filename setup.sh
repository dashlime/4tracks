#!/bin/sh

echo "Cloning JUCE framework ..."
mkdir Includes
cd Includes
git clone https://github.com/juce-framework/JUCE.git
echo "Cloning finished !"

echo "Copying new CMakeLists to JUCE folder ..."
cp -f ../juce_build_static_lib_cmake.txt JUCE/CMakeLists.txt
echo "CMakeLists.txt configured !"

echo "Cloning googletest framework ..."
git clone https://github.com/google/googletest.git
echo "Cloning finished !"

echo "Setup finished ! Now open root CMakeLists.txt with your IDE and start building project, it will automatically build needed libraries"
