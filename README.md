[![Tests](https://github.com/devanonyme-fr/4tracks/actions/workflows/cmake.yml/badge.svg?branch=master)](https://github.com/devanonyme-fr/4tracks/actions/workflows/cmake.yml)

# 4tracks
4tracks is a DAW written in C++. It's completly free and opensource. It's under GPLv3 license and it uses Qt6 and JUCE libraries. It is inspiring from Ableton software

## Special thanks

Thanks to [Ineside France](https://ineside.com/), for sponsoring this project with their VPS offers !

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites

For building this project, you need Qt6 library, that you can find [here](http://qt.io/). You can download Qt Creator too, it will help building project

Also, you will need CMake >= 3.16

### Building

To build this project, first clone it :

```
git clone https://github.com/devanonyme-fr/4tracks
```

Then, run setup.sh script, it will automatically download JUCE and googletest libraries and setup JUCE/CMakeLists.txt file

```
cd 4tracks
./setup.sh
```

#### Qt Creator

If you want to use Qt Creator, open it in by selecting 4tracks/CMakeLists.txt, and select Qt6 kit in the Projects panel.

#### CLion

For CLion users, you need to open root CMakeLists.txt, then set the `-DCMAKE_PREFIX_PATH=YOUR/QT6/PATH` compiler flag in File->Settings->Build, Execution, Deployment->CMake->CMake options

## Running the tests

In qt creator, just go to Test Results panel and start tests by clicking green arrow

For others IDEs, build and run 4tracks_test target

## Built With

* [Qt](https://qt.io/) - The GUI framework
* [JUCE](https://juce.com/) - Audio library
* [Google Test](https://github.com/google/googletest) - Google testing framework

## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests to us.

## Authors

* devanonyme - *Initial work* - [devanonyme](https://github.com/devanonyme-fr)

See also the list of [contributors](https://github.com/devanonyme-fr/4tracks/contributors) who participated in this project.

## License

This project is licensed under the GPLv3 License - see the [LICENSE.md](LICENSE.md) file for details
