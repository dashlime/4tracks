cmake_minimum_required(VERSION 3.16)

project(4tracks LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6Widgets REQUIRED)
find_package(Qt6Core)
find_package(Qt6Gui)
find_package(Qt6Xml)

include_directories(
    Source
    Includes/JUCE/modules
)

add_compile_definitions(
    DEBUG=1
    JUCE_APP_VERSION=1.0.0
    JUCE_APP_VERSION_HEX=0x10000
    JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1
    JUCE_USE_MP3AUDIOFORMAT=1
    JucePlugin_Build_AAX=0
    JucePlugin_Build_AU=0
    JucePlugin_Build_AUv3=0
    JucePlugin_Build_RTAS=0
    JucePlugin_Build_Standalone=0
    JucePlugin_Build_Unity=0
    JucePlugin_Build_VST3=0
    JucePlugin_Build_VST=0
    _DEBUG=1
)

add_subdirectory(Includes/JUCE)
add_subdirectory(Includes/googletest)
add_subdirectory(Source)
add_subdirectory(Tests)