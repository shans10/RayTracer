# BTP

This is a simple pathtracer written in C++ as part of our B.Tech. CSE Final Year Project.

## Building and Running

`$ cmake -B build`

`$ cmake --build build`


On Windows, you can build either `debug` (the default) or `release` (the optimized version). To specify this, use the `--config <debug|release>` option.

## Running The Program
On Linux or OSX, from the terminal, run like this:

`$ build/raytracer > image.ppm`

On Windows, run like this(Use Powershell):

`.\build\Debug\raytracer > image.ppm`

or, run the optimized version (if you've built with `--config release`):

`.\build\Release\raytracer > image.ppm`

*Note*: Normal Powershell may give issues with image.ppm file so preferably run the program in Powershell Core(The Cross Platform version of Powershell).
