cmake_minimum_required(VERSION 3.16)

project(4tracks LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Xml)

add_executable(4tracks
    Source/Audio/JuceIncludes.h
    Source/Audio/audioclip.cpp Source/Audio/audioclip.h
    Source/Audio/track.cpp Source/Audio/track.h
    Source/Audio/clip.cpp Source/Audio/clip.h
    Source/Audio/midiclip.cpp Source/Audio/midiclip.h
    Source/Audio/mixerpositionableaudiosource.cpp Source/Audio/mixerpositionableaudiosource.h
    Source/Audio/project.cpp Source/Audio/project.h
    Source/Audio/resamplingpositionableaudiosource.cpp Source/Audio/resamplingpositionableaudiosource.h

    Source/Constants.h

    Source/Graphics/audiothumbnail.cpp Source/Graphics/audiothumbnail.h
    Source/Graphics/clipsgrid.cpp Source/Graphics/clipsgrid.h
    Source/Graphics/divisionsmarker.cpp Source/Graphics/divisionsmarker.h
    Source/Graphics/elidedlabel.cpp Source/Graphics/elidedlabel.h
    Source/Graphics/clip.cpp Source/Graphics/clip.h Source/Graphics/clip.ui
    Source/Graphics/timeline.cpp Source/Graphics/timeline.h
    Source/Graphics/positionbaroverlay.cpp Source/Graphics/positionbaroverlay.h
    Source/Graphics/selection.cpp Source/Graphics/selection.h
    Source/Graphics/selectionoverlay.cpp Source/Graphics/selectionoverlay.h
    Source/Graphics/settingsdialog.cpp Source/Graphics/settingsdialog.h Source/Graphics/settingsdialog.ui
    Source/Graphics/slider.cpp Source/Graphics/slider.h
    Source/Graphics/track.cpp Source/Graphics/track.h Source/Graphics/track.ui
    Source/Graphics/mainwindow.cpp Source/Graphics/mainwindow.h Source/Graphics/mainwindow.ui

    Source/Utils/projectsaver.cpp Source/Utils/projectsaver.h
    Source/Utils/utils_functions.h

    Source/main.cpp

    Source/Resources/Icons/icons.qrc
    Source/Resources/Stylesheets/stylesheets.qrc
)
target_include_directories(4tracks PUBLIC
    Includes/JUCE_Modules
    Source
)

target_compile_definitions(4tracks PUBLIC
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

target_link_directories(4tracks PUBLIC
    Libs
)

target_link_libraries(4tracks PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Xml
    juce_static_lib
    asound
    curl
    Qt6::Widgets
    dl
)
