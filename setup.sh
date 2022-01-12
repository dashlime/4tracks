#!/bin/sh

echo "Cloning JUCE framework ..."
mkdir Includes
cd Includes
git clone https://github.com/juce-framework/JUCE.git

echo "Cloning finished !"
echo "Building JUCE static lib ..."
cp -f ../juce_build_static_lib_cmake.txt JUCE/CMakeLists.txt
cd ..

