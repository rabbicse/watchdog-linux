# watchdog-linux
Monitor applications/processes in Linux


## Compile & Run Instructions
To compile and run this C++ code on Ubuntu, you can follow these steps:

- Open a terminal window and navigate to the directory where you have saved the C++ file.

- Install the nlohmann-json3 library by running the following command in the terminal:

```
sudo apt install nlohmann-json3-dev
```

Compile the C++ file by running the following command:

```
g++ -std=c++11 -I/usr/include/nlohmann -o watchdog watchdog.cpp
```

This command compiles the C++ file and creates an executable file called watchdog.

Run the executable file by running the following command:

```
./watchdog
```

This will run the C++ program and start monitoring the processes specified in the configuration file.