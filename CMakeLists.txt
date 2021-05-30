#---------------------------------------------------------------------------------------------------
# CMake Build Configuration for the Raytracer
#

cmake_minimum_required ( VERSION 3.1.0 )

project ( RayTracer
  VERSION 3.0.0
  LANGUAGES CXX
)

# Set to c++11
set ( CMAKE_CXX_STANDARD 11 )

# Set Flags
set (CMAKE_CXX_FLAGS "-Wall -pthread")

# Source
set ( RAY
  src/main.h
  src/camera.h
  src/ray.h
  src/vec3.h
  src/color.h
  src/hittable.h
  src/hittable_list.h
  src/sphere.h
  src/material.h
  src/moving_sphere.h
  src/ThreadPool.h
  src/main.cc
)

# Executables
add_executable(raytracer      ${RAY})
