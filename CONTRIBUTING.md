# Contributing to 4tracks

First, thank you for taking the time to contribute and help 4tracks project !

Here are a set guidelines for contributing to 4tracks

## 4tracks design

4tracks design is very simple : 
First, the project is divided in two main parts: Audio part, that handles all the audio engine and app logic, and Graphics part, that handles UI features:
for Audio part, the structure is like:
```
Audio::Project
  |---- Audio::Track (track 1)
        |---- Audio::Clip (clip 1)
        |---- Audio::Clip (clip 2)
```
for Graphics part, it is a little different:
```
MainWindow
  |---- Graphics::Timeline
        |---- Graphics::Track (track 1)
        |---- Graphics::ClipsGrid
              |---- Graphics::Clip (clip 1)
              |---- Graphics::Clip (clip 2)
```
              
## Report bugs

All bug reports must be done in Github Issues system

## Submit new feature / enhancement

You can submit new feature / enhancement in Github Issues, with clear title and description

## Submitting pull request

When your feature is ready, make sure you have executed tests, then submit new pull request
