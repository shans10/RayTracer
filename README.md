# BTP

This is a simple pathtracer written in C++ as part of our B.Tech. CSE Final Year Project. It has multithreading support for rendering the image.  

*This project is based on Peter Shirley's series on raytracing.*

## Images
#### Running The Program
![Screenshot (3)](https://user-images.githubusercontent.com/28944997/120982202-0e11c700-c796-11eb-9b96-698d0f8a5727.png)  

*As we can see in the above image, the rendering process is using all available cores on the system and CPU utilization is 100%.*  
#### Output on Finish  
![Screenshot (7)](https://user-images.githubusercontent.com/28944997/120982213-110cb780-c796-11eb-9e8c-ec8bc9a46ed3.png)  

*The rendering process for the whole scene took more than 5 hours when rendering it with 10,000 samples per pixel.*

*NOTE: Users can change `main.cpp` file and reduce the samples per pixel or image aspect ratio and height/width to render a lower quality image which will speed up the rendering process. This is ideal when one wants to test the renderer themselves.*  

#### Final Scene  
![image](https://user-images.githubusercontent.com/28944997/120983973-d3109300-c797-11eb-8f28-567bffa2cb21.png)  

*This is the final image produced by the renderer.*


## Building The Project

`$ cmake -B build`

`$ cmake --build build`
  
On Windows, you can build either `debug` (the default) or `release` (the optimized version). To specify this, use the `--config <debug|release>` option.

## Running The Program
On Linux or OSX, from the terminal, run like this:  
`$ build/raytracer`  
  
On Windows, run like this(Use Powershell):  
`.\build\Debug\raytracer.exe`

or, run the optimized version (if you've built with `--config release`):  
`.\build\Release\raytracer.exe`

*When running the project it will ask the users:*  
`How many threads you want to use for rendering? (Default: Use All CPU Cores If No Input Provided) : `  
*Enter the number of cores you want to use for rendering or leave it blank and press enter to use all the available cores on your system.*


## Viewing The Output Image
After running the above command a new `output.ppm` file will be generated in the base folder which can be opened with any image viewer on Linux.

On Windows the image can be opened by using any of the following applications:

    1. GIMP
    2. Nomacs
    3. Adobe Photoshop

## External Libraries
1. *ThreadPool.h* - For Multithreading Support (This is a simple C++11 Thread Pool implementation by log4cplus hosted on github at [log4cplus/ThreadPool](https://github.com/log4cplus/ThreadPool)).  
2. *stb_image.h*  - For image manipulation (Hosted on github at [nothings/stb](https://github.com/nothings/stb)).
