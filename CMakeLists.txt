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
  # External header files
  src/external/ThreadPool.h
  src/external/stb_image.h

  # Project header files
  src/aabb.h
  src/aarect.h
  src/box.h
  src/bvh.h
  src/camera.h
  src/color.h
  src/constant_medium.h
  src/hittable.h
  src/hittable_list.h
  src/main.h
  src/material.h
  src/moving_sphere.h
  src/perlin.h
  src/r_stb_image.h
  src/ray.h
  src/sphere.h
  src/texture.h
  src/vec3.h

  # Main cpp file
  src/main.cpp
)

# Executables
add_executable(raytracer      ${RAY})
