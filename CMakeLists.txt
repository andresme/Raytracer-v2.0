cmake_minimum_required(VERSION 3.5)
project(rt2)
find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        scene/object/objects.h
        scene/object/sphere.c
        scene/object/sphere.h
        scene/raytracer/raytracer.c
        scene/raytracer/raytracer.h
        util/io/fileUtils.c
        util/io/fileUtils.h
        util/math/mathUtils.c
        util/math/mathUtils.h
        util/struct/sharedStructs.h
        main.c scene/object/light.c scene/object/light.h scene/object/objects.c)

add_executable(rt2 ${SOURCE_FILES})

target_link_libraries(rt2 m pthread)
