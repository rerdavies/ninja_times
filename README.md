# ninja_times
A small utility for analyzing CMake/ninja build times in .ninja_log files on a per-file basis. An indispensible tool if you are interested in profiling and optimizing build times.

Examples:
```
 $ ninja_times build/.ninja_log | head 
 283.862 react/build/index.html
 236.725 src/CMakeFiles/libpipedald.dir/WebServer.cpp.o
 192.733 src/CMakeFiles/jsonTest.dir/testMain.cpp.o
 158.956 src/CMakeFiles/libpipedald.dir/PiPedalSocket.cpp.o
 118.948 src/CMakeFiles/libpipedald.dir/PluginHost.cpp.o
 107.276 src/CMakeFiles/libpipedald.dir/Storage.cpp.o
  95.101 src/CMakeFiles/libpipedald.dir/PiPedalModel.cpp.o
  81.202 src/CMakeFiles/pipedaltest.dir/testMain.cpp.o
  76.963 src/CMakeFiles/jsonTest.dir/PromiseTest.cpp.o
  75.098 src/CMakeFiles/libpipedald.dir/AudioHost.cpp.o
```
```
$ ninja_times build/.ninja_log EditBoxTest* --history
src/test/CMakeFiles/CairoTest.dir/EditBoxTestPage.cpp.o
   2023-07-17 14:17:51  30.097
   2023-07-17 17:35:11  11.320
```
---

```
ninja_times: Anaylyzes .ninja_log files for per-file build times.
Copyright (c) 2023 Robin Davies.

Syntax: ninja_times filename [options]
   filename: path of a .ninja_log file.
Options:
   -h, --help Display this message.:
   --history  Display history of file build times.
   --match [pattern]
              A glob pattern that selects which files will be displayed.
              ? matches a character. * matches zero or more characters. 
              [abc] matches 'a', 'b' or 'c' [!abc] matches anything but.

ninja_times analyzes file build times in .ninja_log files.

By default, ninja_time displays the most recent build times for 
all files in the project. If a --match argument is provied, only 
files that match are displayed. 

The --history option allows you to display the history of build times 
for one or more files over time.

Examples:
     # display build times for all files in a project.
     ninja_times build/.ninja_log   # display build times for all files.

     # display recent build times for the file PiPedalModel.ccp.o
     ninja_times build/.ninja_log --match PiPedalModel.cpp.o --history```
---
## Build instructions

### Dependencies

Requires cmake and ninja-build to build the project. No other dependencies.
```
  apt install cmake ninja-build
```
### Building

Load the folder in Visual Studio Code, and use VSCodes's integrated CMake project support. 

Or.

Run the following command to configure build scripts:
```
     ./config.sh
```

Run the following command to build the project:
```
    ./make.sh
```

The built binary can be found at
```
build/src/ninja_times
```


